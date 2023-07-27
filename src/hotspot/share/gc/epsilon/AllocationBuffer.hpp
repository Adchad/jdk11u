#ifndef ALLOCATION_BUFFER_H
#define ALLOCATION_BUFFER_H

#include "gc/epsilon/rpcMessages.hpp"
#include "utilities/globalDefinitions.hpp"
#include <sys/time.h>

/*  Sizes are word_sizes in the JVM, meaning they are quantities of 8 bytes
 *	if size = 4, it means that it is 32 bytes
 *  */

typedef struct buffer_bucket_t{
	int bump_pointer;
	HeapWord* array[BUFFER_SIZE];
} buffer_bucket;

class AllocationBuffer{

private:
	buffer_bucket** table;
	int sockfd;

public:
	void initialize(int sockfd_){
		table = (buffer_bucket**) malloc(BUFFER_MAX_SIZE* sizeof(buffer_bucket*));
		memset(table, 0, BUFFER_MAX_SIZE* sizeof(buffer_bucket*));
		sockfd = sockfd_;
	}
	
	HeapWord* allocate(size_t size);
	void add_bucket(size_t size);
	HeapWord* add_bucket_and_allocate(size_t size);
	bool is_candidate(size_t size);
	void free_all(); // pour le faire après la collection, c'est atroce il faut pas faire ça
};


#endif
