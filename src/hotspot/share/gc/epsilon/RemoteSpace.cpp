//
// Created by adam on 4/11/22.
//

#include "RemoteSpace.hpp"
#include "rpcMessages.hpp"
#include <arpa/inet.h>
#include <stdlib.h>
#include "gc/epsilon/roots.hpp"
#include <string.h>
#include <csignal>
#include "oops/objArrayKlass.hpp"
#include "oops/instanceMirrorKlass.hpp"
#include "oops/arrayOop.inline.hpp"
#include "classfile/javaClasses.hpp"
#include "oops/oop.hpp"
#include "oops/oop.inline.hpp"
#include "gc/serial/markSweep.hpp"
#include "gc/serial/markSweep.inline.hpp"
#include "memory/iterator.inline.hpp"
#include "gc/shared/referenceProcessor.inline.hpp"
#include "gc/epsilon/gc_helper.hpp"
#include <fcntl.h>

#include <unistd.h>

int sockfd_remote = -1;

std::mutex lock_remote;
std::mutex lock_collect;
std::mutex lock_allocbuffer;
AllocationBuffer* alloc_buffer;
uint64_t free_space;
uint64_t cap;
uint64_t used_;
int fd_for_heap;
int shm_fd;
void* epsilon_sh_mem;



RemoteSpace::RemoteSpace() : ContiguousSpace() {
	if(sockfd_remote < 0){
	    sockfd_remote = socket(AF_INET, SOCK_STREAM, 0);
	    if (sockfd_remote < 0)
	    {
	        perror ("socket");
	        exit (EXIT_FAILURE);
	    }
	    server.sin_family = AF_INET;
	    server.sin_addr.s_addr = inet_addr("127.0.0.1");
	    server.sin_port = htons( RSPACE_PORT );
	    if (connect(sockfd_remote, (struct sockaddr *)&server , sizeof(server)) < 0)
	    {
	        puts("connect error 1");
	    }
	}
	int one = 1;
	setsockopt(sockfd_remote, SOL_TCP, TCP_NODELAY, &one, sizeof(one));

    int newsize = 131072;
	setsockopt(sockfd_remote, SOL_SOCKET, SO_SNDBUF, &newsize, sizeof(newsize));
	setsockopt(sockfd_remote, SOL_SOCKET, SO_RCVBUF, &newsize, sizeof(newsize));
    signal(SIGUSR1, start_collect_sig);
    signal(SIGUSR2, end_collect_sig);

	epsilon_sh_mem = setup_shm();
	
	gchelper.initialize();
}

RemoteSpace::~RemoteSpace(){
    //close(sockfd_remote);
}


void RemoteSpace::initialize(MemRegion mr, bool clear_space, bool mangle_space) {
	_mr = mr;
    struct msg_initialize msg;
    HeapWord* mr_start = mr.start();
	printf("Start: %p\n",  mr_start);
    size_t  mr_word_size = mr.word_size();
	msg.msg_type.type = 'i';
    msg.mr_start =  mr_start;
    msg.mr_word_size =  mr_word_size;
    msg.obj_array_base = (uint32_t) arrayOopDesc::base_offset_in_bytes(T_OBJECT);
	msg.obj_array_length = arrayOopDesc::length_offset_in_bytes();
    msg.clear_space = clear_space;
    msg.mangle_space = mangle_space;
	msg.pid = getpid();
    write(sockfd_remote, &msg, sizeof(struct msg_initialize));
	//printf("\nPID: %d\n", getpid());
	//sleep(5);
	//
	
	RemoteSpace::poule = nullptr;
	cap = mr_word_size*8;
	free_space = cap;
	used_ = 0;
	
	alloc_buffer = (AllocationBuffer*) malloc(sizeof(AllocationBuffer));
	alloc_buffer->initialize(sockfd_remote);
	
}

void RemoteSpace::post_initialize(){
	_span_based_discoverer.set_span(_mr);
	rp = new ReferenceProcessor(&_span_based_discoverer);
	gchelper.set_rp(rp);
}

