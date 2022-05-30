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

    void do_oop(oop* o){
        struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
        //lock.lock();
        list_length++;
        struct linked_list* curr = head;
        new_node->value = (HeapWord*) *o;
        new_node->next = curr;
        head = new_node;
        //lock.unlock();

    };

    void do_oop(narrowOop* o){
        struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
        //lock.lock();
        list_length++;
        struct linked_list* curr = head;
        new_node->value = (HeapWord*) (unsigned long) *o;
        new_node->next = curr;
        head = new_node;
        //lock.unlock();
    };

    int getSize(){
        return list_length;
    }

    unsigned long* getArray(){
        //lock.lock();
        unsigned long *array = (unsigned long *) malloc(list_length*sizeof(unsigned long));
        linked_list * curr = head;
        int i = 0;
        while(curr!=NULL){
            HeapWord* val = (HeapWord*) curr->value;
           // printf("Root: %lu\n", (unsigned long) val);
            *(array + i) = (unsigned long) val;
            i++;
            curr=curr->next;
        }
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
    RootMark();

    void do_it();

    int getArraySize(){
        return rc.getSize();
    }
    unsigned long* rootArray(){
        return rc.getArray();
    }
};

#endif //JDK11U_ROOTS_HPP
