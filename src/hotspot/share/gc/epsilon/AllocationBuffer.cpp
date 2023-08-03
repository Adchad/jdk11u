#include "gc/epsilon/AllocationBuffer.hpp"


HeapWord* AllocationBuffer::allocate(size_t size){
	if(size + HEADER_OFFSET/sizeof(HeapWord) >= BUFFER_MAX_SIZE){
		printf("Tried buffer allocation for a object that is too big\n");
		exit(0);
	}	
	buffer_bucket *curr = table[size];

	if(curr == nullptr){
		return nullptr;
	}
	if(curr->bump_pointer >= BUFFER_SIZE){
		free(curr);
		table[size] = nullptr;
		return nullptr;
	}

	HeapWord* result = curr->array[curr->bump_pointer];
	curr->bump_pointer++;
	if(result == nullptr){
		free(curr);
		table[size] = nullptr;
		return nullptr;
	}
	//printf("Result: %p,  bump_pointer: %d\n", result, curr->bump_pointer - 1);
	return result;
}

void AllocationBuffer::add_bucket(size_t size){
	if(size + HEADER_OFFSET/sizeof(HeapWord)>= BUFFER_MAX_SIZE || table[size] != nullptr){
		printf("Tried adding a buffer bucket with wrong parameters, size: %lu\n", size);
		exit(0);
	}

	buffer_bucket* new_bucket = (buffer_bucket*) malloc(sizeof(buffer_bucket));
	new_bucket->bump_pointer = 0;

	
	struct msg_request_buffer msg;
	msg.msg_type.type = 'b';
	msg.word_size = size;
	write(sockfd, &msg, sizeof(struct msg_request_buffer));
    read(sockfd, &new_bucket->array, BUFFER_SIZE*sizeof(HeapWord*));

	table[size] = new_bucket;
}

HeapWord* AllocationBuffer::add_bucket_and_allocate(size_t size){
	add_bucket(size);
	return allocate(size);
}

bool AllocationBuffer::is_candidate(size_t size){
	return (size > 0 && (size + HEADER_OFFSET/sizeof(HeapWord)) < BUFFER_MAX_SIZE);
}

void AllocationBuffer::free_all(){
	for(int i=0; i<BUFFER_MAX_SIZE; i++){
		if(table[i]!=nullptr){
			free(table[i]);
		}
	}
	memset(table, 0, BUFFER_MAX_SIZE* sizeof(buffer_bucket*));
}
