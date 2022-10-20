//
// Created by adam on 10/20/22.
//

#ifndef JDK11U_REMOTELOADER_HPP
#define JDK11U_REMOTELOADER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <mutex>
#include "utilities/globalDefinitions.hpp"

#define RLOADER_PORT 42070

class RemoteLoader {
private:
    struct sockaddr_in server;
    int sockfd;
    std::mutex lock;
public:
    RemoteLoader();
    ~RemoteLoader();

    void* operator new(size_t size) throw();

    int write(void* array, size_t size);
    int read(void* array, size_t size);
};


#endif //JDK11U_REMOTELOADER_HPP
