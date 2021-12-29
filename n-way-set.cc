#include "./include/cache.h"
#include "./include/cachelog.h"
#include "./include/n-way-set.h"
#define MISS_PENALTY_CPU_CYCLE 100
using namespace std;
extern cachelog::Cache_event * logger;
extern bitset<32> hex_to_bitset(string hex_str);
/*

ref : https://people.cs.clemson.edu/~mark/464/p_lru.txt

pseudo-LRU

two-way set associative - one bit

   indicates which line of the two has been reference more recently


four-way set associative - three bits

   each bit represents one branch point in a binary decision tree; let 1
   represent that the left side has been referenced more recently than the
   right side, and 0 vice-versa

              are all 4 lines valid?
                   /       \
                 yes        no, use an invalid line
                  |
                  |
                  |
             bit_0 == 0?            state | replace      ref to | next state
              /       \             ------+--------      -------+-----------
             y         n             00x  |  line_0      line_0 |    11_
            /           \            01x  |  line_1      line_1 |    10_
     bit_1 == 0?    bit_2 == 0?      1x0  |  line_2      line_2 |    0_1
       /    \          /    \        1x1  |  line_3      line_3 |    0_0
      y      n        y      n
     /        \      /        \        ('x' means       ('_' means unchanged)
   line_0  line_1  line_2  line_3      don't care)

   (see Figure 3-7, p. 3-18, in Intel Embedded Pentium Processor Family Dev.
    Manual, 1998, http://www.intel.com/design/intarch/manuals/273204.htm)


note that there is a 6-bit encoding for true LRU for four-way set associative

  bit 0: bank[1] more recently used than bank[0]
  bit 1: bank[2] more recently used than bank[0]
  bit 2: bank[2] more recently used than bank[1]
  bit 3: bank[3] more recently used than bank[0]
  bit 4: bank[3] more recently used than bank[1]
  bit 5: bank[3] more recently used than bank[2]

  this results in 24 valid bit patterns within the 64 possible bit patterns
  (4! possible valid traces for bank references)

  e.g., a trace of 0 1 2 3, where 0 is LRU and 3 is MRU, is encoded as 111111

  you can implement a state machine with a 256x6 ROM (6-bit state encoding
  appended with a 2-bit bank reference input will yield a new 6-bit state),
  and you can implement an LRU bank indicator with a 64x2 ROM
*/
//uint32_t N_Way_Set_cache::replacement_lru_treeplru(uint32_t cache_index)
//args : cache_index
// returns : victim's way number of cache_index
uint32_t N_Way_Set_cache::replacement_lru_treeplru(uint32_t cache_index){
    uint32_t val = 0;
    uint32_t i =0;               
    uint32_t way = 0;  //level,i                            
    uint32_t index = 0; //access_bit_size-(i*2) -1          
                        //access_bit_size-(i*2 +1)-1        
    uint32_t level = _access_bit_size;
    bitset<32> whichway(0);
    bitset<32> * tmp=access_array[0][cache_index];

    
    access_array[0][cache_index]->flip();
    cout << "   replacement_lru_treeplru:intentional flip" << endl;
    
    /*
    guess this is 4-way then 3bits is setted
    and let this 3bits is like 110 then 
    */             
    //bitset<32> access_bit_array                             

    for ( i=0 ; i<level ; i++){                            //i = 0 1 2
        val = access_array[0][cache_index][0][index];
        cout << "   replacement_lru_treeplru:tmp " << tmp[level-1-i][0] << endl;
        cout << "   replacement_lru_treeplru:val " << val << endl;
        if(val == 1)  //if 0 then right side is referenced more recently
            whichway.flip(level-1-i);   //make bitset
            //then go left side of tree;
            //stack2.push(left) 
            //else 
            //then left is referenced more recently
            //then go right side of tree; 
            //stack2.push(right)        
        index = index * (2^i) + val +1;
    } 
    way = whichway.to_ulong();
    cout << "replacement_lru_treeplru::access_bit_field:"<<access_array[0][cache_index]->to_string() <<" way:"<< way <<endl;
    return way;
}
/*
int N_Way_Set_cache::replacement_lru_secondchance(uint32_t cache_index){
    
}*/

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
    bitset<32> tag(hex_to_bitset(physical_address).to_ulong());

	mask = mask.flip();
	mask = mask << (_offset_size+_index_size);  //1111 1111 00 00
	tag = tag & mask;
	tag = tag >> (_index_size + _offset_size);
    return tag.to_ulong();
}
uint32_t N_Way_Set_cache::get_index(std::string physical_address){
    bitset<32> mask(0);
    bitset<32> umax(0);
    bitset<32> idx(hex_to_bitset(physical_address).to_ulong());
    mask=mask.flip();			//1111 1111
    umax=umax.flip();			//1111 1111
    mask=mask << _offset_size;      	//1111 1100  xor 11 00 00 = 00 11 00
    umax=umax << (_index_size+_offset_size);
    mask = mask ^ umax ; 
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
    cout<<"N_Way_Set_cache::store_tag:tag:"<<tag<<endl;
    cout<<"N_Way_Set_cache::store_tag:idx:"<<idx<<endl;

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

void N_Way_Set_cache::set_replacement_policy(int type){
    if(type == 0){
        //then LRU
        _replacement_policy = 0;
    }else if(type == 1){
        //then random
        _replacement_policy = 1;
    }   
}

uint32_t N_Way_Set_cache::replacement_policy(uint32_t cache_index){
    //cout<< "N_Way_Set_cache::replacement_policy : " <<endl;
    uint32_t idx = cache_index;
    //#ifdef __REPLACEMENT_POLICY_RANDOM
    uint32_t rdx;
    uint32_t eway;
    if(_replacement_policy == 0){//#ifdef __REPLACEMENT_POLICY_LRU
        /*
        in Set Associative Cache, we need some bit field to recognize which block is lru
        More precisely, we take Tree-PLRU because cache plru evicts victims in hardware level
        So, when desing a cache simulator, simple LinkedList scheme in Virtual Memory may not a proper approach 
        */
        eway = replacement_lru_treeplru(idx);
        tagarray[eway][idx]->reset();
        return eway;
    }//#endif
    else if(_replacement_policy == 1){ 
        rdx= rand() % _blocks;    //make random number
        tagarray[rdx][idx]->reset(); //evict tagarray[rdx][cache_index]        
        return rdx;
    }//#endif
    return 0;
}
