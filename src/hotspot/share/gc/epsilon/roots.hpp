//
// Created by adam on 5/3/22.
//

#ifndef JDK11U_ROOTS_HPP
#define JDK11U_ROOTS_HPP

#include "memory/iterator.hpp"

struct linked_list{
    oop value;
    struct linked_list *next;
};

struct narrow_linked_list{
    narrowOop value;
    struct narrow_linked_list *next;
};

class RemoteClosure : public OopClosure{
public:
    struct linked_list* head = NULL;
    struct narrow_linked_list* narrow_head = NULL;
public:
    void do_oop(oop* o){
        struct linked_list* curr = head;
        struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
        new_node->value = *o;
        new_node->next = curr;
        head = new_node;
    };

    void do_oop(narrowOop* o){
        struct narrow_linked_list* curr = narrow_head;
        struct narrow_linked_list* new_node = (struct narrow_linked_list*) malloc(sizeof(struct narrow_linked_list));
        new_node->value = *o;
        new_node->next = curr;
        narrow_head = new_node;
    };

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
    RootType _root_type;
    RemoteClosure rc;
public:
    RootMark(RootType value) : _root_type(value) {}
    void do_it();
};

#endif //JDK11U_ROOTS_HPP
