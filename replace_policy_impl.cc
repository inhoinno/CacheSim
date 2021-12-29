#include "./include/cachelog.h"
#include "./include/cache.h"
#include "./include/N-way-Set.h"
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

    /*
    guess this is 4-way then 3bits is setted
    and let this 3bits is like 110 then 
    */             
    //bitset<32> access_bit_array                             

    for ( i=0 ; i<level ; i++){                            //i = 0 1 2
        val = access_array[cache_index].at(index);
        if(val == 1)  //if 0 then right side is referenced more recently
            whichway.flip(level-1-i);   //make bitset
            //then go left side of tree;
            //stack2.push(left) 
            //else 
            //then left is referenced more recently
            //then go right side of tree; 
            //stack2.push(right)        
        index = index*2^i + val +1 
    } 
    way = whichway.ulong();
    cout << "replacement_lru_treeplru::access_bit_field:"<<access_bit_array[cache_index].to_string() <<" way:"<< way <<endl;
    return way;
}
/*
int N_Way_Set_cache::replacement_lru_secondchance(uint32_t cache_index){
    
}*/