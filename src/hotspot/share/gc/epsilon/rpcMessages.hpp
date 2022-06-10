//
// Created by adam on 4/11/22.
//

#ifndef JDK11U_RPCMESSAGES_HPP
#define JDK11U_RPCMESSAGES_HPP

#include <stdlib.h>

#define HEADER_OFFSET 16
#define SIZE_OFFSET 12
#define KLASS_OFFSET 8

enum klass_type {instance = 1, objarray = 2, typearray = 3};

struct msg_initialize{
    HeapWord* mr_start;
    size_t mr_word_size;
    uint32_t obj_array_base;
	uint32_t obj_array_length;
    bool clear_space;
    bool mangle_space;
};

struct msg_par_allocate{
    size_t word_size;
};

struct msg_par_allocate_klass{
    size_t word_size;
    unsigned long klass;
};

struct msg_set_end{
    HeapWord* value;
};

struct msg_alloc_response{
    HeapWord* ptr;
    bool send_metadata = false;
};

struct msg_klass_data{
    //TODO: Nettoyer ce struct de merde
    klass_type klasstype;
    int name_length;
	int length;
    unsigned long base_klass = 0; //only for obj_array
    BasicType basetype = T_ILLEGAL; //only for typearray
    int layout_helper;
};


struct msg_collect{
	uint64_t base;
	int32_t shift;
};


#endif //JDK11U_RPCMESSAGES_HPP
