//
// Created by adam on 8/31/23.
//
#include "gc/epsilon/SharedMem.hpp"
#include "gc/epsilon/RemoteSpace.hpp"
#include <time.h>

void SharedMem::initialize( void* addr_, RemoteSpace* rs_){
	start_addr = addr_;
	printf("Shared memory addr: %p\n", start_addr);
	//memset(start_addr, 0, PRE_FREE_SIZE + ENTRIES_SIZE);
	prefree_list = (struct batch_stack*) start_addr;
	share_lock = (struct ticket_lock *) ((uint64_t)start_addr + PRE_FREE_SIZE);
	mutex = (sem_t*) ((uint64_t)start_addr + PRE_FREE_SIZE + SPIN_SIZE );
	entry_tab = (struct entry*) ((uint64_t)start_addr + PRE_FREE_SIZE + SPIN_SIZE + SEM_SIZE );
	start_of_batches = (batch_t*) ((uint64_t)entry_tab + ENTRIES_SIZE);
	
	thread_counter.store(0);
	
	tlab_list = nullptr;
	init_ticket_lock(&tlab_list_lock);

	rs = rs_;
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
    //struct timespec start, end;
	uint64_t offset = 0;
	//int base_index = index*LINEAR_ENTRIES_WIDTH;
	int base_index = index << 2;
	int count = thread_offset;
	//entry_tab[word_size].state = USED;
	//tlab->nb_get_batch++;
	//entry_tab[base_index].count.store(1);
	//entry_tab[base_index+1].count.store(1);
	//entry_tab[base_index+2].count.store(1);
	//entry_tab[base_index+3].count.store(1);
	//entry_tab[base_index+4].count.store(1);
    //clock_gettime(CLOCK_MONOTONIC, &start);
	do{
		//faire un test en lecture avant d'écrire
	//	tlab->nb_loops++;
		if(entry_tab[base_index + count].batch.load() != 0)
			offset = entry_tab[base_index + count].batch.exchange(0);
		//if(entry_tab[base_index].batch.load() != 0)
		//	offset = entry_tab[base_index].batch.exchange(0);
		count = (count + 1) & 3;// cette ligne semble diviser par 3 les perfs
		asm volatile("pause");
	}while(offset==0);
    //clock_gettime(CLOCK_MONOTONIC, &end);
	//tlab->cumulative_time += (end.tv_sec - start.tv_sec)* 1e9 + end.tv_nsec - start.tv_nsec;
	//if(tlab->nb_get_batch % 1000 == 0)
	//	printf("size: %lu, getbatch: %d, loop: %d \n", word_size, tlab->nb_get_batch, tlab->nb_loops);
	return abs_addr(offset);
}

batch_t* SharedMem::get_new_batch_exp(int index, PseudoTLAB* tlab){
    //struct timespec start, end;
	uint64_t offset = 0;
	int base_index = index ; //+ 32; //32 = 40 -8
	//entry_tab[word_size].state = USED;
	//tlab->nb_get_batch++;
	entry_tab[base_index].count.store(1);
    //clock_gettime(CLOCK_MONOTONIC, &start);
	do{
		//faire un test en lecture avant d'écrire
	//	tlab->nb_loops++;
		if(entry_tab[base_index].batch.load() != 0)
			offset = entry_tab[base_index].batch.exchange(0);
		asm volatile("pause");
	}while(offset==0);
    //clock_gettime(CLOCK_MONOTONIC, &end);
	//tlab->cumulative_time += (end.tv_sec - start.tv_sec)*1e9 + end.tv_nsec - start.tv_nsec;
	//if(tlab->nb_get_batch % 1000 == 0)
	//	printf("size: %lu, getbatch: %d, loop: %d \n", word_size, tlab->nb_get_batch, tlab->nb_loops);
	return abs_addr(offset);
}

int SharedMem::nbr_threads(){
	int i = 0;
	PseudoTLAB* curr = tlab_list;
	spin_lock(&tlab_list_lock);
	while(curr!=nullptr){
		i++;
		curr = curr->next;
	}
	spin_unlock(&tlab_list_lock);
	return i;
}
uint64_t SharedMem::get_while_time(){
	uint64_t time_total=0;
	spin_lock(&tlab_list_lock);
	PseudoTLAB* curr = tlab_list;
	while(curr!=nullptr){
		time_total += curr->cumulative_time;
		curr = curr->next;
	}
	spin_unlock(&tlab_list_lock);

	return time_total;
}

void SharedMem::reset_used(){
	spin_lock(&tlab_list_lock);
	PseudoTLAB* curr = tlab_list;
	while(curr!=nullptr){
		//curr->thread->used_local = 0;
		curr = curr->next;
	}
	spin_unlock(&tlab_list_lock);
}

