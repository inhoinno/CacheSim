#include "./include/cachelog.h"
#include "./include/cache.h"
#include "./include/direct_mapped.h"
#include "./include/N-way-Set.h"
#define MISS_PENALTY_CPU_CYCLE 100
using namespace std;
extern cachelog::Cache_event * logger;
extern bitset<32> hex_to_bitset(string hex_str);

int ICache::cache_lookup_algorithm(std::string physical_address){
    if(get_tag(physical_address) == cache_lookup(physical_address) && is_valid(physical_address)){
        //then HIT
        return 1;
    }
    return 0;
}
void ICache::eviction_algorithm(char operation_type, std::string physical_address){ 
    cout << "eviction "<<operation_type<<physical_address<<endl;    
    return;
    
}

uint32_t ICache::get_tag(std::string physical_address){
    cout<<"ICache::get_tag called"<< physical_address <<endl;
    return 0;
}
uint32_t ICache::get_index(std::string physical_address){
    cout<<"ICache::get_index called"<< physical_address <<endl;
    return 0;
}
uint32_t ICache::cache_lookup(std::string physical_address){
    cout<<"ICache::cache_lookup called"<< physical_address <<endl;
    return 0;
}
bool ICache::is_valid(std::string physical_address){
    cout<<"ICache::is_valid_bit called"<< physical_address <<endl;
    return false;
}

void ICache::set_valid_bit(std::string physical_address){
    cout<<"ICache::set_valid_bit called"<<  physical_address <<endl;
    return ;
}
void ICache::set_dirty_bit(std::string physical_address){
    cout<<"ICache::set_dirty_bit called"<<  physical_address <<endl;
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
    bitset<32> umax(0);
    bitset<32> idx(hex_to_bitset(physical_address).to_ulong());
    //cout<< " Direct_mapped_cache:: get_index(0x"<< physical_address <<"): "<< idx.to_string() << endl;
    mask=mask.flip();
    umax=umax.flip();
    mask=mask << _offset_size;      //11 11 00  xor 11 00 00 = 11 00 11
    umax=umax << _index_size;
    mask = mask ^ umax ;            //11 00 11 
    //cout<< " Direct_mapped_cache:: get_index:mask(0x"<< physical_address <<"): "<< mask.to_string() << endl;
    idx = idx & mask;
    idx = idx >> _offset_size;
    //cout<< " Direct_mapped_cache:: get_index:idx(0x"<< physical_address <<"): "<< idx.to_string()<< endl;
    return idx.to_ulong();
}

