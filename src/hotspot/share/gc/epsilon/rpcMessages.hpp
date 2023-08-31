//
// Created by adam on 4/11/22.
//

#ifndef JDK11U_RPCMESSAGES_HPP
#define JDK11U_RPCMESSAGES_HPP

#include <stdlib.h>
#include "oops/objArrayKlass.hpp"
#include "oops/arrayOop.inline.hpp"
#include "oops/oop.hpp"
#include "oops/oop.inline.hpp"
#include <unistd.h>

#define SHM_NAME "prout"
#define SHM_SIZE (16*1024)

// Header Params
#define HEADER_OFFSET 8  // size of HEADER
#define SIZE_OFFSET 8 // offset of size (the offset are negative)
#define KLASS_OFFSET 4 // offset of klass ID


// Global parameters
#define RSPACE_PORT 42069 // port of socket btwn GC and JVM
#define COLLECT_PORT 42071 // port of socket btwn GC and JVM
#define KLASSNAME 0 // give the class name to gc ?
#define GCHELPER 1 // help from JVM for some complicated objects in the GC
#define DEADBEEF 0 // mark collected objects by replacing data with deadbeef
#define REMOTE_LOADING 1 // instrumentate classloader to send data to GC
#define ALLOC_BUFFER 1

// Allocation buffer parameters
#define BUFFER_SIZE 30 // amount of pointers stored in the allocation buffer in the JVM side
#define BUFFER_MAX_SIZE 256 // max size of object that can be allocated from the buffer

enum klass_type {instance = 1, objarray = 2, typearray = 3, instanceref = 4, instancemirror = 5, instanceclassloader = 6};

typedef struct opcode_t{
	char type;
	char pad[7];

}opcode;

struct msg_initialize{
	opcode msg_type;
    HeapWord* mr_start;
    size_t mr_word_size;
    uint32_t obj_array_base;
	uint32_t obj_array_length;
	pid_t pid;
    bool clear_space;
    bool mangle_space;
};

struct msg_par_allocate{
	opcode msg_type;
    uint64_t word_size;
};

struct msg_par_allocate_klass{
	opcode msg_type;
    uint32_t word_size;
	uint32_t klass;
};

struct msg_request_buffer{
	opcode msg_type;
    uint32_t word_size;
};

struct msg_set_end{
	opcode msg_type;
    HeapWord* value;
};

struct msg_alloc_response{
    HeapWord* ptr;
	uint64_t send_metadata;
};

struct msg_klass_data{
    //TODO: Nettoyer ce struct de merde
    klass_type klasstype;
	int64_t name_length;
	uint32_t special;
	uint32_t length;
    uint64_t base_klass = 0; //only for obj_array
    BasicType basetype = T_ILLEGAL; //only for typearray
    int layout_helper;
};

struct msg_klass_data_2{
    //TODO: Nettoyer cet autre struct de merde
	opcode msg_type;
    uint32_t klass;
    klass_type klasstype;
	uint32_t special;
    uint32_t length;
    uint64_t base_klass = 0; //only for obj_array
    BasicType basetype = T_ILLEGAL; //only for typearray
    int layout_helper;
};


struct msg_collect{
	opcode msg_type;
	uint64_t base;
	int32_t shift;
    uint32_t static_base;
    uint32_t static_length;
};


#endif //JDK11U_RPCMESSAGES_HPP
