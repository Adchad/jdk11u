//
// Created by adam on 4/11/22.
//

#ifndef JDK11U_RPCMESSAGES_HPP
#define JDK11U_RPCMESSAGES_HPP

#include <stdlib.h>



struct msg_initialize{
    uint64_t mr_start;
    uint64_t mr_word_size;
    bool clear_space;
    bool mangle_space;
};

struct msg_par_allocate{
    uint64_t word_size;
};

struct msg_set_end{
    uint64_t value;
};




#endif //JDK11U_RPCMESSAGES_HPP
