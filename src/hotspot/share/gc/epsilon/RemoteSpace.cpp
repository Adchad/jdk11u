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


RemoteSpace::RemoteSpace() : ContiguousSpace() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons( 42069 );

    if (connect(sockfd, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
    }

    signal(SIGUSR1, getandsend_roots);
}


void RemoteSpace::initialize(MemRegion mr, bool clear_space, bool mangle_space) {
    struct msg_initialize * msg = (struct msg_initialize*) malloc(sizeof(struct msg_initialize));
    HeapWord* mr_start = mr.start();
	printf("Start: %lu\n", (unsigned long) mr_start);
    size_t  mr_word_size = mr.word_size();
    char msg_tag = 'i';
    msg->mr_start =  mr_start;
    msg->mr_word_size =  mr_word_size;
    msg->obj_array_base = (uint32_t) arrayOopDesc::base_offset_in_bytes(T_OBJECT);
	msg->obj_array_length = arrayOopDesc::length_offset_in_bytes();
    msg->clear_space = clear_space;
    msg->mangle_space = mangle_space;
    lock.lock();
    write(sockfd, &msg_tag, 1);
    write(sockfd, msg, sizeof(struct msg_initialize));
    lock.unlock();

    std::free(msg);
}

HeapWord *RemoteSpace::par_allocate(size_t word_size) {
    counter++;
    struct msg_par_allocate * msg = (struct msg_par_allocate*) malloc(sizeof(struct msg_par_allocate));
    char msg_tag = 'a';
    msg->word_size =  word_size;

    lock.lock();
    write(sockfd, &msg_tag, 1);
    write(sockfd, msg, sizeof(struct msg_par_allocate));

    HeapWord** result = (HeapWord**) malloc(sizeof(HeapWord*));
    read(sockfd, result, sizeof(HeapWord*));
    lock.unlock();
    std::free(msg);
    HeapWord * allocated = *result;
    if(allocated == nullptr){
        collect();
    }
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
    struct msg_par_allocate_klass* msg = (struct msg_par_allocate_klass*) malloc(sizeof(struct msg_par_allocate_klass));
    char msg_tag = 'k';
    msg->word_size =  word_size;
    msg->klass = (unsigned long) klass;
    lock.lock();
    write(sockfd, &msg_tag, 1);
    write(sockfd, msg, sizeof(struct msg_par_allocate_klass));

    struct msg_alloc_response* result = (struct msg_alloc_response*) malloc(sizeof(struct msg_alloc_response));
    read(sockfd, result, sizeof(msg_alloc_response));
    if(result->send_metadata){
        struct msg_klass_data* klass_data = (struct msg_klass_data*) malloc(sizeof(struct msg_klass_data));
        klass_data->name_length = strlen(klass->external_name());
		klass_data->length = 0;
        klass_data->layout_helper = klass->layout_helper();
        OopMapBlock* field_array = NULL;
        if(klass->is_instance_klass()){
            klass_data->klasstype = instance;
            InstanceKlass* iklass = (InstanceKlass*) klass;
            klass_data->length = iklass->nonstatic_oop_map_count();
            field_array = iklass->start_of_nonstatic_oop_maps();
            write(sockfd, klass_data, sizeof(struct msg_klass_data));
            write(sockfd, field_array, klass_data->length*sizeof(OopMapBlock));
        }
        if(klass->is_objArray_klass()){
            klass_data->klasstype = objarray;
            ObjArrayKlass* oklass = (ObjArrayKlass*) klass;
            klass_data->base_klass = (unsigned long)oklass->element_klass();
            write(sockfd, klass_data, sizeof(struct msg_klass_data));
            }
        if(klass->is_typeArray_klass()){
            klass_data->klasstype = typearray;
            TypeArrayKlass* tklass = (TypeArrayKlass*) klass;
            klass_data->basetype = tklass->element_type();
            write(sockfd, klass_data, sizeof(struct msg_klass_data));
        }
        write(sockfd, klass->external_name(), klass_data->name_length);
        std::free(klass_data);
    }
    lock.unlock();
    HeapWord* allocated = result->ptr;
    if(allocated == nullptr){
        collect();
    }
    else{
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
    struct msg_set_end * msg = (struct msg_set_end*) malloc(sizeof(struct msg_set_end));
    char msg_tag = 'e';
    msg->value = value;

    lock.lock();
    write(sockfd, &msg_tag, 1);
    write(sockfd, msg, sizeof(struct msg_set_end));
    lock.unlock();

    std::free(msg);
}

size_t RemoteSpace::used() const{
    char msg_tag = 'u';
    lock.lock();
    write(sockfd, &msg_tag, 1);
    size_t *result = (size_t*) malloc(sizeof(size_t));
    read(sockfd, result, sizeof(size_t));
    lock.unlock();

    return *result;
}


unsigned long first_root;

void getandsend_roots(int sig) {
    char msg_tag = 'r';

    RootMark rm;
    rm.do_it();
    int array_length = rm.getArraySize();
    unsigned long *root_array= rm.rootArray();
    lock.lock();
    write(sockfd, &msg_tag,1);
    write(sockfd, &array_length, sizeof(int));
    write(sockfd, root_array, sizeof(unsigned long)*array_length );
    lock.unlock();
}

void getandsend_roots(){
    RootMark rm;
	printf("Starting to collect roots\n");
    rm.do_it();
    int array_length = rm.getArraySize();
    unsigned long *root_array= rm.rootArray();
	first_root = *root_array;
    write(sockfd, &array_length, sizeof(int));
    write(sockfd, root_array, sizeof(unsigned long)*array_length );
	printf("End of roots collection\n");
}

void RemoteSpace::safe_object_iterate(ObjectClosure* blk){
    return;
};

void RemoteSpace::print_on(outputStream* st) const{
    return;
}

void RemoteSpace::collect() {

    	char msg_tag = 'c';
    	fsync(fd_for_heap);
    	lock.lock();
    	write(sockfd, &msg_tag, 1);

		struct msg_collect* msg = (struct msg_collect*) malloc(sizeof(struct msg_collect));
		msg->base =(uint64_t) Universe::narrow_oop_base();
		msg->shift = Universe::narrow_oop_shift();
		write(sockfd, msg, sizeof(struct msg_collect));  // on envoie la base et le shift pour la conversion narrowoop en oop

    	getandsend_roots();
    	int* ack = (int*) malloc(sizeof(int));
    	read(sockfd, ack, sizeof(int));
    	lock.unlock();
}
