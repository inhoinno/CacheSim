#ifndef __INTERPRETER__
#define __INTERPRETER__

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include "./cache.h"
using namespace std;

class Text_interpreter {
private:
    std::ifstream fp;
    std::string next;
    //char next_ins;
    //std::string mem_addr;


public :
    Text_interpreter();
    Text_interpreter(std::string filename){
        fp.open(filename);
        if(!fp.is_open()){
            if (!fp.is_open()) 
                std::cout << " FILE open ERROR : " << filename << "has not opened ..." <<std::endl;
            
        }else{
            std::cout << " FILE opened : " << filename << "..." <<std::endl;

        }
    }
    int next_instruction(char * type,  string * addr);

};

class Args_interpreter {
    private:
    int _sets;
    int _blocks;
    int _bytes_in_block;
    string _which_cache;
    int _cache_type; //0:Direct-Mapped 1:N-way-Set 2: Fully-associate
    string _write_allocation;
    string _write_policy;
    int _evict_type;
    string _evict_policy;

    public:
    Args_interpreter();
    Args_interpreter(int n_sets, int n_blocks, int n_bytes, 
                    string write_alloc, 
                    string write_pol, 
                    int eviction_pol){
        _sets = n_sets;
        _blocks = n_blocks;
        _bytes_in_block = n_bytes;
        if (_sets==1){
            if(_blocks >= 2){
                //then Fully associative cache
                _which_cache = "fully associative cache";
                _cache_type = FULLY_ASSOCIATIVE_CACHE;

            }else if(_blocks ==1){ // minicache{
                _which_cache = "direct_mapped cache";
                _cache_type = DIRECT_MAPPED_CACHE;
            }
        }
        else if(_sets >= 2){
            if(_blocks==1){
                //then direct mapped cache
                _which_cache = "direct_mapped";
                _cache_type = DIRECT_MAPPED_CACHE;
            }else if(_blocks >= 2){
                //then N-way Set associative cache
                _which_cache = "N-Way set asscociative cache";
                _cache_type = N_WAY_SET_ASSOCIATIVE_CACHE;
                std::replace(_which_cache.begin(), _which_cache.end(), 'N', (char)(_blocks+48));

            }
        }else {
            
            cout<<"     Input WARNING ::: ARGV undefined :: sets : " << _sets <<" blocks : "<< _blocks << " bytes :"<< _bytes_in_block << "  " << endl;
            cout<<"     Input WARNING ::: work with default (TEST_ALL_CACHE) "<<endl;
            _cache_type = TEST_ALL_CACHE; 
        }
        
        /*
        if (write_allocation){
            //then 
        }
        if (write_poilcy){

        }
        if(eviction_policy){
            evict_type = eviction_policy ;
            evict()
        }*/
        int i=0;
        int v = _sets*_blocks*_bytes_in_block;
        for(i=0; (v/1024)!=0; i++) v %= 1024;

        cout <<" Args_interpreter::cache type         : "<< _which_cache << "(type:"<< _cache_type << ")"<<endl;
        cout <<" Args_interpreter::cache sets         : "<< _sets << " sets" <<endl;
        cout <<" Args_interpreter::cache blocks       : "<< _blocks << " blocks" <<endl;       
        cout <<" Args_interpreter::cache bytes per block: " << _bytes_in_block << " bytes" <<endl;
        //cout <<" Args_interpreter::cache size         : "<< v <<" bytes"<<endl;
        cout <<" Args_interpreter::cache size         : "<< (double)_sets*_blocks*_bytes_in_block/pow((double)1024,(double)i) << [](int i) ->string { return (i==0)? "bytes": (i==1)? " KiB": (i==2)? "MiB" : "GiB";}(i) <<endl;
        cout <<" Args_interpreter::write allocation   : "<< write_alloc <<endl;
        cout <<" Args_interpreter::write policy       : "<< write_pol <<endl;
        cout <<" Args_interpreter::eviction policy    : "<< eviction_pol <<endl;
        
    }
    int which_cache();
    int get_sets();
    int get_blocks();
    int get_bytes();
};

#endif