//
// Created by adam on 4/11/22.
//

#include "RemoteSpace.hpp"
#include "rpcMessages.hpp"
#include <arpa/inet.h>
#include <stdlib.h>

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
    //printf("Start value: %p\n",  mr_start->value());
    //printf("WordSize: %p\n",  mr_word_size);
    //printf("Start: %ld\nWordSize: %ld\n", msg->mr_start, msg->mr_word_size);
    lock.lock();
    write(sockfd, &msg_tag, 1);
    write(sockfd, msg, sizeof(struct msg_initialize));
    lock.unlock();

    std::free(msg);

}

HeapWord *RemoteSpace::par_allocate(size_t word_size) {
    struct msg_par_allocate * msg = (struct msg_par_allocate*) malloc(sizeof(struct msg_par_allocate));
    char msg_tag = 'a';
    msg->word_size =  word_size;

    lock.lock();
    write(sockfd, &msg_tag, 1);
    write(sockfd, msg, sizeof(struct msg_par_allocate));
    lock.unlock();

    HeapWord** result = (HeapWord**) malloc(sizeof(HeapWord*));
    read(sockfd, result, sizeof(HeapWord*));
    std::free(msg);
    HeapWord * allocated =  *result;
    //printf("New allocated word: %p\n", (void*) allocated);
    //allocated->setI(*result);
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
    lock.unlock();

    size_t* result = (size_t*) malloc(sizeof(size_t));
    read(sockfd, result, sizeof(size_t));

    return *result;
}


void RemoteSpace::safe_object_iterate(ObjectClosure* blk){
    return;
};

void RemoteSpace::print_on(outputStream* st) const{
    return;
}