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
#include "oops/arrayOop.inline.hpp"
#include "oops/oop.hpp"
#include "oops/oop.inline.hpp"
#include "gc/serial/markSweep.hpp"
#include "gc/serial/markSweep.inline.hpp"
#include "memory/iterator.inline.hpp" 

int sockfd_remote = -1;

std::mutex lock_remote;

RemoteSpace::RemoteSpace() : ContiguousSpace() {
	if(sockfd_remote < 0){
		printf("space socket\n");
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
	        puts("connect error");
	    }
	}
	int one = 1;
	setsockopt(sockfd_remote, SOL_TCP, TCP_NODELAY, &one, sizeof(one));

    int newsize = 131072;
	setsockopt(sockfd_remote, SOL_SOCKET, SO_SNDBUF, &newsize, sizeof(newsize));
	setsockopt(sockfd_remote, SOL_SOCKET, SO_RCVBUF, &newsize, sizeof(newsize));
    signal(SIGUSR1, getandsend_roots);
}

RemoteSpace::~RemoteSpace(){
    //close(sockfd_remote);
}


void RemoteSpace::initialize(MemRegion mr, bool clear_space, bool mangle_space) {
    struct msg_initialize * msg = (struct msg_initialize*) calloc(1,sizeof(struct msg_initialize));
    HeapWord* mr_start = mr.start();
	printf("Start: %p\n",  mr_start);
    size_t  mr_word_size = mr.word_size();
	msg->msg_type.type = 'i';
    msg->mr_start =  mr_start;
    msg->mr_word_size =  mr_word_size;
    msg->obj_array_base = (uint32_t) arrayOopDesc::base_offset_in_bytes(T_OBJECT);
	msg->obj_array_length = arrayOopDesc::length_offset_in_bytes();
    msg->clear_space = clear_space;
    msg->mangle_space = mangle_space;
    write(sockfd_remote, msg, sizeof(struct msg_initialize));

    std::free(msg);
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

    /*----------------*/
    /* Ce bout de code sert à trouver et  afficher les racines du tas pour le gc*/
    /*à terme, la fonction getandsend_roots est appelée par le RemoteSpace via des sockets ou un signal*/
    /*counter++;
    if(counter >= 2000 && roots){
        getandsend_roots(0);
        roots = false;
    }*/
    /*-------------*/
    //printf("klass: %s\n", klass->external_name());

    //printf("Avant\n");

    struct msg_par_allocate_klass* msg = (struct msg_par_allocate_klass*) calloc(1,sizeof(struct msg_par_allocate_klass));
	msg->msg_type.type = 'k';
    msg->word_size =  word_size;
    msg->klass = (unsigned long) klass;
    lock_remote.lock();
    write(sockfd_remote, msg, sizeof(struct msg_par_allocate_klass));

    struct msg_alloc_response* result = (struct msg_alloc_response*) malloc(sizeof(struct msg_alloc_response));
    read(sockfd_remote, result, sizeof(msg_alloc_response));
    if(result->send_metadata){
        struct msg_klass_data* klass_data = (struct msg_klass_data*) malloc(sizeof(struct msg_klass_data));
		klass_data->length = 0;
        klass_data->layout_helper = klass->layout_helper();
        OopMapBlock* field_array = NULL;
        if(klass->is_instance_klass()){
            klass_data->klasstype = instance;
            InstanceKlass* iklass = (InstanceKlass*) klass;
            klass_data->length = iklass->nonstatic_oop_map_count();
            field_array = iklass->start_of_nonstatic_oop_maps();
            write(sockfd_remote, klass_data, sizeof(struct msg_klass_data));
            write(sockfd_remote, field_array, klass_data->length*sizeof(OopMapBlock));
        }
        if(klass->is_objArray_klass()){
            klass_data->klasstype = objarray;
            ObjArrayKlass* oklass = (ObjArrayKlass*) klass;
            klass_data->base_klass = (unsigned long)oklass->element_klass();
            write(sockfd_remote, klass_data, sizeof(struct msg_klass_data));
            }
        if(klass->is_typeArray_klass()){
            klass_data->klasstype = typearray;
            TypeArrayKlass* tklass = (TypeArrayKlass*) klass;
            klass_data->basetype = tklass->element_type();
            write(sockfd_remote, klass_data, sizeof(struct msg_klass_data));
        }
        std::free(klass_data);
    }
    lock_remote.unlock();
    HeapWord* allocated = result->ptr;
   // if(allocated == nullptr){
   //     collect();
   // }
    if(allocated != nullptr){
        uint64_t iptr = (uint64_t)allocated;
        *((uint64_t*)(iptr - KLASS_OFFSET)) = msg->klass;
        *((uint32_t*)(iptr - SIZE_OFFSET)) = (uint32_t) word_size;
        *((uint32_t*)(iptr - 16)) = 0xdeadbeef;
    }
    std::free(msg);
    std::free(result);
    //printf("Après\n");
    return allocated;
}


