//
// Created by adam on 5/3/22.
//

#ifndef JDK11U_ROOTS_HPP
#define JDK11U_ROOTS_HPP

#include "memory/iterator.hpp"
#include <mutex>
#include <stdio.h>


struct linked_list{
    HeapWord* value;
    struct linked_list *next;
};


class RemoteClosure : public OopClosure{
private:
    //mutable std::mutex lock;
public:
    struct linked_list* head = NULL;
    int list_length = 0;

public:
	// print les racines, et print d'où elles viennent avant
    void do_oop(oop* o){
		if(*o != nullptr){
			struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
        	//lock.lock();
        	list_length++;
        	struct linked_list* curr = head;
        	new_node->value = (HeapWord*) *o;
			//printf("\t%p\n",(HeapWord*) *o);
        	new_node->next = curr;
        	head = new_node;
		}
        //lock.unlock();

    };

    void do_oop(narrowOop* o){
		printf("narrow root: %d\n", *o);
		if(*o != 0){
			struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
        	//lock.lock();
        	list_length++;
        	struct linked_list* curr = head;
        	new_node->value = (HeapWord*) (uint64_t) *o;
			//printf("\t%p\n", (HeapWord*)(uint64_t)  *o);
        	new_node->next = curr;
        	head = new_node;
		}
        //lock.unlock();
    };

    int getSize(){
        return list_length;
    }

    uint64_t* getArray(){
        //lock.lock();
        uint64_t *array = (uint64_t *) calloc(1,list_length*sizeof(uint64_t));
        linked_list * curr = head;
        int i = 0;
        while(curr!=NULL && i<list_length){
            HeapWord* val = (HeapWord*) curr->value;
			array[i] = (uint64_t) val;
            i++;
            curr=curr->next;
        }
		list_length = i;
        return array;
        //lock.unlock();
    }

};



class RootMark {
public:
    enum RootType {
        universe              = 1,
        jni_handles           = 2,
        threads               = 3,
        object_synchronizer   = 4,
        management            = 5,
        jvmti                 = 6,
        system_dictionary     = 7,
        class_loader_data     = 8,
        code_cache            = 9
    };
public:
    RemoteClosure rc;
public:
    RootMark() {};

    void do_it();

    int getArraySize(){
        return rc.getSize();
    }
    uint64_t* rootArray(){
        return rc.getArray();
    }
};

#endif //JDK11U_ROOTS_HPP ")
