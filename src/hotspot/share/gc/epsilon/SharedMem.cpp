//
// Created by adam on 8/31/23.
//
#include "gc/epsilon/SharedMem.hpp"

void SharedMem::initialize( void* addr_){
	start_addr = addr_;
	printf("Shared memory addr: %p\n", start_addr);
	//memset(start_addr, 0, PRE_FREE_SIZE + ENTRIES_SIZE);
	prefree_list = (struct batch_stack*) start_addr;
	mutex = (sem_t*) ((uint64_t)start_addr + PRE_FREE_SIZE);
	entry_tab = (struct entry*) ((uint64_t)start_addr + PRE_FREE_SIZE + SEM_SIZE);
	start_of_batches = (batch_t*) ((uint64_t)entry_tab + ENTRIES_SIZE);


}

void SharedMem::stack_push(struct batch_stack* list, uint64_t batch){
	sem_wait(mutex);
	uint64_t orig;
	//do{ 
		orig = list->head.load(std::memory_order_relaxed);
		abs_addr(batch)->next1 = (uint32_t) orig;
		list->head.store(batch, std::memory_order_relaxed);
	//}while(!list->head.compare_exchange_strong(orig, batch));
	sem_post(mutex);

}

uint64_t SharedMem::stack_pop(batch_stack *list){
	sem_wait(mutex);
	uint64_t orig;
	uint64_t next;
	//do{
		orig = list->head.load(std::memory_order_relaxed);
		if(orig == 0){
			sem_post(mutex);
			return 0;
		}
		next = (uint64_t) abs_addr(orig)->next1;
		list->head.store(next, std::memory_order_relaxed);
	//}while(!list->head.compare_exchange_strong(orig, next));

	sem_post(mutex);
	return orig;
}


batch_t* SharedMem::get_new_batch(size_t word_size){
	uint64_t offset;

	//entry_tab[word_size].state = USED;
	entry_tab[word_size].count++;
	do{
		offset = entry_tab[word_size].batch.exchange(0, std::memory_order_relaxed);
		asm volatile("pause");
	}while(offset==0);
	//printf("New batch : %lu\n", offset);	
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
	else if(batch_tab[word_size]->bump >= BUFFER_SIZE){ // if batch is finished
		//printf("What was pushed: %lu, for size: %ld\n", (uint64_t) batch_tab[word_size] - (uint64_t)shm->start_addr, word_size);
		batch_t* temp = batch_tab[word_size];

		if(batch_tab[word_size]->next_batch =! 0 && batch_tab[word_size]->count != 0 ){
			batch_t* temp = shm->abs_addr(batch_tab[word_size]->next_batch);
			shm->stack_push(shm->prefree_list, (uint64_t) batch_tab[word_size] - (uint64_t)shm->start_addr); //push finished batch to prefree list
			batch_tab[word_size] = temp;
			//printf("Coucou: batch: %p, size: %lu\n", batch_tab[word_size], word_size );
		} else {
			shm->stack_push(shm->prefree_list, (uint64_t) batch_tab[word_size] - (uint64_t)shm->start_addr); //push finished batch to prefree list
			batch_tab[word_size] = shm->get_new_batch(word_size); //get new batch
		}
	}

	ret = (HeapWord*) batch_tab[word_size]->array[batch_tab[word_size]->bump]; // allocate from inside the batch
	//printf("Allocated: %p,  Size: %lu,  Count: %u, bump: %u\n", ret, word_size, batch_tab[word_size]->count, batch_tab[word_size]->bump++);
	batch_tab[word_size]->bump++;

	return ret;

}
