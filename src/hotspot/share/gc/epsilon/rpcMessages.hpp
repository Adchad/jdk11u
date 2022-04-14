//
// Created by adam on 4/11/22.
//

#ifndef JDK11U_RPCMESSAGES_HPP
#define JDK11U_RPCMESSAGES_HPP

#include <stdlib.h>



struct msg_initialize{
    char msg ;
    char* mr_start;
    size_t mr_word_size;
    bool clear_space;
    bool mangle_space;
};

struct msg_par_allocate{
    char msg;
    size_t word_size;
};

struct msg_set_end{
    char msg;
    char* value;
};

struct msg_used{
    char msg;
};


#endif //JDK11U_RPCMESSAGES_HPP
