//
// Created by adam on 8/31/23.
//
#include "gc/epsilon/SharedMem.hpp"

void SharedMem::initialize( void* addr_){
	start_addr = addr_;
	memset(start_addr, 0, PRE_FREE_SIZE + ENTRIES_SIZE);
	prefree_list = (struct batch_stack*) start_addr;
	entry_tab = (struct entry*) start_addr + PRE_FREE_SIZE;
	start_of_batches = (batch_t*) entry_tab + ENTRIES_SIZE;
}

void SharedMem::queue_push(batch_queue *list, batch_t* batch){
	if(list->tail == NULL){
		list->head = list->tail = batch;
		return;
	}
	list->tail->next = batch;
	list->tail = batch;
}

batch_t* SharedMem::queue_pop(batch_queue *list){
	if(list->head == NULL)
		return NULL;
	batch_t* temp = list->head;
	list->head = list->head->next;

	if(list->head == NULL)
		list->tail = NULL;
	
	return temp;
}

void SharedMem::stack_push(struct batch_stack* list, batch_t* batch){
	batch_t* orig = list->head.load();
	do{
		batch->next = orig;
	}while(!list->head.compare_exchange_strong(orig, batch));

}

batch_t* SharedMem::stack_pop(batch_stack *list){
	batch_t* orig = list->head.load();
	batch_t* next;
	do{
		if(orig == NULL)
			return NULL;
		next = orig->next;
	}while(!list->head.compare_exchange_strong(orig, next));

	return orig;
}


batch_t* SharedMem::get_new_batch(size_t word_size){
	batch_t* batch = NULL;

	entry_tab[word_size].state = USED;
	do{
		batch = entry_tab[word_size].batch.exchange(NULL);
		asm volatile("pause");
	}while(batch == NULL);
	
	return batch;
}

void PseudoTLAB::initialize(SharedMemory* shm_){
	shm = shm_;
	memset(batch_tab, 0, NBR_OF_ENTRIES*sizeof(batch_t*));
}

HeapWord* PseudoTLAB::allocate(size_t word_size){
	HeapWord* ret;
	if(batch_tab[word_size] == NULL){
		batch_tab[word_size] = shm->get_new_batch()
	}
	ret = batch_tab[word_size]->array[batch_tab[word_size]->bump];
	batch_tab[word_size]->bump++;

	return ret;

}
