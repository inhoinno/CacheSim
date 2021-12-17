#pragma once
#include "./cache.h"
//word size 32 bit

using namespace std;

class Fully_associate_cache : public ICache{
    std::vector<std::bitset<32> *> tagarray; //e.g. [index(3) valid(1) tag(8)] then 
                                          // (tagarray[index] & 1<<__valid_bit_offset) check valid bit
                                          // (tagarray[index] & 1*tagsize << __tag_bit_offset) is tag
    uint32_t __index_bit_offset;
    uint32_t __tag_bit_offset;
    uint32_t __valid_bit_offset;    //these field is use for check valid bit like real world cache.
    uint32_t __dirty_bit_offset;    //e.g [1 tag data 1] then check valid bit like (tagarray[index] & 1<<__valid_bit_offset)==1 
                                    //     ^valid     ^dirty

    //bit size
    uint32_t _offset_size;
    uint32_t _tag_size;
    uint32_t _index_size;
    uint32_t _valid_bit_size =1;
    uint32_t _dirty_bit_size =1;


public :
    Fully_associate_cache();
    //Direct_mapped_cache Constructor construct tagarray and bit field(valid, dirty)
    Fully_associate_cache(int mem_address_size , int sets, int bytes_per_block){
        //if direct_mapped_cache, then blocks=1 and sets=n
        //log_2(bytes_per_block) bits 만큼 offset bit 필요
        _offset_size = (uint32_t)log2(bytes_per_block);
        _index_size = (uint32_t)log2(sets);
        _tag_size = (uint32_t)(mem_address_size) - _index_size - _offset_size;

        cout <<"  Direct_mapped_cache:: offset  :" << _offset_size << " bits"<<endl; 
        cout <<"  Direct_mapped_cache:: index   :" << _index_size << " bits"<<endl; 
        cout <<"  Direct_mapped_cache:: tag     :" << _tag_size << " bits"<<endl; 

        tagarray.assign(pow(2,_index_size), new bitset<32>(0));
        cout <<"  Direct_mapped_cache:: tagarray:"<< tagarray.size()<<" entries"<<endl;


    }
    void set_write_policy(int wp);
    void placement_policy(char operation_type, string physical_address);
    void replacement_policy();
    int cache_lookup_algorithm(std::string physical_address);

    uint32_t get_tag(std::string physical_address);
    uint32_t get_index(std::string physical_address);
    uint32_t cache_lookup(uint32_t cache_index);
    void store_tag(std::string physical_address);
    bool is_valid(uint32_t cache_index);
    bool is_dirty(uint32_t cache_index);
    void set_valid_bit(uint32_t cache_index);
    void set_dirty_bit(uint32_t cache_index);
};