//
// Created by adam on 4/11/22.
//

#ifndef JDK11U_RPCMESSAGES_HPP
#define JDK11U_RPCMESSAGES_HPP

#include <stdlib.h>



struct msg_initialize{
    HeapWord* mr_start;
    size_t mr_word_size;
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
    int name_length;
    int fields_length;
};



#endif //JDK11U_RPCMESSAGES_HPP