void SharedMem::add_tlab(PseudoTLAB* t){
	spin_lock(&tlab_list_lock);
	t->next = tlab_list;
	tlab_list = t;
	spin_unlock(&tlab_list_lock);
}

void SharedMem::remove_tlab(PseudoTLAB* t){
	PseudoTLAB* prev = nullptr;
	spin_lock(&tlab_list_lock);
	PseudoTLAB* curr = tlab_list;
	if(curr == t){
		tlab_list = curr->next;
		spin_unlock(&tlab_list_lock);
		return;
	}

	while(curr != nullptr){
		if(curr == t ){
			prev->next = curr->next;
		}

		prev = curr;
		curr = curr->next;
	}

	spin_unlock(&tlab_list_lock);
}


void PseudoTLAB::init_fake_batch_entries(batch_t** b){
	b[0] = (batch_t*) malloc(sizeof(batch_t));
	b[0]->bump = 1;
	b[0]->end = 0;
	b[0]->thread_no = 0xffffffff;
	for(int i = 1; i< NBR_OF_LINEAR_ENTRIES + OVERFLOW_EXP; i++){
		b[i] = (batch_t*) malloc(sizeof(batch_t));
		memcpy(b[i], b[0],16);
	}
}

void PseudoTLAB::initialize(SharedMem* shm_, Thread* thread_){
	//SharedMem::shm = SharedMem::shm_;
	//tid = SharedMem::shm->thread_counter.fetch_add(1);
	thread = thread_;
	thread_offset = tid%LINEAR_ENTRIES_WIDTH;
	//thread->used_local=0;
	//memset(batch_tab, 0, (NBR_OF_LINEAR_ENTRIES + NBR_OF_EXP_ENTRIES)*sizeof(batch_t*));
    init_fake_batch_entries(batch_tab);	
	cumulative_time=0;
	//SharedMem::shm->add_tlab(this);
	
}

HeapWord* PseudoTLAB::allocate(size_t word_size){
	HeapWord* ret;
	size_t index = batch_index_from_size(word_size);
	//printf("%p;%lu : bump : %u, end: %u\n", this, word_size, batch_tab[index]->bump, batch_tab[index]->end);	
	//printf("%u,  %lu\n", batch_tab[index]->thread_no, (uint64_t) batch_tab[index]-(uint64_t)start_addr);
	if(batch_tab[index]->end == 0 || batch_tab[index]->thread_no == 69){ // if there is no batch
	//printf("%u,  %lu, %lu\n", batch_tab[index]->thread_no, (uint64_t) batch_tab[index]-(uint64_t)start_addr, index);
		std::free(batch_tab[index]);
		if(index<=7)
			batch_tab[index] = SharedMem::shm->get_new_batch(index, thread_offset, this); //get new batch
		else
			batch_tab[index] = SharedMem::shm->get_new_batch_exp(index + START_OF_EXP, this); //get new batch
		batch_tab[index]->bump = 0;
	} else if(batch_tab[index]->bump >= batch_tab[index]->end){ // if batch is finished
		uint32_t old_end = batch_tab[index]->end;
		batch_t* temp = batch_tab[index];
		SharedMem::shm->stack_push(SharedMem::shm->prefree_list, (uint64_t) batch_tab[index] - (uint64_t)SharedMem::shm->start_addr); //push finished batch to prefree list

		if(index<=7)
			batch_tab[index] = SharedMem::shm->get_new_batch(index, thread_offset, this); //get new batch
		else
			batch_tab[index] = SharedMem::shm->get_new_batch_exp(index + START_OF_EXP, this); //get new batch
		batch_tab[index]->bump = 0;
		SharedMem::shm->rs->slow_path_post_alloc(old_end*word_size);
		//thread->used_local = 0;
	}
	
	ret = (HeapWord*) batch_tab[index]->array[batch_tab[index]->bump]; // allocate from inside the batch
	batch_tab[index]->bump++;
	
	//thread->used_local += word_size;
	return ret;

}

void PseudoTLAB::free(){
	batch_t* curr;
	for(unsigned int i=0; i<NBR_OF_LINEAR_ENTRIES + NBR_OF_EXP_ENTRIES; i++){
		curr = batch_tab[i];
		if(curr!=NULL && curr->thread_no != 0xffffffff){
			SharedMem::shm->stack_push(SharedMem::shm->prefree_list, (uint64_t)curr  - (uint64_t)SharedMem::shm->start_addr); //push finished batch to prefree list
		}
	}
	//SharedMem::shm->remove_tlab(this);
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
