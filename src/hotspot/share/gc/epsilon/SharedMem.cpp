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


batch_t* SharedMem::get_new_batch(int index, int thread_offset, PseudoTLAB* tlab){
	uint64_t offset = 0;
	int base_index = index*LINEAR_ENTRIES_WIDTH;
	int count = thread_offset;
	//entry_tab[word_size].state = USED;
	//tlab->nb_get_batch++;
	entry_tab[base_index].count.store(1);
	//entry_tab[base_index+1].count.store(1);
	//entry_tab[base_index+2].count.store(1);
	//entry_tab[base_index+3].count.store(1);
	//entry_tab[base_index+4].count.store(1);
	do{
		//faire un test en lecture avant d'écrire
	//	tlab->nb_loops++;
		//if(entry_tab[base_index + count].batch.load() != 0)
		//	offset = entry_tab[base_index + count].batch.exchange(0);
		if(entry_tab[base_index].batch.load() != 0)
			offset = entry_tab[base_index].batch.exchange(0);
		count = (count + 1) %LINEAR_ENTRIES_WIDTH;
		asm volatile("pause");
	}while(offset==0);
	//if(tlab->nb_get_batch % 1000 == 0)
	//	printf("size: %lu, getbatch: %d, loop: %d \n", word_size, tlab->nb_get_batch, tlab->nb_loops);
	return abs_addr(offset);
}

batch_t* SharedMem::get_new_batch_exp(int index, PseudoTLAB* tlab){
	uint64_t offset = 0;
	int base_index = index ; //+ 32; //32 = 40 -8
	//entry_tab[word_size].state = USED;
	//tlab->nb_get_batch++;
	entry_tab[base_index].count.store(1);
	do{
		//faire un test en lecture avant d'écrire
	//	tlab->nb_loops++;
		if(entry_tab[base_index].batch.load() != 0)
			offset = entry_tab[base_index].batch.exchange(0);
		asm volatile("pause");
	}while(offset==0);
	//if(tlab->nb_get_batch % 1000 == 0)
	//	printf("size: %lu, getbatch: %d, loop: %d \n", word_size, tlab->nb_get_batch, tlab->nb_loops);
	return abs_addr(offset);
}

void PseudoTLAB::initialize(SharedMem* shm_){
	shm = shm_;
	tid = shm->thread_counter.fetch_add(1);
	thread_offset = tid%LINEAR_ENTRIES_WIDTH;
	memset(batch_tab, 0, (NBR_OF_LINEAR_ENTRIES + NBR_OF_EXP_ENTRIES)*sizeof(batch_t*));
}

HeapWord* PseudoTLAB::allocate(size_t word_size){
	HeapWord* ret;
	size_t index = batch_index_from_size_slow(word_size);

	if(batch_tab[index] == NULL){ // if there is no batch
		if(index<=7)
			batch_tab[index] = shm->get_new_batch(index, thread_offset, this); //get new batch
		else
			batch_tab[index] = shm->get_new_batch_exp(index + 32, this); //get new batch
		batch_tab[index]->bump = 0;
	} else if(batch_tab[index]->bump >= (uint32_t) shm->size_of_buffer(word_size)){ // if batch is finished
		batch_t* temp = batch_tab[index];
		shm->stack_push(shm->prefree_list, (uint64_t) batch_tab[index] - (uint64_t)shm->start_addr); //push finished batch to prefree list

		if(index<=7)
			batch_tab[index] = shm->get_new_batch(index, thread_offset, this); //get new batch
		else
			batch_tab[index] = shm->get_new_batch_exp(index + 32, this); //get new batch
		batch_tab[index]->bump = 0;
	}

	ret = (HeapWord*) batch_tab[index]->array[batch_tab[index]->bump]; // allocate from inside the batch
	batch_tab[index]->bump++;

	return ret;

}

void PseudoTLAB::free(){
	batch_t* curr;
	for(unsigned int i=0; i<NBR_OF_LINEAR_ENTRIES + NBR_OF_EXP_ENTRIES; i++){
		curr = batch_tab[i];
		if(curr!=NULL){
			shm->stack_push(shm->prefree_list, (uint64_t)curr  - (uint64_t)shm->start_addr); //push finished batch to prefree list
		}
	}
}

int PseudoTLAB::index_from_size(int size){                                                                                                                                                   
    if(size <= NBR_OF_LINEAR_ENTRIES){
		//printf("tid: %d, index: %d\n",tid , (size-1)*LINEAR_ENTRIES_WIDTH + tid%LINEAR_ENTRIES_WIDTH);
        return (size-1)*LINEAR_ENTRIES_WIDTH ;
	}
    if(size <= 8192)
        return NBR_OF_LINEAR_ENTRIES*LINEAR_ENTRIES_WIDTH + __builtin_ctz(size) - 4;  
    return 0;
}
  
inline size_t PseudoTLAB::batch_index_from_size(size_t size){                                                                                                                                                   
	size_t idx_lin = size - 1;
	size_t idx_exp = (sizeof(size_t) << 3) - __builtin_clzll(idx_lin) + 4 ; //c'est pas +4 c'est -log2(16) + 8
  	size_t klin = size < 9 ? -1 : 0;
  	size_t kexp = ~klin;
  	return (idx_lin & klin) | (idx_exp & kexp);
}
 
size_t PseudoTLAB::batch_index_from_size_slow(size_t size){                                                                                                                                                   
    if(size <= NBR_OF_LINEAR_ENTRIES){
		//printf("tid: %d, index: %d\n",tid , (size-1)*LINEAR_ENTRIES_WIDTH + tid%LINEAR_ENTRIES_WIDTH);
        return (size-1);
	}
    if(size <= 8192)
        return NBR_OF_LINEAR_ENTRIES + __builtin_ctzll(size) - 4;  
    return 0;
}

int SharedMem::size_from_index(int index){                                                                                                                                                  
    if(index <= NBR_OF_LINEAR_ENTRIES*LINEAR_ENTRIES_WIDTH -1) 
        return index/LINEAR_ENTRIES_WIDTH + 1;
    return 16 << (index - NBR_OF_LINEAR_ENTRIES*LINEAR_ENTRIES_WIDTH);
} 
