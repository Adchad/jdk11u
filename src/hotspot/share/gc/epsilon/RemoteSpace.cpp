//
// Created by adam on 4/11/22.
//

#include "RemoteSpace.hpp"
#include "rpcMessages.hpp"
#include <arpa/inet.h>
#include <stdlib.h>

RemoteSpace::RemoteSpace() {
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
    struct msg_initialize * msg = (struct msg_initialize*)  malloc(sizeof(struct msg_initialize));
    HeapWord* mr_start = mr.start();
    size_t  mr_word_size = mr.word_size();

    msg->msg = 'i';
    msg->mr_start = mr_start->value();
    msg->mr_word_size = mr_word_size;
    msg->clear_space = clear_space;
    msg->mangle_space = mangle_space;

    write(sockfd, msg, sizeof(msg));
    std::free(msg);
}

HeapWord *RemoteSpace::par_allocate(size_t word_size) {
    struct msg_par_allocate * msg = (struct msg_par_allocate*) malloc(sizeof(struct msg_par_allocate));
    msg->msg = 'a';
    msg->word_size = word_size;
    write(sockfd, msg, sizeof(msg));

    char **result = (char**) malloc(sizeof(char*));

    read(sockfd, result, sizeof(char*));
    std::free(msg);


    HeapWord * allocated = (HeapWord*) malloc(sizeof(HeapWord));
    allocated->setI(*result);
    return allocated;
}

void RemoteSpace::set_end(HeapWord* value){
    struct msg_set_end * msg = (struct msg_set_end*) malloc(sizeof(struct msg_set_end));
    msg->msg = 'e';
    msg->value = value->value();
    write(sockfd, msg, sizeof(msg));

    std::free(msg);
}

size_t RemoteSpace::used() const{
    struct msg_used msg = {.msg = 'u'};
    write(sockfd, &msg, 1);

    size_t *result = (size_t*) malloc(sizeof(size_t));
    read(sockfd, result, sizeof(size_t));

    return *result;
}

void RemoteSpace::safe_object_iterate(ObjectClosure* blk){
    return;
};

void RemoteSpace::print_on(outputStream* st) const{
    return;
}