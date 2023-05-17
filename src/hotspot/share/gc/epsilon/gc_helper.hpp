//
// Created by adam on 5/3/22.
//

#ifndef JDK11U_GCHELPER_HPP
#define JDK11U_GCHELPER_HPP

#include <stdio.h>
#include <cstring>
#include <mutex>
#include "memory/iterator.hpp"
#include "memory/iterator.inline.hpp"
#include "gc/epsilon/roots.hpp"
#include "oops/oop.inline.hpp"
#include "oops/instanceMirrorKlass.inline.hpp"
#include "oops/instanceRefKlass.inline.hpp"


class HelperClosure : public BasicOopIterateClosure  {
private:
	template <class T>
  	void do_oop_work(T* p) {
		T o = RawAccess<>::oop_load(p);
		if (!CompressedOops::is_null(o)) {
			oop obj = CompressedOops::decode_not_null(o);
			do_oop(&obj);
		}
  	}

public:
    struct linked_list* head = NULL;
    int list_length = 0;
	ReferenceProcessor* rp;
public:
	void initialize(){
	}

	void set_rp(ReferenceProcessor* _rp){
		rp = _rp;
		set_ref_discoverer_internal(rp);
	}

	ReferenceIterationMode reference_iteration_mode() { return DO_DISCOVERY; }	

	bool do_metadata(){return false;}
	
	void do_klass(Klass* k){}

	void do_cld(ClassLoaderData* k){}

    //void do_oop(oop* o){
	//	if(*o != nullptr){
	//		struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
    //    	list_length++;
    //    	struct linked_list* curr = head;
    //    	new_node->value = (HeapWord*) *o;
    //    	new_node->next = curr;
    //    	head = new_node;
	//	}
    //};

    //void do_oop(narrowOop* o){
	//	if(*o != 0){
	//		//printf("narrow root: 0x%x\n", *o);
	//		struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
    //    	list_length++;
    //    	struct linked_list* curr = head;
    //    	new_node->value = (HeapWord*) oopDesc::decode_oop_raw(*o);
    //    	new_node->next = curr;
    //    	head = new_node;
	//	}
    //};

    void do_oop(oop* o){
		if(*o != nullptr){
			if(head == nullptr){
				struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
				new_node->value = (HeapWord*) *o;
        		new_node->next = nullptr;
        		head = new_node;
				list_length++;
				return;
			}
        	struct linked_list* curr = head;
			while(curr->next !=nullptr){
				if(curr->next->value == (HeapWord*) *o){return;}
				curr = curr->next;
			}
		
			struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
        	new_node->value = (HeapWord*) *o;
        	new_node->next = nullptr;
        	curr->next = new_node;
			list_length++;
		}
    };

    void do_oop(narrowOop* o){
		if(*o != 0){
			HeapWord* word = (HeapWord*) oopDesc::decode_oop_raw(*o);
			if(head == nullptr){
				struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
				new_node->value = word;
        		new_node->next = nullptr;
        		head = new_node;
				list_length++;
				return;
			}
        	struct linked_list* curr = head;
			while(curr->next !=nullptr){
				if(curr->next->value == word){return;}
				curr = curr->next;
			}
		
			struct linked_list* new_node = (struct linked_list*) malloc(sizeof(struct linked_list));
        	new_node->value = word;
        	new_node->next = nullptr;
        	curr->next = new_node;
			list_length++;
		}
    };


    int getSize(){
        return list_length;
    }

    uint64_t* getArray(){
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
    }
};


class GCHelper {
private:
   MemRegion mr;	
public:
    HelperClosure* hc;
	struct linked_list* helper_roots;
	ReferenceProcessor* rp;

public:
	void initialize(){
		helper_roots = nullptr;
		hc = (HelperClosure*) malloc(sizeof(HelperClosure));
		HelperClosure hcp;
		memcpy(hc, &hcp, sizeof(HelperClosure));
	}

	void set_rp(ReferenceProcessor* _rp){
		rp = _rp;
		hc->set_rp(rp);
	}

    void do_it();

    int getArraySize(){
        return hc->getSize();
    }
    uint64_t* helperArray(){
        return hc->getArray();
    }

	void add_root(HeapWord* ref){
		struct linked_list* new_root = (struct linked_list*) malloc(sizeof(linked_list));
		new_root->value = ref;
		new_root->next = helper_roots;
		helper_roots = new_root;
	}



};

#endif //JDK11U_GCHELPER_HPP ")
