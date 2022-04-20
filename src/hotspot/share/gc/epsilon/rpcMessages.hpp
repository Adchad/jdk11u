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

struct msg_set_end{
    HeapWord* value;
};




#endif //JDK11U_RPCMESSAGES_HPP
