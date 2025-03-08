/*
2024 Spring, Computer Architecture 
Project 2: Simple MIPS emulaotr

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

using namespace std;


struct Register{
    string name;
    unsigned int value;
};


void print_registers(vector<Register> registers, Register pc){
    printf("Current register values:\n");
    printf("------------------------------------\n");
    printf("PC: 0x%x\n", pc.value);
    printf("Registers:\n");
    for (size_t i=0; i < registers.size(); i++){
        char register_name[5];
        strcpy(register_name, registers[i].name.c_str());
        printf("%s: 0x%x\n", register_name, registers[i].value);
    }
    printf("\n");
}

void print_memory(map<unsigned int, unsigned int> memory, int start, int end){
    printf("Memory content [0x%x..0x%x]:\n", start, end);
    printf("------------------------------------\n");
    unsigned int front_val=0;
    unsigned int back_val=0;
    unsigned int start_mod = start % 4;
    unsigned int front_addr;
    unsigned int back_addr;
    unsigned int val=0;
    unsigned int range_mod = (end-start) % 4;

    if (range_mod == 0 && ((end-start)!=0)){
        for (unsigned int i=start; i <= end; i+=4){
            front_addr = i - start_mod;
            back_addr = front_addr + 4;
            
            if (memory.find(front_addr) != memory.end()) {
                front_val = memory[front_addr];
            }
            else {
                front_val = 0;
            }
            if (memory.find(back_addr) != memory.end()) {
                back_val = memory[back_addr];
            }
            else {
                back_val = 0;
            }
            if (start_mod==0){
                val = front_val;
            }
            else{
                val = ((front_val << (start_mod*8)) + (back_val >> (4-start_mod)*8));
            }
            printf("0x%x: 0x%x\n", i, val);
        }
    }
    else{
        for (unsigned int i=start; i <= end; i+=4){
            front_addr = i - start_mod;
            back_addr = front_addr + 4;
            
            if (memory.find(front_addr) != memory.end()) {
                front_val = memory[front_addr];
            }
            else {
                front_val = 0;
            }
            if (memory.find(back_addr) != memory.end()) {
                back_val = memory[back_addr];
            }
            else {
                back_val = 0;
            }
    
            if (start_mod==0){
                val = front_val;
            }
            else{
                val = ((front_val << (start_mod*8)) + (back_val >> (4-start_mod)*8));
            }
            if (i+4 > end){
                val = (val >> ((3-range_mod)*8));
            }            
            printf("0x%x: 0x%x\n", i, val);
        }
    }
    printf("\n");
}


unsigned int str2int(string num_str){ // Make string to int. Support both decimal, hexadecimal.
    unsigned int num_int = 0;
    if (num_str.rfind("0x", 0) == 0){ // for hex
        int str_length = num_str.length();
        char num_char[str_length+1];
        strcpy(num_char, num_str.c_str());
        num_int = (int) strtol(num_char, NULL, 0);
    }
    else{ // for dec
        num_int = stoi(num_str);
    }
    return (unsigned int) num_int;
}


int main(int argc, const char* argv[]){
    int program_cnt = 0x00400000; // init pc
    int general_ptr = 0x10000000; // init gp
    int cnt_text = 0; // counting number of instruction
    int cnt_data = 0; // counting number of data
    int inst_idx = 0; // check instrunction idx
    bool arg_d = 0; 
    bool arg_m = 0;
    bool arg_n = 0;

    int val_m_start;
    int val_m_end;
    int val_n;

    int idx_m = -999;
    int idx_n = -999;
    int idx_d = -999;
    int idx_filename = -999;

    string filename="";

    // arg parsing
    vector<string> args;
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        string str_arg(arg);
        args.push_back(str_arg);
    }
    
    for (size_t i = 0; i < args.size(); i++){ 
        if (args[i] == "-m"){
            arg_m = 1;
            idx_m = i;
        }
        else if (args[i] == "-d"){
            arg_d = 1;
            idx_d = i;
        }
        else if (args[i] == "-n"){
            arg_n = 1;
            idx_n = i;
        }
    }

    for (size_t i = 0; i < args.size(); i++){ 
        if (i!=(idx_m) && i!=(idx_m+1) && i!=(idx_n) && i!=(idx_n+1) && i!=(idx_d)) {
            filename = args[i];
            idx_filename = i;
            break;
        }
    }

    if (arg_m){
        string memory_start_to_end = args[idx_m+1];
        string memory_start = memory_start_to_end.substr(0, memory_start_to_end.find(":"));
        string memory_end = memory_start_to_end.substr(memory_start_to_end.find(":")+1, memory_start_to_end.length());
        val_m_start = str2int(memory_start);
        val_m_end = str2int(memory_end);
    }

    if (arg_n){
        string str_val_n = args[idx_n+1];
        val_n = str2int(str_val_n);
    }

    // init PC and registers with init value(= 0)
    Register pc;
    pc.name = "PC";
    pc.value = 0x400000;

    vector<Register> registers; 
    for (size_t i=0; i < 32; i++){
        Register temp;
        string name="R"+to_string(i);
        temp.name = name;
        temp.value = 0;
        registers.push_back(temp);
    }

    FILE* binary_file = fopen(argv[idx_filename+1], "r");
    char str[128];
    vector<string> lines;

    // save input file contents to vector<string> format
    while (fgets(str, 128, binary_file) != NULL){
        string line(str);
        lines.push_back(line);
    }

    // init memory
    map<unsigned int, unsigned int> memory;

    int text_sec_size = str2int(lines[0]);
    int data_sec_size = str2int(lines[1]);
    int num_text_line = text_sec_size / 4;
    int num_data_line = data_sec_size / 4;
    unsigned int raw;

    for (size_t i = 2; i < lines.size()-num_data_line; i++){
        raw = str2int(lines[i]);
        memory.insert({program_cnt, raw});
        program_cnt += 4;
    }
    for (size_t i = 2+num_text_line; i < lines.size(); i++){
        raw = str2int(lines[i]);
        memory.insert({general_ptr, raw});
        general_ptr += 4;
    }

    // start emulation
    unsigned int opcode;
    unsigned int rs;
    unsigned int rt;
    unsigned int rd;
    unsigned int shamt;
    unsigned int funct;
    int imm;
    unsigned int addr;

    int cnt = 0;

    while(pc.value < 0x400000 + text_sec_size){
        if (arg_n && cnt==val_n){
            break;
        }

        raw = memory[pc.value];
        opcode = raw >> 26;
        
        if (opcode==0){ // R-format
            rs = (raw << 6) >> 27;
            rt = (raw << 11) >> 27;
            rd = (raw << 16) >> 27;
            shamt = (raw << 21) >> 27;
            funct = (raw << 26) >> 26;

            if (funct==0x21){ // ADDU
                registers[rd].value = registers[rs].value + registers[rt].value;
                pc.value += 4;
            }
            else if (funct==0x24){ // AND
                registers[rd].value = registers[rs].value & registers[rt].value;
                pc.value += 4;
            }
            else if (funct==8){ // JR
                pc.value = registers[rs].value;
            }
            else if (funct==0x27){ // NOR
                registers[rd].value = ~(registers[rs].value | registers[rt].value);                    
                pc.value += 4;
            }
            else if (funct==0x25){ // OR
                registers[rd].value = registers[rs].value | registers[rt].value;
                pc.value += 4;
            }
            else if (funct==0x2b){ // SLTU
                if (registers[rs].value < registers[rt].value){
                    registers[rd].value = 1;
                }
                else{
                    registers[rd].value = 0;
                }
                pc.value += 4;
            }
            else if (funct==0){ // SLL
                registers[rd].value = (registers[rt].value << shamt);
                pc.value += 4;
            }
            else if (funct==2){ // SRL
                registers[rd].value = (registers[rt].value >> shamt);
                pc.value += 4;
            }
            else if (funct==0x23){ // SUBU
                registers[rd].value = registers[rs].value - registers[rt].value;
                pc.value += 4;
            }
        }

        else if (opcode==2 || opcode==3){ // J-format
            int target;
            addr = (raw << 6) >> 6;
            target = addr * 4;

            if (opcode==2){ // J
                pc.value = target;
            }
            else if (opcode==3){ // JAL
                registers[31].value = pc.value + 4;
                pc.value = target;
            }
        }
        
        else{ // I-format
            rs = (raw << 6) >> 27;
            rt = (raw << 11) >> 27;
            imm = ((int)(raw << 16)) >> 16; // for signed-extension

            if (opcode == 9){ // ADDIU
                registers[rt].value = registers[rs].value + imm;
                pc.value += 4;
            }
            else if (opcode == 0xc){ // ANDI
                unsigned int zero_imm;
                zero_imm = (((unsigned int) imm) << 16) >> 16;
                registers[rt].value = registers[rs].value & zero_imm;
                pc.value += 4;
            }
            else if (opcode == 4){ // BEQ
                if (registers[rs].value == registers[rt].value){
                    pc.value = (pc.value + 4) + 4 * imm; 
                }
                else{
                    pc.value += 4;
                }
            }
            else if (opcode == 5){ // BNE
                if (registers[rs].value != registers[rt].value){
                    pc.value = (pc.value + 4) + 4 * imm; 
                }
                else{
                    pc.value += 4;
                }
            }
            else if (opcode == 0xf){ // LUI
                registers[rt].value = imm<<16;
                pc.value += 4;
            }
            else if (opcode == 0x23){ // LW
                int load_addr = registers[rs].value + imm;
                int load_val;
                if (memory.find(load_addr) != memory.end()) {
                    load_val = memory[load_addr];
                }
                else {
                    load_val = 0;
                }
                registers[rt].value = load_val;
                pc.value += 4;
            }
            else if (opcode == 0x20){ // LB
                int load_addr = registers[rs].value + imm;
                int mod = load_addr % 4;
                int load_val;
                if (memory.find(load_addr-mod) != memory.end()) {
                    load_val = memory[load_addr-mod];
                    if (mod == 0){
                        load_val = (load_val >> 24);
                    }
                    else if (mod == 1){
                        load_val = ((load_val << 8) >> 24);                            
                    }
                    else if (mod == 2){
                        load_val = ((load_val << 16) >> 24);                                                        
                    }
                    else if (mod == 3){
                        load_val = ((load_val << 24) >> 24);                                                        
                    }
                }
                else {
                    load_val = 0;
                }
                registers[rt].value = load_val;
                pc.value += 4;
            }
            else if (opcode == 0xd){ // ORI
                unsigned int zero_imm;
                zero_imm = (((unsigned int) imm) << 16) >> 16;
                registers[rt].value = registers[rs].value | zero_imm;
                pc.value += 4;
            }
            else if (opcode == 0xb){ // SLTIU
                if (registers[rs].value < imm){
                    registers[rt].value = 1;
                }
                else{
                    registers[rt].value = 0;
                }
                pc.value += 4;
            }
            else if (opcode == 0x2b){ // SW
                int save_addr = registers[rs].value + imm;
                if (memory.find(save_addr) != memory.end()) {
                    memory[save_addr] = registers[rt].value;
                }
                else {
                    memory.insert({save_addr, registers[rt].value});
                }
                pc.value += 4;
            }
            else if (opcode == 0x28){ // SB
                int save_addr = registers[rs].value + imm;
                int mod = save_addr % 4;
                int mask;
                int target;
                target = (registers[rt].value << 24) >>24; // 1-byte data
                if (mod == 0){
                    mask = 0x00FFFFFF;
                }
                else if (mod == 1){
                    mask = 0xFF00FFFF;
                }
                else if (mod == 2){
                    mask = 0xFFFF00FF;
                }
                else if (mod == 3){
                    mask = 0xFFFFFF00;
                }
                if (memory.find(save_addr-mod) != memory.end()){
                    int temp = memory[save_addr-mod];
                    temp = temp & mask;
                    temp = temp + (target << ((3-mod)*8));
                    memory[save_addr-mod] = temp;
                }
                else {
                    memory.insert({save_addr-mod, (target << ((3-mod)*8))});
                }
                pc.value += 4;
            }
        }

        if (arg_d){
            print_registers(registers, pc);
            if (arg_m){
                print_memory(memory, val_m_start, val_m_end);
            }
        }

        cnt += 1;

    }

    if (!arg_d){
        print_registers(registers, pc);
        if (arg_m){
            print_memory(memory, val_m_start, val_m_end);
        }
    }

    return 0;
}