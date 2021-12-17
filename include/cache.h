#ifndef __CACHE__
#define __CACHE__

#pragma once
#include <vector>
#include <bitset>
#include <cmath>
#include <iostream>
#include <string>
#define DIRECT_MAPPED_CACHE 0
#define N_WAY_SET_ASSOCIATIVE_CACHE 1
#define FULLY_ASSOCIATIVE_CACHE 2
#define TEST_ALL_CACHE 3
#define interface class
//For here, 1byte=1bit
extern const uint32_t _WORD_SIZE;  
extern const uint32_t _MEM_ADDRESS_SIZE; 


class ICache {
protected:
    //std::vector<std::vector<int>> ** tag_store; 
    uint32_t __index_bit_offset;
    uint32_t __tag_bit_offset;
    uint32_t __valid_bit_offset;    //these field is use for check valid bit like real world cache.
    uint32_t __dirty_bit_offset;    //e.g [1 deadbeef 1] then check valid bit like (tagarray[index] & 1<<__valid_bit_offset)==1 
                                    //     ^valid     ^dirty
    uint32_t __access_bit_offset;
    //bit size
    uint32_t _access_size=3;
    uint32_t _offset_size;
    uint32_t _tag_size;
    uint32_t _index_size;
    uint32_t _valid_bit_size =1;
    uint32_t _dirty_bit_size =1;

    int index_offset;
    int index_size;
    int tag_offset;
    int tag_size; 
    int set_size;
    int entry_per_set;
    int block_size;
    int cache_size;
    int dirtybit;
    int eviction_bit;
    char * cachename;
    int eviction_policy;
    bool write_through=false;
    bool write_back=true;

public : 
    ICache(){};
    ~ICache(){};
    //ICache(int sets, int blocks, int bytes_per_block){};

    //__int8 tagstore[512][512] = {0};
    // Index field access : tagstore[index_offset:index_offset+index_size]
    // Tag field access : tagstore[tag_offset:tag_offset+tag_size]
    // Valid field access : tagstore[valid_offset:valid_offset+valid_bit_size]    
    void placement_policy(char operation_type, std::string physical_address);
    int cache_lookup_algorithm(std::string physical_address);
    void eviction_algorithm(char operation_type, std::string physical_address);
    void replacement_policy();
    void _write_mem();
    uint32_t get_tag(std::string physical_adderss);
    uint32_t get_index(std::string physical_address);
    uint32_t cache_lookup(uint32_t cache_index);
    
    bool is_valid(uint32_t cache_index);
    void set_valid_bit(uint32_t cahce_index);
    bool is_dirty(uint32_t cache_index);
    void set_dirty_bit(uint32_t cache_index);
    void store_tag(std::string physical_address);

    /*
    int set_dirtybit(    );
    int clear_dirtybit(     );
    int set_cachename(string name);
    void sanitize();*/
};
#endif