uint32_t Direct_mapped_cache::cache_lookup(uint32_t cache_index){
    //Direct mapped cache에서는 index의 tag를 봄
    bitset<32> mask(0);
    bitset<32> umax(0);
    bitset<32> tag(tagarray.at(cache_index)->to_ulong());
    mask = mask.flip();         // 11 1111
    umax = umax.flip();         // 11 1111
    mask = mask << _tag_size;   // 11 0000
    mask = mask ^ umax;         // 00 1111
    mask &= tag;                // 00 0101 & 00 1111
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

void N_Way_Set_cache::placement_policy(char operation_type, std::string physical_address){
    bool write_flag=false;   
    //uint32_t i = 0;  
    uint32_t way = 0;
    uint32_t idx = get_index(physical_address);
    uint32_t tag = get_tag(physical_address);

    cout<< "N_way_set_cache::placement_policy : operation:"<< operation_type<<" address:" << physical_address <<endl;

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
            set_dirty_bit(physical_address);        
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
        
        if(!is_valid(physical_address)){    //if is cold miss
            //then there is some room for pm 
            logger->cold_miss(); 
            store_tag(physical_address);    //store_tag
            set_valid_bit(physical_address);//

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
        else{   
            //else then there are all valid entry in cache
            //so we need replacement policy
            logger->conflict_miss(); 
            cout << "N_Way_Set_cache::store_tag call eviction algorithm" << endl;
            way = replacement_policy(idx);
            logger->miss_penalty(MISS_PENALTY_CPU_CYCLE*_offset_size);
            _store_tag(way,idx,tag);
            //now eviction done with write allocation & policy
            //load data to mem
            //now data loaded to cache
            //store_tag(physical_address);
            if (write_flag) {
                logger->store_miss();
                set_dirty_bit(physical_address);
            }else{
                logger->load_miss();
            }
        }
    }
    return;
}

uint32_t N_Way_Set_cache::get_tag(std::string physical_address){
    bitset<32> mask(0); //physical address
    bitset<32> umax(0); //pm
    bitset<32> tag(mask & hex_to_bitset(physical_address));

	mask = mask.flip();
	umax = umax.flip();
	mask = mask << (_offset_size+_index_size);//1111 1111 00 00
	umax = umax << __valid_bit_offset;      //1111 0000 00 00
	mask = mask ^ umax;					    //0000 1111 00 00
	tag = tag & mask;
	tag >> (_index_size + _offset_size);
    return tag.to_ulong();
}
uint32_t N_Way_Set_cache::get_index(std::string physical_address){
    bitset<32> mask(0);
    bitset<32> umax(0);
    bitset<32> idx(hex_to_bitset(physical_address).to_ulong());
    mask=mask.flip();			//1111 1111
    umax=umax.flip();			//1111 1111
    mask=mask << _offset_size;      	//1111 1100  xor 11 00 00 = 00 11 00
    umax=umax << _index_size;
    mask = mask ^ umax ; 
    	//cout<< " Direct_mapped_cache:: get_index:mask(0x"<< physical_address <<"): "<< mask.to_string() << endl;
    idx = idx & mask;
    idx = idx >> _offset_size;
    

	//cout<< " Direct_mapped_cache:: get_index(0x"<< physical_address << "): "<< idx.to_string() << endl;
    return idx.to_ulong();
}
uint32_t N_Way_Set_cache::cache_lookup(std::string physical_address){
    //N_Way에서는 병렬로 처리 됨. 
    //Code상에서 동적으로 Way를 가져가면서 static한 병렬을 수행할 수 없으므로 추후 반복문을 통해 비교해야함
    bitset<32> ptag(get_tag(physical_address));
    bitset<32> pidx(get_index(physical_address));
    bitset<32> * match;
    bitset<32> mask(0);
    uint32_t i = 0;
    mask = mask.flip();
    mask = mask << __valid_bit_offset;
    mask = mask.flip(); //tag, index, offset

    for (i =0; i<_blocks; i++){
        match = new bitset<32>(tagarray[i][pidx.to_ulong()]->to_ulong() & mask.to_ulong());
        if ( match->to_ulong() == ptag.to_ulong()) break;
    }
    return _cache_lookup(i, pidx.to_ulong());
}

uint32_t N_Way_Set_cache::_cache_lookup(uint32_t way, uint32_t cache_index){
    //N_Way에서는 병렬로 처리 됨. 
    //Code상에서 동적으로 Way를 가져가면서 static한 병렬을 수행할 수 없으므로 추후 반복문을 통해 비교해야함
    bitset<32> mask(0);
    bitset<32> umax(0);
    bitset<32> tag(tagarray[way].at(cache_index)->to_ulong());
    mask = mask.flip();     //1111 
    umax = umax.flip();     //1111
    mask = mask << _tag_size;   // 1000
    mask = mask ^ umax;         // 1111 -> 0111
    mask &= tag;
    return mask.to_ulong();
}
uint32_t N_Way_Set_cache::get_way(std::string physical_address){
    bitset<32> ptag(get_tag(physical_address));
    bitset<32> pidx(get_index(physical_address));
    bitset<32> * match;
    bitset<32> mask(0);
    uint32_t i = 0;
    //uint32_t j = 0;
    mask = mask.flip();
    mask = mask << __valid_bit_offset;
    mask = mask.flip(); //tag, index, offset

    for (i =0; i<_blocks; i++){
        match = new bitset<32>(tagarray[i][pidx.to_ulong()]->to_ulong() & mask.to_ulong());
        if ( match->to_ulong() == ptag.to_ulong()) break;
    }
    if (i==_blocks) return 0;
    return i;
}

bool N_Way_Set_cache::is_valid(std::string physical_address){
    uint32_t idx = get_index(physical_address);
    uint32_t i =0;
    for (i= 0; i<_blocks; i++){
        if(!_is_valid(i, idx))   //true if any entry is invalid
            break;
    }
    //return false if any one entry is invalid 
    return (i!=_blocks)? false : true;  //i:-1
}
bool N_Way_Set_cache::is_dirty(std::string physical_address){
    return _is_dirty(get_way(physical_address), get_index(physical_address));
}
void N_Way_Set_cache::set_valid_bit(std::string physical_address){
    _set_valid_bit(get_way(physical_address), get_index(physical_address));
}
void N_Way_Set_cache::set_dirty_bit(std::string physical_address){
    _set_dirty_bit(get_way(physical_address), get_index(physical_address));
}

bool N_Way_Set_cache::_is_valid(uint32_t way, uint32_t cache_index){
    //bitset<32> mask(1);     //1111  1 11 11
    bitset<32> val(tagarray[way].at(cache_index)->to_ulong()); // 0 00 00
    //mask = mask << __valid_bit_offset; //1 00 00
    //mask = mask & val; // 1 00 00 
    //return (mask.to_ulong() == 0) ? false : true ;
    return (val[__valid_bit_offset] == 1)? true: false;
}
bool N_Way_Set_cache::_is_dirty(uint32_t way, uint32_t cache_index){
    bitset<32> mask(1);
    bitset<32> dir(tagarray[way].at(cache_index)->to_ulong());
    mask = mask << __valid_bit_offset+1;
    mask = mask & dir;
    return (mask.to_ulong() ==1) ? true : false;
}
void N_Way_Set_cache::_set_valid_bit(uint32_t way, uint32_t cache_index){
    tagarray[way].at(cache_index)->set(__valid_bit_offset, 1);
}
void N_Way_Set_cache::_set_dirty_bit(uint32_t way, uint32_t cache_index){
        tagarray[way].at(cache_index)->set(__valid_bit_offset+1, 1);

}
void N_Way_Set_cache::store_tag(std::string physical_address){
    cout<<"N_Way_Set_cache::store_tag:"<<physical_address<<endl;
    uint32_t tag = get_tag(physical_address);
    uint32_t idx = get_index(physical_address);
    uint32_t way = 0;

    /* 
    Is this function necessary?
    */
    for (way = 0; way < _blocks; way++)
        if(!_is_valid(way,idx))
            break;

    if(way != _blocks)
        _store_tag(way,idx,tag);
    else if(way == _blocks){
        cout << "N_Way_Set_cache::store_tag call eviction algorithm" << endl;
        way = replacement_policy(idx);
        _store_tag(way,idx,tag);
    }
    return;
    }
void N_Way_Set_cache::_store_tag(uint32_t way, uint32_t cache_index, uint32_t ptag){
    cout<<"N_Way_Set_cache::store_tag:"<<ptag<<endl;
    uint32_t tag = ptag;
    uint32_t idx = cache_index;
    /* 
    If store tag only call by Cache miss, 
    since get_way return exact way of pm,
    then 
    */
    tagarray[way][idx] = new bitset<32>(tag);  //N-Way store tag
    return;
    }

int N_Way_Set_cache::cache_lookup_algorithm(std::string physical_address){
    uint32_t i =0 ;
    uint32_t ptag = get_tag(physical_address);
    uint32_t pidx = get_index(physical_address);

    cout<<"N_Way::cache_lookup_algorithm"<<physical_address<<endl;
	for(i=0; i<_blocks; i++){
        if(ptag == _cache_lookup(i,pidx) && _is_valid(i,pidx)){
                //then HIT
            return 1;
        }
    }
    return 0;
}
int N_Way_Set_cache::replacement_policy(uint32_t cache_index){
    //cout<< "N_Way_Set_cache::replacement_policy : " <<endl;
//#ifdef __REPLACEMENT_POLICY_RANDOM
    uint32_t rdx = rand() % _blocks;    //make random number
    tagarray[rdx][cache_index]->reset(); //evict tagarray[rdx][cache_index]        
    return rdx;
//#endif
//#ifdef __REPLACEMENT_POLICY_LRU
    /*
    in Set Associative Cache, we need some bit field to recognize which block is lru
    More precisely, 
    1bit is needed to recognize which block set (2~4)
    1bit is needed to recognize which block
    */
//#endif
}