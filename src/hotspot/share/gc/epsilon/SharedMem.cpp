//
// Created by adam on 8/31/23.
//
#include "gc/epsilon/SharedMem.hpp"

void SharedMem::initialize( void* addr_){
	start_addr = addr_;
	printf("Shared memory addr: %p\n", start_addr);
	//memset(start_addr, 0, PRE_FREE_SIZE + ENTRIES_SIZE);
	prefree_list = (struct batch_stack*) start_addr;
	share_lock = (struct ticket_lock *) ((uint64_t)start_addr + PRE_FREE_SIZE);
	mutex = (sem_t*) ((uint64_t)start_addr + PRE_FREE_SIZE + SPIN_SIZE );
	entry_tab = (struct entry*) ((uint64_t)start_addr + PRE_FREE_SIZE + SPIN_SIZE + SEM_SIZE );
	start_of_batches = (batch_t*) ((uint64_t)entry_tab + ENTRIES_SIZE);
	
	thread_counter.store(0);

}

void SharedMem::stack_push(struct batch_stack* list, uint64_t batch){
	spin_lock(share_lock);
	uint64_t orig;
	//do{ 
		orig = list->head.load();
		abs_addr(batch)->next1 =  orig;
		list->head.store(batch);
	//}while(!list->head.compare_exchange_strong(orig, batch));
	spin_unlock(share_lock);

}

uint64_t SharedMem::stack_pop(batch_stack *list){
	spin_lock(share_lock);
	uint64_t orig;
	uint64_t next;
	//do{
		orig = list->head.load();
		if(orig == 0){
			spin_unlock(share_lock);
			return 0;
		}
		next = abs_addr(orig)->next1;
		list->head.store(next);
	//}while(!list->head.compare_exchange_strong(orig, next));

	spin_unlock(share_lock);
	return orig;
}


batch_t* SharedMem::get_new_batch(size_t word_size, PseudoTLAB* tlab){
	uint64_t offset = 0;

	//entry_tab[word_size].state = USED;
	tlab->nb_get_batch++;
	entry_tab[word_size].count.store(1);
	do{
		//faire un test en lecture avant d'écrire
		tlab->nb_loops++;
		if(entry_tab[word_size].batch.load() != 0)
			offset = entry_tab[word_size].batch.exchange(0);
		asm volatile("pause");
	}while(offset==0);
	if(tlab->nb_get_batch % 1000 == 0)
		printf("size: %lu, getbatch: %d, loop: %d \n", word_size, tlab->nb_get_batch, tlab->nb_loops);
	return abs_addr(offset);
}

void PseudoTLAB::initialize(SharedMem* shm_){
	shm = shm_;
	tid = shm->thread_counter.fetch_add(1);
	memset(batch_tab, 0, NBR_OF_ENTRIES*sizeof(batch_t*));
}

HeapWord* PseudoTLAB::allocate(size_t word_size){
	int index = index_from_size(word_size);
	//printf("size: %lu, index_from_size: %d\n", word_size, index);
	HeapWord* ret;
	if(batch_tab[index] == NULL){ // if there is no batch
		batch_tab[index] = shm->get_new_batch(index, this); //get new batch
		batch_tab[index]->bump = 0;
	}
	else if(batch_tab[index]->bump >= (uint32_t) shm->size_of_buffer(word_size)){ // if batch is finished
	//else if(batch_tab[word_size]->bump >= BUFFER_SIZE){ // if batch is finished
		//printf("What was pushed: %lu, for size: %ld\n", (uint64_t) batch_tab[word_size] - (uint64_t)shm->start_addr, word_size);
		batch_t* temp = batch_tab[index];

		//if(batch_tab[word_size]->next_batch =! 0 && batch_tab[word_size]->count != 0 ){
		//	batch_t* temp = shm->abs_addr(batch_tab[word_size]->next_batch);
		//	shm->stack_push(shm->prefree_list, (uint64_t) batch_tab[word_size] - (uint64_t)shm->start_addr); //push finished batch to prefree list
		//	batch_tab[word_size] = temp;
		//	//printf("Coucou: batch: %p, size: %lu\n", batch_tab[word_size], word_size );
		//} else {
			shm->stack_push(shm->prefree_list, (uint64_t) batch_tab[index] - (uint64_t)shm->start_addr); //push finished batch to prefree list
			batch_tab[index] = shm->get_new_batch(index, this); //get new batchù
			batch_tab[index]->bump = 0;
		//}
	}
	ret = (HeapWord*) batch_tab[index]->array[batch_tab[index]->bump]; // allocate from inside the batch
	//printf("ret: %p, size: %lu, bump: %u\n", ret, word_size, batch_tab[word_size]->bump);
	//printf("Allocated: %p,  Size: %lu,  thread: %p, bump: %u, batch: %p\n", ret, word_size, thread, batch_tab[word_size]->bump, batch_tab[word_size]);
	batch_tab[index]->bump++;

	return ret;

}

void PseudoTLAB::free(){
	batch_t* curr;
	for(unsigned int i=0; i<NBR_OF_ENTRIES; i++){
		curr = batch_tab[i];
		if(curr!=NULL){
			shm->stack_push(shm->prefree_list, (uint64_t)curr  - (uint64_t)shm->start_addr); //push finished batch to prefree list
		}
	}
}

int PseudoTLAB::index_from_size(int size){                                                                                                                                                   
    if(size <= NBR_OF_LINEAR_ENTRIES)                                                                                                                                                       
        return (size-1)*LINEAR_ENTRIES_WIDTH + tid%LINEAR_ENTRIES_WIDTH;                                                                                                                                               
    if(size <= 8192)                                                                                                                                                                        
        return NBR_OF_LINEAR_ENTRIES*LINEAR_ENTRIES_WIDTH + __builtin_ctz(size) - 4;                                                                                                        
    return 0;                                                                                                                                                                               
}                                                                                                                                                                                           
                                                                                                                                                                                            
                                                                                                                                                                                            
int SharedMem::size_from_index(int index){                                                                                                                                                  
    if(index <= NBR_OF_LINEAR_ENTRIES*LINEAR_ENTRIES_WIDTH -1)                                                                                                                              
        return index/LINEAR_ENTRIES_WIDTH + 1;                                                                                                                                              
    return 16 << (index - NBR_OF_LINEAR_ENTRIES*LINEAR_ENTRIES_WIDTH);                                                                                                                      
                                                                                                                                                                                            
}                                                                                                                                                                                           
