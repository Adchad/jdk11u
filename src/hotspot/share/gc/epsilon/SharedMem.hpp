//
// Created by adam on 8/31/23.
//

#ifndef SPACESERVER_SHARED_MEM_HPP
#define SPACESERVER_SHARED_MEM_HPP

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <semaphore.h>
#include "gc/epsilon/rpcMessages.hpp"

enum entry_state {
	UNUSED = 0,
	USED = 1
};


typedef struct batch{
	uint64_t array[BUFFER_SIZE];
	uint64_t next1;
	uint64_t next2;
	uint32_t bump;
	uint32_t size;
} batch_t;

struct entry{
	//enum entry_state state;
	std::atomic<uint64_t> batch;
	std::atomic<uint64_t> count;

	char padding[64 - 16];
};

struct batch_queue{
	batch_t* head;
	batch_t* tail;
};

struct batch_stack{
	std::atomic<uint64_t> head;
	char padding[64 - sizeof(head)];
};

#define PRE_FREE_SIZE sizeof(struct batch_stack)
#define SEM_SIZE sizeof(sem_t)
#define ENTRIES_SIZE (sizeof(struct entry)*BUFFER_MAX_SIZE)
#define NBR_OF_ENTRIES BUFFER_MAX_SIZE
//#define BATCH_SPACE_SIZE (SHM_SIZE - ENTRIES_SIZE) 
#define BATCH_SPACE_SIZE (5ULL*1024*1024*1024UL)
#define NBR_OF_BATCHES (BATCH_SPACE_SIZE/sizeof(batch_t))


class SharedMem {
public:
	void* start_addr;
	struct batch_stack* prefree_list;
	struct entry* entry_tab;
	batch_t* start_of_batches;
	sem_t* mutex;

//	struct batch_queue* free_list;
//	struct batch_queue* in_use_list;
	
public:
	void initialize(void* addr_);
	
	batch_t* abs_addr(uint64_t offset){
		return (batch_t*)(offset + (uint64_t)start_addr);
	}

	void stack_push(batch_stack* list, uint64_t batch);
	uint64_t stack_pop(batch_stack*);

	batch_t* get_new_batch(size_t word_size); 

	int size_of_buffer(int size){
		if(size <= 128)
			return 125;
		if(size <= 256)
			return 64;
		if(size <= 512)
			return 32;
		if(size <= 1024)
			return 16;
		if(size <= 2048)
			return 8;
		if(size <= 4096)
			return 4;

		return 1;
	}

};


class PseudoTLAB {
private:
	SharedMem* shm;
	void* thread;
	batch_t* batch_tab[NBR_OF_ENTRIES];
public:
	void initialize(SharedMem* shm_, void* thread_);
	HeapWord* allocate(size_t word_size);
	//HeapWord* allocate(size_t word_size);
	void free();

};


#endif //SPACESERVER_SHARED_MEM_HPP