HeapWord *RemoteSpace::par_allocate(size_t word_size) {
    counter++;
    struct msg_par_allocate * msg = (struct msg_par_allocate*) calloc(1,sizeof(struct msg_par_allocate));

	msg->msg_type.type = 'a';
    msg->word_size =  word_size;

    lock_remote.lock();
    write(sockfd_remote, msg, sizeof(struct msg_par_allocate));

    HeapWord** result = (HeapWord**) malloc(sizeof(HeapWord*));
    read(sockfd_remote, result, sizeof(HeapWord*));
    lock_remote.unlock();
    std::free(msg);
    HeapWord * allocated = *result;
    //if(allocated == nullptr){
    //    collect();
    //}
    std::free(result);
    return allocated;
}


HeapWord *RemoteSpace::par_allocate_klass(size_t word_size, Klass* klass) {
	HeapWord* allocated;
	//printf("Test alloc wait lock");
	//printf("Tentative d'alloc\n");
	Thread* thread = Thread::current();
	
	if(thread->pseudo_tlab() != nullptr){
		//PseudoTLAB* ptlab = (PseudoTLAB) thread->pseudo_tlab();
		//ptlab()
	}

	lock_collect.lock();
	lock_remote.lock();
#if ALLOC_BUFFER
	if(alloc_buffer->is_candidate(word_size)  ){
		lock_allocbuffer.lock();
		allocated = alloc_buffer->allocate(word_size);
		if(allocated == nullptr){
			allocated = alloc_buffer->add_bucket_and_allocate(word_size);
		}
		lock_allocbuffer.unlock();
	}
	else{
#endif
		//lock_remote.lock();
		struct msg_par_allocate_klass msg ;
		msg.msg_type.type = 'k';
    	msg.word_size = static_cast<uint32_t>(word_size);
    	msg.klass = static_cast<uint32_t>((uint64_t) klass);
		write(sockfd_remote, &msg, sizeof(struct msg_par_allocate_klass));
    	struct msg_alloc_response result ;
    	read(sockfd_remote, &result, sizeof(msg_alloc_response));
		//lock_remote.unlock();
		allocated = result.ptr;
#if ALLOC_BUFFER
	}
#endif


    if(allocated != nullptr){
#if GCHELPER
		if(klass->is_instance_klass() &&( ((InstanceKlass*)klass)->is_reference_instance_klass() || ((InstanceKlass*)klass)->is_mirror_instance_klass() )){
			gchelper.add_root(allocated);
		}
		//lock_remote.unlock();
#endif
		used_ += word_size*8 + HEADER_OFFSET;
        uint64_t iptr = (uint64_t)allocated;
		uint32_t short_klass = static_cast<uint32_t>((uint64_t)klass);
		if(klass->is_objArray_klass()){
			*((uint32_t*)(iptr - KLASS_OFFSET)) = short_klass + 1;
		}else if(klass->is_typeArray_klass()){
			*((uint32_t*)(iptr - KLASS_OFFSET)) = 42;
		}else if(klass->is_instance_klass()){
			*((uint32_t*)(iptr - KLASS_OFFSET)) = short_klass;
		}
        *((uint32_t*)(iptr - SIZE_OFFSET)) = (uint32_t) word_size;

		//return allocated;
    }
	lock_remote.unlock();
	lock_collect.unlock();

	//printf("Alloc: %p\n", allocated);
    return allocated;
}




void RemoteSpace::set_end(HeapWord* value){
    struct msg_set_end msg;
	msg.msg_type.type = 'e';
    msg.value = value;
    write(sockfd_remote, &msg, sizeof(struct msg_set_end));
}

size_t RemoteSpace::used() const{
	return cap - free_space + used_;
}

size_t used_glob(){
	return cap - free_space + used_;
}

size_t RemoteSpace::capacity() const{
    return cap;
}



