#include "./include/cachelog.h"
#include "./include/interpreter.h"
#include "./include/cache.h"
#include "./include/direct_mapped.h"
#include "./include/N-way-Set.h"
#include <iostream>
#include <bitset>
#include <sstream>
#include <cstdlib>

constexpr uint32_t _WORD_SIZE = 4;          //4byte
constexpr uint32_t _MEM_ADDRESS_SIZE = 32;    //4GB
cachelog::Cache_event * logger = new cachelog::Cache_event();
using namespace std;
//Cache * L1 = new Cache

string * hex_to_binary(string hex_str){ 
    stringstream ss ;
    unsigned n;
    ss << std::hex << hex_str;
    ss >> n;
    bitset<32> bin(n);
    string * bin_str = new string(bin.to_string());
    return bin_str;
}
bitset<32> hex_to_bitset(string hex_str){ 
    stringstream ss ;
    unsigned n;
    ss << std::hex << hex_str;
    ss >> n;
    bitset<32> bin(n);
    //string * bin_str = new string(bin.to_string());
    return bin;
}
/*
argv[1] : filename
argv[2] : number of sets in cache
argv[3] : number of blocks in each set
argv[4] : number of bytes in each block
argv[5] : write-allocate or no-write-allocate
argv[6] : write-through or write-back
argv[7] : lru, fifo, random eviction
*/
int main(int argc, char* argv[]){
    //vars
    int mem_address_size = _MEM_ADDRESS_SIZE;
    char type = 'c';
    int argcount = argc;
    string addr("?");
    string st(argv[1]);
    Args_interpreter * ai; 
    Text_interpreter * ti = new Text_interpreter(st);
    Direct_mapped_cache * dmcache;
    N_Way_Set_cache * nscache;
    Fully_associate_cache * facache;
    //N_Way_Set_cache * 
    if(argcount == 8) {
        ai= new Args_interpreter(atoi(argv[2]),   //int n sets
                                atoi(argv[3]),    //int n_blocks
                                atoi(argv[4]),    //int n_bytes_in_blocks
                                string(argv[5]),    //string write_allocation
                                string(argv[6]),    //string write_policy
                                atoi(argv[7]));   //int evict_type lru,fifo,random

    switch (ai->which_cache()){
        case 0:     //then directed mapped 
            dmcache = new Direct_mapped_cache(mem_address_size, ai->get_sets(),ai->get_bytes());
        break;

        case 1:     //then N way m block cache
            nscache = new N_Way_Set_cache(mem_address_size, ai->get_sets(),ai->get_blocks(),->get_bytes())

        break;

        case 2:     //then Fully associate cache

        default : 
            cout<<"unkwon cache args" << argv[2] <<"  "<< argv[3]<< "  "<< argv[4]<< "  " << endl;
        break;
    }
    }

    //logic
    cout << "argc : "<<argcount<<endl;
    while ( !(ti->next_instruction(&type, &addr) < 0) ){
        //pass to cache type and addr
        // then cache will do l or s 
        // L1.
        dmcache->placement_policy(type, addr);



    }
    logger->summary();
    cout << "Termiated Gracefully." <<endl;
    return 0;
}