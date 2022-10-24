//
// Created by adam on 10/20/22.
//
#include <arpa/inet.h>
#include <stdlib.h>
#include "remoteLoader.hpp"


RemoteLoader::RemoteLoader() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons( RLOADER_PORT );

    if (connect(sockfd, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
    }

	printf("Loader fd: %d\n", sockfd);
}

RemoteLoader::~RemoteLoader() {
    //close(sockfd);
}

void* RemoteLoader::operator new(size_t size) throw(){
    return malloc(size);
};

int RemoteLoader::write(void *array, size_t size) {
    return send(sockfd, array, size, 0);
}

int RemoteLoader::read(void *array, size_t size) {
    return recv(sockfd,array, size, 0);
}
