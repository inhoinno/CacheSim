#pragma once

#include "./cache.h"
class N_Way_Set_cache : public ICache{
private: 
    std::vector<std::vector<std::bitset<32> *>> tagarray; //e.g. [index(3) valid(1) tag(8)] then 
                                          // (tagarray[index] & 1<<__valid_bit_offset) check valid bit
                                          // (tagarray[index] & 1*tagsize << __tag_bit_offset) is tag
    uint32_t __index_bit_offset;
    uint32_t __tag_bit_offset;
    uint32_t __valid_bit_offset;    //these field is use for check valid bit like real world cache.
    uint32_t __dirty_bit_offset;    //e.g [1 deadbeef 1] then check valid bit like (tagarray[index] & 1<<__valid_bit_offset)==1 
                                    //     ^valid     ^dirty
    //bit size
    uint32_t _offset_size;
    uint32_t _tag_size;
    uint32_t _index_size;
    uint32_t _valid_bit_size =1;
    uint32_t _dirty_bit_size =1;

public :
    N_Way_Set_cache();
    N_Way_Set_cache(int mem_address_size , int sets, int blocks, int bytes_per_block){
        //if N-Way Set Associative Cache, then block=N and set=m
        // Data Cache size = sets * blocks * bytes_per_block
        // # of Entries = cache size / bytes_per_block
        // there is # of Entries in total tag store (N-way)
        // if N-way, #ofEntries/N
        //      offset_bits = log2(bytes_per_block)
        //      index_bits = log2(# of Entries)
        //  32-offset_bits-index_bits
        _offset_size = (uint32_t)log2(bytes_per_block);
        _index_size = (uint32_t)log2(sets);
        _tag_size = (uint32_t)(mem_address_size) - _index_size - _offset_size;

        cout <<"  N-way-Associative_cache:: offset  :" << _offset_size << " bits"<<endl; 
        cout <<"  N-way-Associative_cache:: index   :" << _index_size << " bits"<<endl; 
        cout <<"  N-way-Associative_cache:: tag     :" << _tag_size << " bits"<<endl; 
        
        for (int i =0; i<blocks; i++)
            tagarray[i].assign(pow(2,_index_size), new bitset<32>(0));

        cout <<"  Direct_mapped_cache:: tagarray:"<< tagarray.size()<<" entries"<<endl;

    }
};