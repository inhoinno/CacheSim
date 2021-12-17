/*
32152332 송인호 
2021 FW Computer Architecture   

Cache simulator :: Cache 
*/

//pseudo code
CPU request data by Memory_address
Cache check Memory_address is in Cache
    if Memory_address is in Cache       # then _hit
        update history_buffer
    else if Memory_address is not in Cache # then miss
        // 1. request Memory data
        then Cache request Memory(or Memory Controller) data by Memory_address
        Memory returns data 
        if Cache needs to evict (= needs to flush cacheline)
            Call this.replacement_policy
            update history buffer
            Cache gives evicted Cacheline to memory #Write-back here 
            Call placement_policy
            
    return Cache_lookup(Memory_address) # return data
    