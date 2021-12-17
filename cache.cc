#include "./include/cachelog.h"
#include "./include/cache.h"
#include "./include/direct_mapped.h"
#include "./include/N-way-Set.h"
#define MISS_PENALTY_CPU_CYCLE 100
using namespace std;
extern cachelog::Cache_event * logger;
extern bitset<32> hex_to_bitset(string hex_str);

int ICache::cache_lookup_algorithm(std::string physical_address){
    if(get_tag(physical_address) == cache_lookup(get_index(physical_address)) && is_valid(get_index(physical_address))){
        //then HIT
        return 1;
    }
    return 0;
}
void ICache::eviction_algorithm(char operation_type, std::string physical_address){ 
    cout << "eviction "<<operation_type<<physical_address<<endl;    
    return;
    
}

uint32_t ICache::get_tag(std::string physical_adderss){
    cout<<"ICache::get_tag called"<< physical_adderss <<endl;
    return 0;
}
uint32_t ICache::get_index(std::string physical_address){
    cout<<"ICache::get_index called"<< physical_address <<endl;
    return 0;
}
uint32_t ICache::cache_lookup(uint32_t cache_index){
    cout<<"ICache::cache_lookup called"<< cache_index <<endl;
    return 0;
}
bool ICache::is_valid(uint32_t cache_index){
    cout<<"ICache::is_valid_bit called"<< cache_index <<endl;
    return false;
}

void ICache::set_valid_bit(uint32_t cache_index){
    cout<<"ICache::set_valid_bit called"<< cache_index <<endl;
    return ;
}
void ICache::set_dirty_bit(uint32_t cache_index){
    cout<<"ICache::set_dirty_bit called"<< cache_index <<endl;
    return ;
}

int Direct_mapped_cache::cache_lookup_algorithm(std::string physical_address){
    cout << "direct_mapped_cache::cache_lookup_algorithm : " << get_tag(physical_address) << " == " << cache_lookup(get_index(physical_address))<<endl;
    if(get_tag(physical_address) == cache_lookup(get_index(physical_address)) && is_valid(get_index(physical_address))){
        //then HIT
        return 1;
    }
    return 0;
}

void Direct_mapped_cache::placement_policy(char operation_type, std::string physical_address){
    bool write_flag=false;    
    
    cout<< "direct_mapped_cache::placement_policy : operation:"<< operation_type<<" address:" << physical_address <<endl;
    if(operation_type == 'l'){
        logger->load();
    }else if(operation_type == 's'){
        logger->store();
        write_flag=true;
    }
    if(cache_lookup_algorithm(physical_address)==1){
        //Cache Hit :: This simulator doesnt care datastore 
        // so if ld then set access_bit and return data to cpu is enough
        // if sd then set dirty_bit is enough 
        cout<< operation_type<< " "<<physical_address <<" HIT(tag:"<< get_tag(physical_address)<<"  index:"<<get_index(physical_address)<<")"<<endl;
        if (write_flag) {
            set_dirty_bit(get_index(physical_address));        
            logger->store_hit();
        }else{
            logger->load_hit();
        }
        //return data[get_index(pm)] to cpu
    }else{
        //Cache Miss :: This simulator doesnt care datastore
        // so read physical address from memory
        //  if cache needs eviction
        //      then call eviction algorithm so make cache has a room
        //  place physical address block to cache. tag updated.
        
        if(!is_valid(get_index(physical_address))){
            
            logger->cold_miss();
            store_tag(physical_address);
            set_valid_bit(get_index(physical_address));

            if (!write_flag) {  
                //if ld instruction coldmiss then goto mem and fetch data
                logger->load_miss();
                logger->miss_penalty(MISS_PENALTY_CPU_CYCLE*_offset_size);  //100cycle per word
            }else{
                logger->store_miss();
                if(write_through){
                    //if sd insturction coldmiss and write through then goto mem directly
                    //and also write through doesnt store data 
                    logger->miss_penalty(MISS_PENALTY_CPU_CYCLE*_offset_size);
                }
            }
        }
        else if(get_tag(physical_address) != cache_lookup(get_index(physical_address))){
            logger->conflict_miss(); 

            //now eviction done with write allocation & policy

            //load data to mem
            logger->miss_penalty(MISS_PENALTY_CPU_CYCLE*_offset_size);
            //now data loaded to cache
            store_tag(physical_address);
            if (write_flag) {
                logger->store_miss();
                set_dirty_bit(get_index(physical_address));
            }else{
                logger->load_miss();
            }
        }
    }
    return;
}
void Direct_mapped_cache::replacement_policy(){
    cout<< "direct_mapped_cache::replacement_policy : " <<endl;
    return;
}

void Direct_mapped_cache::store_tag(std::string physical_address){
    uint32_t tag = get_tag(physical_address);
    tagarray[get_index(physical_address)] = new bitset<32>(tag);
    return;

}
uint32_t Direct_mapped_cache::get_tag(std::string physical_address){
    bitset<32> mask(0);
    for (uint32_t i = _offset_size+_index_size; i<_tag_size; i++)
        mask.flip(i);
    bitset<32> tag(mask & hex_to_bitset(physical_address));
    //cout<< " Direct_mapped_cache:: get_tag(0x"<< physical_address << "): "<< tag.to_ulong() << endl;
    return tag.to_ulong();
}
uint32_t Direct_mapped_cache::get_index(std::string physical_address){
    bitset<32> mask(0);
    for (uint32_t i = _offset_size; i<_index_size; i++)
        mask.flip(i);
    bitset<32> idx(mask & hex_to_bitset(physical_address));
    //cout<< " Direct_mapped_cache:: get_index(0x"<< physical_address << "): "<< idx.to_string() << endl;
    return idx.to_ulong();
}

uint32_t Direct_mapped_cache::cache_lookup(uint32_t cache_index){
    //Direct mapped cache에서는 index의 tag를 봄
    bitset<32> mask(0);
    bitset<32> umax(0);
    bitset<32> tag(tagarray.at(cache_index)->to_ulong());
    mask = mask.flip(); 
    umax = umax.flip();
    mask = mask << _tag_size;   // 11 000
    mask = mask ^ umax;
    mask &= tag;
    //cout<<" Direct_mapped_cache::cache_lookup: mask: " <<mask.to_string()<<endl;

    return mask.to_ulong();
}

bool Direct_mapped_cache::is_valid(uint32_t cache_index){
    return tagarray[cache_index]->test(_tag_size);
}
void Direct_mapped_cache::set_valid_bit(uint32_t cache_index){
    tagarray[cache_index]->set(_tag_size);
    return;
}

bool Direct_mapped_cache::is_dirty(uint32_t cache_index){
    return tagarray[cache_index]->test(_tag_size+1);
}
void Direct_mapped_cache::set_dirty_bit(uint32_t cache_index){
    tagarray[cache_index]->set(_tag_size+1);
    return;
}