//
// Created by adam on 4/11/22.
//

#ifndef JDK11U_REMOTESPACE_HPP
#define JDK11U_REMOTESPACE_HPP

#include "gc/shared/space.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <mutex>
#include "rpcMessages.hpp"
#include "utilities/globalDefinitions.hpp"
#include "gc/epsilon/gc_helper.hpp"


struct range_t {
	char* base;
	size_t size;
};

struct hw_list{
	oop hw;
	struct hw_list* next;
};

void getandsend_roots(int sig);
void getandsend_roots();


extern std::mutex lock_remote;
extern int sockfd_remote;

class RemoteSpace : public ContiguousSpace{


public:
	static struct hw_list* poule;
	
	static void add_poule(oop hw){
		struct hw_list* new_poule = (struct hw_list*) malloc(sizeof(struct hw_list));
		new_poule->hw = hw;
		new_poule->next = poule;
		poule = new_poule;
	}

private:
    int counter;
    bool roots = true;
    struct sockaddr_in server;
    int fd_for_heap;
	bool collected = false;
	struct range_t* heap_range;
	bool rp_init = false;
#if GCHELPER
	GCHelper gchelper;
	SpanSubjectToDiscoveryClosure _span_based_discoverer;
	ReferenceProcessor* rp;
	MemRegion _mr;
#endif


public:
    RemoteSpace();
    ~RemoteSpace();

    void initialize(MemRegion mr, bool clear_space, bool mangle_space);
    void post_initialize();
    HeapWord* par_allocate(size_t word_size);
    HeapWord* par_allocate_klass(size_t word_size, Klass* klass);
    void set_end(HeapWord* value);
    size_t used() const;
    size_t capacity() const;
    void safe_object_iterate(ObjectClosure* blk);
    void print_on(outputStream* st) const;

    void collect();

    void set_fd(int fd){ fd_for_heap = fd;}

	void deadbeef_area(uint64_t addr, int size, int type);

    void set_range(char* base, size_t size){
		heap_range = (struct range_t*) malloc(sizeof(struct range_t));
		heap_range->base = base; 
		heap_range->size = size; 
	};

#if GCHELPER
	void getandsend_helper();
#endif

};


#endif //JDK11U_REMOTESPACE_HPP
