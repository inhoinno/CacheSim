#ifndef __LOG__
#define __LOG__

#pragma once

#include <iostream>
#include <vector>
extern const uint32_t _WORD_SIZE;  
extern const uint32_t _MEM_ADDRESS_SIZE; 
namespace cachelog{
    class Cache_event{
        private:
        uint32_t total_loads;
        uint32_t total_stores;
        uint32_t load_hit_count;
        uint32_t load_miss_count;
        uint32_t store_hit_count=0;
        uint32_t store_miss_count=0;
        uint32_t total_hit_count;
        uint32_t total_miss_count;
        uint32_t cold_miss_count;
        uint32_t capacity_miss_count;
        uint32_t conflict_miss_count;
        uint32_t total_cycle;
        uint32_t total_miss_penalty;
        std::vector<std::string> * messages;
        std::vector<std::string> * eviction_log;

        public:
        Cache_event(){ 
            messages = new std::vector<std::string>(1," Inhoinno's Cache Simulator (github.com/inhoinno/cachesim)\n"); 
            eviction_log = new std::vector<std::string>();
            total_loads=0;
            total_stores=0;
            load_hit_count=0;
            load_miss_count=0;            
            store_hit_count=0;
            store_miss_count=0;
            total_hit_count=0;
            total_miss_count=0;
            cold_miss_count=0;
            capacity_miss_count=0;
            conflict_miss_count=0;
            total_cycle=0;
            total_miss_penalty=0;
        };
        ~Cache_event();
        void put_message(std::string m){messages->push_back(m);}
        void load(){total_loads++; total_cycle++;}
        void store(){total_stores++; total_cycle++;}
        void load_hit(){load_hit_count++;}
        void load_miss(){load_miss_count++;}
        void store_hit(){store_hit_count++;}
        void store_miss(){store_miss_count++;}

        void cold_miss(){cold_miss_count++;}
        void capacity_miss(){capacity_miss_count++;}
        void conflict_miss(){conflict_miss_count++;}

        void miss_penalty(int penalty){total_miss_penalty+=penalty;}
        void summary(){
            total_cycle = total_loads+total_stores+total_miss_penalty;
            total_hit_count = load_hit_count + store_hit_count;
            total_miss_count = load_miss_count + store_miss_count;

            std::cout<<std::endl;
            std::cout<< "Total loads    : "<< total_loads       <<std::endl;
            std::cout<< "Total stores   : "<< total_stores      <<std::endl;
            std::cout<< "Load hits      : "<< load_hit_count    <<std::endl;
            std::cout<< "Load misses    : "<< load_miss_count   <<std::endl;
            std::cout<< "Store hits     : "<< store_hit_count    <<std::endl;
            std::cout<< "Store misses   : "<< store_miss_count   <<std::endl;
            std::cout<< "Total cycles   : "<< total_cycle       <<" cycles" <<std::endl;
            std::cout<< "Miss penalty   : "<< total_miss_penalty<<" cycles" <<std::endl;
            std::cout<< "Hit Ratio      : "<< (double)100*total_hit_count/(total_hit_count+total_miss_count)    <<"%"<<std::endl;
            std::cout<< "Miss Ratio     : "<< (double)100*total_miss_count/(total_hit_count+total_miss_count)   <<"%"<<std::endl;
            std::cout<< " cold miss         : "<< cold_miss_count       << " "<< 100*cold_miss_count/total_miss_count<<" (%)" <<std::endl;
            std::cout<< " capacity miss     : "<< capacity_miss_count   << " "<< 100*capacity_miss_count/total_miss_count<<" (%)" <<std::endl;
            std::cout<< " conflict miss     : "<< conflict_miss_count   << " "<< 100*conflict_miss_count/total_miss_count<<" (%)" <<std::endl;
            return;

        }


    };

}
#endif