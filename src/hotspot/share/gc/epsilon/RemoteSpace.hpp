//
// Created by adam on 4/11/22.
//

#ifndef JDK11U_REMOTESPACE_HPP
#define JDK11U_REMOTESPACE_HPP

#include "gc/shared/space.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <mutex>


class RemoteSpace : public ContiguousSpace{
private:
    int sockfd;
    struct sockaddr_in server;
    mutable std::mutex lock;

public:
    RemoteSpace();
    ~RemoteSpace();

    void initialize(MemRegion mr, bool clear_space, bool mangle_space);
    HeapWord* par_allocate(size_t word_size);
    HeapWord* par_allocate_klass(size_t word_size, Klass* klass);
    void set_end(HeapWord* value);
    size_t used() const;
    void safe_object_iterate(ObjectClosure* blk);
    void print_on(outputStream* st) const;

};

#endif //JDK11U_REMOTESPACE_HPP
