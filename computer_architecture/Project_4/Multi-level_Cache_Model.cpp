/*
2024 Spring, Computer Architecture 
Project 4: Multi-level Cache Model

Student ID : 201811118
Name       : Gu Lee
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <regex>
#include <map>
#include <random>

using namespace std;

struct Cache_Block{
    unsigned int valid_bit;
    unsigned int dirty_bit;
    unsigned long long ref_cnt;
    unsigned long long tag;
    unsigned long long block_addr;
};


vector<string> split_line(string input, char delimiter){ 
    vector<string> words;    
    stringstream sstream(input);    
    string word;
    while (getline(sstream, word, delimiter)) {
        if (!word.empty())
            words.push_back(word);    
    }    
    return words;
}


int make_random_int(int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, max-1);
    int random_val = distrib(gen);
    return random_val;
}

unsigned long long str2ull(string num_str){ 
    unsigned long long val = 0;
    if (num_str.rfind("0x", 0) == 0){ 
        int str_length = num_str.length();
        char num_char[str_length+1];
        strcpy(num_char, num_str.c_str());
        val = stoull(num_char, NULL, 0);
    }
    else{
        val = stoull(num_str);
    }
    return val;
}

int log2(int num){
    int power = 0;
    while (num > 1) {
        num >>= 1;
        power++;
    }
    return power;
}

int main(int argc, const char* argv[]){
    bool arg_c = 0; 
    bool arg_a = 0;
    bool arg_b = 0;
    bool arg_lru = 0;
    bool arg_random = 0;

    int idx_c = -1;
    int idx_a = -1;
    int idx_b = -1;
    int idx_lru_or_random = -1;
    int idx_filename = -1;

    unsigned int l1_capa = 0;
    unsigned int l1_way = 0;
    unsigned int l2_capa = 0;
    unsigned int l2_way = 0;
    unsigned int block_size = 0;
    unsigned int total_access = 0;
    unsigned int read_access = 0;
    unsigned int write_access = 0;
    unsigned int l1_read_misses = 0;
    unsigned int l2_read_misses = 0;
    unsigned int l1_write_misses = 0;
    unsigned int l2_write_misses = 0;
    unsigned int l1_clean_eviction = 0;
    unsigned int l2_clean_eviction = 0;
    unsigned int l1_dirty_eviction = 0;
    unsigned int l2_dirty_eviction = 0;

    string filename="";
    vector<string> args;

    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];
        string str_arg(arg);
        args.push_back(str_arg);
    }
 
    for (int i = 0; i < args.size(); i++){ 
        if (args[i] == "-c"){
            arg_c = 1;
            idx_c = i;
        }
        else if (args[i] == "-a"){
            arg_a = 1;
            idx_a = i;
        }
        else if (args[i] == "-b"){
            arg_b = 1;
            idx_b = i;
        }
        else if (args[i] == "-lru"){
            arg_lru = 1;
            idx_lru_or_random = i;
        }
        else if (args[i] == "-random"){
            arg_random = 1;
            idx_lru_or_random = i;
        }
    }

    for (size_t i = 0; i < args.size(); i++){ 
        if (i!=(idx_c) && i!=(idx_c+1) && i!=(idx_a) && i!=(idx_a+1) && i!=(idx_b) && i!=(idx_b+1) && i!=(idx_lru_or_random)) {
            filename = args[i];
            idx_filename = i;
            break;
        }
    }

   
    l2_way = stoi(args[idx_a+1]);
    l2_capa = stoi(args[idx_c+1]); 
    block_size = stoi(args[idx_b+1]);

    if (l2_way <= 2){
        l1_way = l2_way;
    } 
    else{
        l1_way = l2_way / 4;
    }
    l1_capa = l2_capa / 4;


    // init L1, L2 cache
    int l2_block_num = (l2_capa << 10) / block_size;
    int l2_set_num = (l2_block_num) / l2_way; 
    int l2_index_bit = log2(l2_set_num);

    int l1_block_num = (l1_capa << 10) / block_size;
    int l1_set_num = (l1_block_num) / l1_way;
    int l1_index_bit = log2(l1_set_num);
    
    unsigned long long l1_ref_cnt = 0;
    unsigned long long l2_ref_cnt = 0;
    

    vector<vector<Cache_Block>> L1_Cache;
    for (int i=0; i < l1_set_num; i++){
        vector<Cache_Block> temp_set;
        for (int j=0; j < l1_way; j++){
            Cache_Block temp_block={0,};
            temp_set.push_back(temp_block);
        }    
        L1_Cache.push_back(temp_set);
    }

    vector<vector<Cache_Block>> L2_Cache;
    for (int i=0; i < l2_set_num; i++){
        vector<Cache_Block> temp_set;
        for (int j=0; j < l2_way; j++){
            Cache_Block temp_block={0,};
            temp_set.push_back(temp_block);
        }    
        L2_Cache.push_back(temp_set);
    }

    // Cache Specification
    // printf("l1_way: %d, l1_capa: %dKB, block size: %d Bytes\n", l1_way, l1_capa, block_size);
    // printf("l2_way: %d, l2_capa: %dKB, block size: %d Bytes\n", l2_way, l2_capa, block_size);
    // printf("l1_block_num: %d, l1_set_num: %d, l1_index_bit: %d\n", l1_block_num, l1_set_num, l1_index_bit);
    // printf("l2_block_num: %d, l2_set_num: %d, l2_index_bit: %d\n", l2_block_num, l2_set_num, l2_index_bit);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    string read = "R";
    string write = "W";


    string input_name_wo_ext = filename.substr(0, filename.find('.'));
    string output_name = input_name_wo_ext + "_" + to_string(l2_capa) + "_" + to_string(l2_way)+ "_" + to_string(block_size) + ".out";
    FILE* input_file = fopen(argv[idx_filename+1], "r");
    char str[128];

    while (fgets(str, 128, input_file) != NULL){
        string line(str);
        vector<string> words = split_line(line, ' ');
        string mode = words[0];
        unsigned long long addr = str2ull(words[1]);
        unsigned long long byte_addr = addr >> 3; 
        int offset = log2(block_size);
        unsigned long long block_addr = addr >> offset;
        unsigned long long l1_tag = block_addr >> l1_index_bit;
        unsigned long long l2_tag = block_addr >> l2_index_bit;

        int l1_index = block_addr % l1_set_num;
        int l2_index = block_addr % l2_set_num;

        int l1_hit = 0;
        int l2_hit = 0;

        total_access += 1;
        if (mode==write){
            write_access += 1;
        }
        else{
            read_access += 1;
        }

        // L1 hit ~
        for (int j=0; j < l1_way; j++){
            if ((L1_Cache[l1_index][j].valid_bit==1) && (L1_Cache[l1_index][j].tag == l1_tag)){
                l1_hit = 1;
                if (mode==write){
                    L1_Cache[l1_index][j].dirty_bit = 1;
                }
                L1_Cache[l1_index][j].ref_cnt = l1_ref_cnt;
                l1_ref_cnt += 1;
                break;
            }
        }
        // ~ L1 hit

        // if (l1_hit==1){
        //     cout << "Total Access: " << total_access << " L1 Hit" << " mode: " << mode << endl;            
        // }

        // L1 miss ~ 
        if (l1_hit==0){

            if (mode==read){
                l1_read_misses += 1;
            }
            else{
                l1_write_misses += 1;
            }

            // L2 hit ~
            for (int j=0; j < l2_way; j++){
                if ((L2_Cache[l2_index][j].valid_bit==1) && (L2_Cache[l2_index][j].tag == l2_tag)){
                    l2_hit = 1;
                    if (mode==write){
                        L2_Cache[l2_index][j].dirty_bit = 1;
                    }
                    L2_Cache[l2_index][j].ref_cnt = l2_ref_cnt;
                    l2_ref_cnt += 1;
                    break;
                }
            }

            if (l2_hit==1){
                // non-valid victim (L1)
                int find_victim = 0;
                for (int j=0; j < l1_way; j++){
                    if (L1_Cache[l1_index][j].valid_bit==0){ 
                        find_victim = 1;
                        // Caching
                        L1_Cache[l1_index][j].valid_bit = 1;
                        if (mode==write){
                            L1_Cache[l1_index][j].dirty_bit = 1;
                        }
                        else{
                            L1_Cache[l1_index][j].dirty_bit = 0;
                        }                    
                        L1_Cache[l1_index][j].ref_cnt = l1_ref_cnt;
                        L1_Cache[l1_index][j].tag = l1_tag;
                        L1_Cache[l1_index][j].block_addr = block_addr;
                        l1_ref_cnt += 1;
                        break;
                    }
                }

                // valid victim (L1)
                if (find_victim==0){ 

                    int victim_idx = 0; 
                    if (arg_lru==1){ // replacement policy : least recently used
                        unsigned long long temp_min = l1_ref_cnt;
                        for (int j=0; j < l1_way; j++){
                            if (L1_Cache[l1_index][j].ref_cnt < temp_min){
                                victim_idx = j;
                                temp_min = L1_Cache[l1_index][j].ref_cnt;
                            }
                        }
                    }
                    else if (arg_random==1){ // replacement policy : random
                        victim_idx = make_random_int(l1_way);
                    }


                    if (L1_Cache[l1_index][victim_idx].dirty_bit == 0){
                        l1_clean_eviction += 1;
                    }
                    else{
                        l1_dirty_eviction += 1;
                    }

                    // Caching
                    L1_Cache[l1_index][victim_idx].valid_bit = 1;
                    if (mode==write){ 
                        L1_Cache[l1_index][victim_idx].dirty_bit = 1;
                    }
                    else{
                        L1_Cache[l1_index][victim_idx].dirty_bit = 0;
                    }                    
                    L1_Cache[l1_index][victim_idx].ref_cnt = l1_ref_cnt;
                    L1_Cache[l1_index][victim_idx].tag = l1_tag;
                    L1_Cache[l1_index][victim_idx].block_addr = block_addr;
                    l1_ref_cnt += 1;
                }
                // ~ L2 hit

            }
            // L1 miss & L2 miss ~

            if (l2_hit==0){
                if (mode==write){
                    l2_write_misses += 1;
                }
                else{
                    l2_read_misses += 1;
                }

                // non-valid victim (L2)
                int find_l2_victim = 0;
                for (int j=0; j < l2_way; j++){
                    if (L2_Cache[l2_index][j].valid_bit==0){ 
                        find_l2_victim = 1;
                        L2_Cache[l2_index][j].valid_bit = 1;
                        if (mode==write){
                            L2_Cache[l2_index][j].dirty_bit = 1;
                        }
                        else{
                            L2_Cache[l2_index][j].dirty_bit = 0;
                        }                    
                        L2_Cache[l2_index][j].ref_cnt = l2_ref_cnt;
                        L2_Cache[l2_index][j].tag = l2_tag;
                        L2_Cache[l2_index][j].block_addr = block_addr;
                        l2_ref_cnt += 1;
                        break;
                    }
                }

                // valid victim (L2)
                int find_same_victim_and_evict = 0;
                if (find_l2_victim==0){ 
                    int l2_victim_idx = 0; 
                    if (arg_lru==1){ // replacement policy : least recently used
                        unsigned long long temp_l2_min = l2_ref_cnt;
                        for (int j=0; j < l2_way; j++){
                            if (L2_Cache[l2_index][j].ref_cnt < temp_l2_min){
                                l2_victim_idx = j;
                                temp_l2_min = L2_Cache[l2_index][j].ref_cnt;
                            }
                        }
                    }
                    else if (arg_random==1){ // replacement policy : random
                        l2_victim_idx = make_random_int(l2_way);
                    }

                    unsigned long long target_addr = L2_Cache[l2_index][l2_victim_idx].block_addr;
                    for (int j=0; j < l1_set_num; j++){
                        for (int k=0; k < l1_way; k++){
                            if (L1_Cache[j][k].block_addr == target_addr){
                                find_same_victim_and_evict = 1;

                                // Evict counting
                                if (L1_Cache[j][k].dirty_bit == 1){
                                    L2_Cache[l2_index][l2_victim_idx].dirty_bit = 1;
                                    l1_dirty_eviction += 1;
                                }
                                else{
                                    l1_clean_eviction += 1;    
                                }
                                    
                                // Caching
                                L1_Cache[j][k].valid_bit = 1;
                                if (mode==write){
                                    L1_Cache[j][k].dirty_bit = 1;
                                }
                                else{
                                    L1_Cache[j][k].dirty_bit = 0;
                                }                    
                                L1_Cache[j][k].ref_cnt = l1_ref_cnt;
                                L1_Cache[j][k].tag = l1_tag;
                                L1_Cache[j][k].block_addr = block_addr;
                                l1_ref_cnt += 1;
                                break;
                            }
                        }
                        if (find_same_victim_and_evict==1){
                            break;
                        }
                    }

                    if (L2_Cache[l2_index][l2_victim_idx].dirty_bit == 0){
                        l2_clean_eviction += 1;
                    }
                    else{
                        l2_dirty_eviction += 1;
                    }

                    L2_Cache[l2_index][l2_victim_idx].valid_bit = 1;
                    if (mode==write){
                        L2_Cache[l2_index][l2_victim_idx].dirty_bit = 1;
                    }
                    else{
                        L2_Cache[l2_index][l2_victim_idx].dirty_bit = 0;
                    }                    
                    L2_Cache[l2_index][l2_victim_idx].ref_cnt = l2_ref_cnt;
                    L2_Cache[l2_index][l2_victim_idx].tag = l2_tag;
                    L2_Cache[l2_index][l2_victim_idx].block_addr = block_addr;
                    l2_ref_cnt += 1;
                }

                if (find_same_victim_and_evict==0){
                    // non-valid victim
                    int find_l1_victim = 0;
                    for (int j=0; j < l1_way; j++){
                        if (L1_Cache[l1_index][j].valid_bit==0){
                            find_l1_victim = 1;

                            L1_Cache[l1_index][j].valid_bit = 1;
                            if (mode==write){
                                L1_Cache[l1_index][j].dirty_bit = 1;
                            }
                            else{
                                L1_Cache[l1_index][j].dirty_bit = 0;
                            }                    
                            L1_Cache[l1_index][j].ref_cnt = l1_ref_cnt;
                            L1_Cache[l1_index][j].tag = l1_tag;
                            L1_Cache[l1_index][j].block_addr = block_addr;
                            l1_ref_cnt += 1;
                            break;
                        }
                    }

                    // valid victim
                    if (find_l1_victim==0){ 
                        int l1_victim_idx = 0; 
                        if (arg_lru==1){ // replacement policy : least recently used
                            unsigned long long temp_l1_min = l1_ref_cnt;
                            for (int j=0; j < l1_way; j++){
                                if (L1_Cache[l1_index][j].ref_cnt < temp_l1_min){
                                    l1_victim_idx = j;
                                    temp_l1_min = L1_Cache[l1_index][j].ref_cnt;
                                }
                            }
                        }
                        else if (arg_random==1){ // replacement policy : random
                            l1_victim_idx = make_random_int(l1_way);
                        }

                        if (L1_Cache[l1_index][l1_victim_idx].dirty_bit == 0){
                            l1_clean_eviction += 1;
                        }
                        else{
                            l1_dirty_eviction += 1;
                        }

                        // Caching
                        L1_Cache[l1_index][l1_victim_idx].valid_bit = 1;
                        if (mode==write){
                            L1_Cache[l1_index][l1_victim_idx].dirty_bit = 1;
                        }
                        else{
                            L1_Cache[l1_index][l1_victim_idx].dirty_bit = 0;
                        }                    
                        L1_Cache[l1_index][l1_victim_idx].ref_cnt = l1_ref_cnt;
                        L1_Cache[l1_index][l1_victim_idx].tag = l1_tag;
                        L1_Cache[l1_index][l1_victim_idx].block_addr = block_addr;
                        l1_ref_cnt += 1;
                    }
                }
            }
            // L1 miss & L2 miss ~
        }
        // ~ L1 miss

    }

    fclose(input_file);


    // printf("L1 Cache\n");
    // for(int i=0; i<L1_Cache.size(); i++){
    //     printf("L1 Set: %d\n", i);
    //     for(int j=0; j<L1_Cache[0].size(); j++){
    //         printf("block addr: %llu\n", L1_Cache[i][j].block_addr);
    //     }
    // }
    // printf("L2 Cache\n");
    // for(int i=0; i<L2_Cache.size(); i++){
    //     printf("L2 Set: %d\n", i);
    //     for(int j=0; j<L2_Cache[0].size(); j++){
    //         printf("block addr: %llu\n", L2_Cache[i][j].block_addr);
    //     }
    // }

    FILE* asm_file_o = fopen(output_name.c_str(), "w");
    fprintf(asm_file_o, "-- General Stats --\n");
    fprintf(asm_file_o, "L1 Capacity: %d\n", l1_capa);
    fprintf(asm_file_o, "L1 way: %d\n", l1_way);
    fprintf(asm_file_o, "L2 Capacity: %d\n", l2_capa);
    fprintf(asm_file_o, "L2 way: %d\n", l2_way);
    fprintf(asm_file_o, "Block Size: %d\n", block_size);
    fprintf(asm_file_o, "Total accesses: %d\n", total_access);
    fprintf(asm_file_o, "Read accesses: %d\n", read_access);
    fprintf(asm_file_o, "Write accesses: %d\n", write_access);
    fprintf(asm_file_o, "L1 Read misses: %d\n", l1_read_misses);
    fprintf(asm_file_o, "L2 Read misses: %d\n", l2_read_misses);
    fprintf(asm_file_o, "L1 Write misses: %d\n", l1_write_misses);
    fprintf(asm_file_o, "L2 Write misses: %d\n", l2_write_misses);

    double l1_read_miss_rate = double(l1_read_misses) / double(read_access) * 100;
    double l2_read_miss_rate = double(l2_read_misses) / double(l1_read_misses) * 100;
    double l1_write_miss_rate = double(l1_write_misses) / double(write_access) * 100;
    double l2_write_miss_rate = double(l2_write_misses) / double(l1_write_misses) * 100;
    
    fprintf(asm_file_o, "L1 Read miss rate: %f%%\n", l1_read_miss_rate);
    fprintf(asm_file_o, "L2 Read miss rate: %f%%\n", l2_read_miss_rate);
    fprintf(asm_file_o, "L1 Write miss rate: %f%%\n", l1_write_miss_rate);
    fprintf(asm_file_o, "L2 Write miss rate: %f%%\n", l2_write_miss_rate);
    fprintf(asm_file_o, "L1 Clean eviction: %d \n", l1_clean_eviction);
    fprintf(asm_file_o, "L2 Clean eviction: %d \n", l2_clean_eviction);
    fprintf(asm_file_o, "L1 dirty eviction: %d \n", l1_dirty_eviction);
    fprintf(asm_file_o, "L2 dirty eviction: %d", l2_dirty_eviction);
    fclose(asm_file_o);
    return 0;
}