void RemoteSpace::set_end(HeapWord* value){
    struct msg_set_end * msg = (struct msg_set_end*) calloc(1,sizeof(struct msg_set_end));
	msg->msg_type.type = 'e';
    msg->value = value;
    write(sockfd_remote, msg, sizeof(struct msg_set_end));
    std::free(msg);
}

size_t RemoteSpace::used() const{
	//printf("Start used\n");
	opcode tag;
    tag.type = 'u';
    lock_remote.lock();
    write(sockfd_remote, &tag, 8);
    size_t *result = (size_t*) malloc(sizeof(size_t));
    read(sockfd_remote, result, sizeof(size_t));
    lock_remote.unlock();
	//printf("End used: %lu\n", *result);

    return *result;
}


unsigned long first_root;

void getandsend_roots(int sig) {
	opcode tag;
    tag.type = 'r';

    RootMark rm;
    rm.do_it();
    int array_length = rm.getArraySize();
    unsigned long *root_array= rm.rootArray();
    lock_remote.lock();
    write(sockfd_remote, &tag,8);
    write(sockfd_remote, &array_length, sizeof(int));
	int flag = 1;
	setsockopt(sockfd_remote, IPPROTO_TCP, O_NDELAY, (char *) &flag, sizeof(int));
    write(sockfd_remote, root_array, sizeof(unsigned long)*array_length );
	setsockopt(sockfd_remote, IPPROTO_TCP, O_NDELAY, (char *) &flag, sizeof(int));
    lock_remote.unlock();
}

void getandsend_roots(){
    RootMark rm;
	printf("Starting to collect roots\n");
    rm.do_it();
    uint64_t array_length = (uint64_t) rm.getArraySize();
	printf("Length: %lu\n", array_length);
    uint64_t*root_array= rm.rootArray();
	first_root = *root_array;
    write(sockfd_remote, &array_length, sizeof(uint64_t));
    int res = write(sockfd_remote, root_array, sizeof(uint64_t)*array_length );
	printf("End of roots collection ; res: %d\n", res);
}

void RemoteSpace::safe_object_iterate(ObjectClosure* blk){
    return;
};

void RemoteSpace::print_on(outputStream* st) const{
    return;
}

void RemoteSpace::collect() {
		printf("Start of collection\n");
    	fsync(fd_for_heap);

		struct msg_collect* msg = (struct msg_collect*) calloc(1, sizeof(struct msg_collect));
		msg->msg_type.type = 'c';
		msg->base =(uint64_t) Universe::narrow_oop_base();
		msg->shift = Universe::narrow_oop_shift();
    	lock_remote.lock();
		write(sockfd_remote, msg, sizeof(struct msg_collect));  // on envoie la base et le shift pour la conversion narrowoop en oop
    	getandsend_roots();
    	uint64_t* ack = (uint64_t*) malloc(sizeof(uint64_t));
    	int res = read(sockfd_remote, ack, sizeof(uint64_t));
		printf("Ack: %lu\n", *ack);
    	lock_remote.unlock();
		printf("End of collection\n");
}
