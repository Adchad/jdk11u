//
// Created by adam on 8/31/23.
//
#include "gc/epsilon/SharedMem.hpp"

void SharedMem::initialize( void* addr_){
	start_addr = addr_;
	printf("Shared memory addr: %p\n", start_addr);
	//memset(start_addr, 0, PRE_FREE_SIZE + ENTRIES_SIZE);
	prefree_list = (struct batch_stack*) start_addr;
	entry_tab = (struct entry*) ((uint64_t)start_addr + PRE_FREE_SIZE);
	start_of_batches = (batch_t*) ((uint64_t)entry_tab + ENTRIES_SIZE);
}

void SharedMem::stack_push(struct batch_stack* list, uint64_t batch){
	uint64_t orig = list->head.load();
	do{
		abs_addr(batch)->next = orig;
	}while(!list->head.compare_exchange_strong(orig, batch));

}

uint64_t SharedMem::stack_pop(batch_stack *list){
	uint64_t orig = list->head.load();
	uint64_t next;
	do{
		if(orig == 0)
			return 0;
		next = abs_addr(orig)->next;
	}while(!list->head.compare_exchange_strong(orig, next));

	return orig;
}


batch_t* SharedMem::get_new_batch(size_t word_size){
	uint64_t offset;

	entry_tab[word_size].state = USED;
	printf("Waiting for batch number: %ld\n", word_size);
	do{
		offset = entry_tab[word_size].batch.exchange(0);
		asm volatile("pause");
	}while(offset==0);
	printf("batchedd : %lu\n", offset);
	
	return abs_addr(offset);
}

void PseudoTLAB::initialize(SharedMem* shm_){
	shm = shm_;
	memset(batch_tab, 0, NBR_OF_ENTRIES*sizeof(batch_t*));
}

HeapWord* PseudoTLAB::allocate(size_t word_size){
	HeapWord* ret;
	if(batch_tab[word_size] == NULL){ // if there is no batch
		batch_tab[word_size] = shm->get_new_batch(word_size); //get new batch
	}
	if(batch_tab[word_size]->bump >= BUFFER_SIZE){ // if batch is finished
		shm->stack_push(shm->prefree_list, (uint64_t) batch_tab[word_size] - (uint64_t)shm->start_addr); //push finished batch to prefree list
		batch_tab[word_size] = NULL;
		batch_tab[word_size] = shm->get_new_batch(word_size); //get new batch
	}

	ret = (HeapWord*) batch_tab[word_size]->array[batch_tab[word_size]->bump]; // allocate from inside the batch
	batch_tab[word_size]->bump++;
	printf("TLAB Allocated: %p\n", ret);

	return ret;

}
