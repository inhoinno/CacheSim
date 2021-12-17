#include "./include/interpreter.h"
using namespace std;

//ins type, address,  
int Text_interpreter::next_instruction(char * type,  string * addr){
    std::getline(fp,next);
    if(next.size() !=0 ){
        *(type) = next[0];
        //cout << "TEST : next_instuction() type :"<< *(type) << endl;
        *(addr) = next.substr(4,8);
        //cout << "TEST : next_instuction() addr :"<< *(addr) << endl;

        return 0;
    }
    return -1;
}

int Args_interpreter::which_cache(){
    return _cache_type;
}
int Args_interpreter::get_sets(){return _sets;}
int Args_interpreter::get_blocks(){return _blocks;}
int Args_interpreter::get_bytes(){return _bytes_in_block;}
