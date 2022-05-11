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

int sockfd;
std::mutex lock;

RemoteSpace::RemoteSpace() : ContiguousSpace() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
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
    size_t  mr_word_size = mr.word_size();
    char msg_tag = 'i';
    msg->mr_start =  mr_start;
    msg->mr_word_size =  mr_word_size;
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
    std::free(result);
    return allocated;
}


HeapWord *RemoteSpace::par_allocate_klass(size_t word_size, Klass* klass) {

    /*----------------/
    /* Ce bout de code sert à trouver et  afficher les racines du tas pour le gc*/
    /*à terme, la fonction getandsend_roots est appelée par le RemoteSpace via des sockets ou un signal*/
    counter++;
    if(counter >= 2000 && roots){
        getandsend_roots(0);
        roots = false;
    }
    /*-------------*/
    //printf("klass: %s\n", klass->external_name());


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
        klass_data->table_length = klass->vtable_length();
        write(sockfd, klass_data, sizeof(struct msg_klass_data));
        write(sockfd,klass->external_name(), klass_data->name_length);
        write(sockfd, klass->vtable().table(), klass_data->table_length);
        std::free(klass_data);
    }
    lock.unlock();
    std::free(msg);
    HeapWord* allocated = result->ptr;
    std::free(result);
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




void getandsend_roots(int sig) {
    char msg_tag = 'r';

    RootMark rm(RootMark::threads);
    rm.do_it();
    int array_length = rm.getArraySize();
    unsigned long *root_array= rm.rootArray();
    lock.lock();
    write(sockfd, &msg_tag,1);
    write(sockfd, &array_length, sizeof(int));
    write(sockfd, root_array, sizeof(unsigned long)*array_length );
    lock.unlock();
}


void RemoteSpace::safe_object_iterate(ObjectClosure* blk){
    return;
};

void RemoteSpace::print_on(outputStream* st) const{
    return;
}