void getandsend_roots(){
    RootMark rm;
    rm.do_it();
    uint64_t array_length = (uint64_t) rm.getArraySize();
    uint64_t*root_array= rm.rootArray();
    write(sockfd_remote, &array_length, sizeof(uint64_t));
	//printf("Send roots, size: %lu\n", array_length);
    int res = write(sockfd_remote, root_array, sizeof(uint64_t)*array_length );
}

#if GCHELPER
void RemoteSpace::getandsend_helper(){
    gchelper.do_it();
    uint64_t array_length = (uint64_t) gchelper.getArraySize();
    uint64_t*helper_array = gchelper.helperArray();
    write(sockfd_remote, &array_length, sizeof(uint64_t));
	//printf("Send helper roots, size: %lu\n", array_length);
    int res = write(sockfd_remote, helper_array, sizeof(uint64_t)*array_length );
}
#endif


void RemoteSpace::safe_object_iterate(ObjectClosure* blk){return;};

void RemoteSpace::print_on(outputStream* st) const{return;}

void RemoteSpace::stw_pre_collect() {

	printf("Start of collection\n");
	if(!rp_init){
		rp_init = true;
		post_initialize();
	}

	size_t start_used  = used();
	printf("cap: %lu, used: %lu\n", cap, used_);

	//struct timeval start, end;
	//gettimeofday(&start, NULL);
	lock_collect.lock();
	fsync(fd_for_heap);

	ioctl(fd_for_heap, 0, 1);

	struct msg_collect msg;
	msg.msg_type.type = 'c';
	msg.base =(uint64_t) Universe::narrow_oop_base();
	msg.shift = Universe::narrow_oop_shift();
	msg.static_base = (uint32_t) InstanceMirrorKlass::offset_of_static_fields();
	msg.static_length = java_lang_Class::static_oop_field_count_offset();

	lock_remote.lock();

	write(sockfd_remote, &msg, sizeof(struct msg_collect));  // on envoie la base et le shift pour la conversion narrowoop en oop
	getandsend_roots();
#if GCHELPER
	getandsend_helper();
#endif
	lock_remote.unlock();

}

void start_collect_sig(int sig) {
		EpsilonHeap::heap()->vm_collect_impl();
}

void end_collect_sig(int sig) {
	free_space = 0;
	used_ = 0;
	//read(sockfd_collect, &free_space, sizeof(uint64_t));
	lock_allocbuffer.lock();
	alloc_buffer->free_all();
	lock_allocbuffer.unlock();
	ioctl(fd_for_heap, 0, 0);
	opcode msg;
	msg.type = 'f';
	lock_remote.lock();
	write(sockfd_remote, &msg, sizeof(uint64_t));
	read(sockfd_remote, &free_space, sizeof(uint64_t));
	lock_remote.unlock();

	lock_collect.unlock();
	//gettimeofday(&end, NULL);
	//printf("[téléGC] Collection:  Time: %lds;  Used before: %lu; Used after: %lu\n", end.tv_sec - start.tv_sec, start_used, used() );
	printf("\n");
	printf("[téléGC] Collection: Used after: %lu\n",  used_glob() );
}




#if DEADBEEF
void RemoteSpace::deadbeef_area(uint64_t addr, int size, int type){
	uint64_t lorem;
	if(type == 42){
		lorem = 0xdeadbeefdeadbeef;
	}else{
		lorem = 0xcafebabecafebabe;
	}
	for(int i=0; i<size; i+=8){
		*((uint64_t*) (addr + i)) = lorem;
	}
}
#endif




void* setup_shm(){
	void* ret_addr;
	shm_fd = shm_open(SHM_NAME, O_RDWR, 0600);

 	if (shm_fd == -1)
 	    exit(-1);

 	if (ftruncate(shm_fd, SHM_SIZE) == -1)
 	    exit(-1);

 	/* Map the object into the caller's address space. */

 	ret_addr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
 	if (ret_addr == MAP_FAILED)
		exit(-1);

	return ret_addr;
}
