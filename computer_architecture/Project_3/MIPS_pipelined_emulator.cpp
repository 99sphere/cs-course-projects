/*
2024 Spring, Computer Architecture 
Project 3: Pipeilned MIPS emulaotr

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
#include "global.h"

using namespace std;

// set global values
Register pc;
vector<Register> registers; 
State_Register prev_state;
State_Register current_state;
map<unsigned int, unsigned int> memory;

bool arg_d = 0; 
bool arg_m = 0;
bool arg_n = 0;
bool arg_p = 0;
bool arg_atp = 0;
unsigned int EOI;

unsigned int pc_IF;
unsigned int pc_ID;
unsigned int pc_EX;
unsigned int pc_MEM;
unsigned int pc_WB;
int inst_cnt = 0;

unsigned int pipline_empty;

void print_registers(){
    printf("Current register values:\n");

    unsigned int pc_out;

    pc_out = (pc.value > EOI) ? EOI : pc.value;

    printf("PC: 0x%x\n", pc_out);
    printf("Registers:\n");
    for (size_t i=0; i < registers.size(); i++){
        char register_name[5];
        strcpy(register_name, registers[i].name.c_str());
        printf("%s: 0x%x\n", register_name, registers[i].value);
    }
    printf("\n");
}


void print_memory(int start, int end){
    printf("Memory content [0x%x..0x%x]:\n", start, end);
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
    else if (start==end){
        if (memory.find(start) != memory.end()) {
            val = memory[start];
        }
        else {
            val = 0;
        }
        printf("0x%x: 0x%x\n", start, val);
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


void pipeline_check(){
    pipline_empty = 0;
    unsigned int if_empty = 0;
    unsigned int id_empty = 0;
    unsigned int ex_empty = 0;
    unsigned int mem_empty = 0;
    unsigned int wb_empty = 0;

    if (pc_IF != 0 && (memory[pc_IF] != 0)){
    }
    else {
        if_empty = 1;
    }
    if (pc_ID != 0 && (memory[pc_ID] != 0)){
    }
    else {
        id_empty = 1;
    }
    if (pc_EX != 0 && (memory[pc_EX] != 0)){
    }
    else {
        ex_empty = 1;
    }
    if (pc_MEM != 0 && (memory[pc_MEM] != 0)){
    }
    else {
        mem_empty = 1;
    }
    if (pc_WB != 0 && (memory[pc_WB] != 0)){
    }
    else {
        wb_empty = 1;
    }
    if (if_empty && id_empty && ex_empty && mem_empty && wb_empty){
        pipline_empty = 1;
    }
}


void print_pipelined_pc(){
    printf("Current pipeline PC state:\n");
    printf("{");

    if (pc_IF != 0 && (memory[pc_IF] != 0)){
        printf("0x%x|", pc_IF);
    }
    else {
        printf("|");
    }
    if (pc_ID != 0 && (memory[pc_ID] != 0)){
        printf("0x%x|", pc_ID);
    }
    else {
        printf("|");
    }
    if (pc_EX != 0 && (memory[pc_EX] != 0)){
        printf("0x%x|", pc_EX);
    }
    else {
        printf("|");
    }
    if (pc_MEM != 0 && (memory[pc_MEM] != 0)){
        printf("0x%x|", pc_MEM);
    }
    else {
        printf("|");
    }

    if (pc_WB != 0 && (memory[pc_WB] != 0)){
        printf("0x%x}\n\n", pc_WB);
    }
    else {
        printf("}\n\n");
    }
}


void print_id_ex_reg(ID_EX_Register *p){
    printf("------------------------------------\n");
    cout << "[ID_EX] next_pc: " << p->next_pc << endl;
    cout << "[ID_EX] rs: " << p->rs << endl;
    cout << "[ID_EX] rt: " << p->rt << endl;
    cout << "[ID_EX] rd: " << p->rd << endl;
    cout << "[ID_EX] imm: " << p->imm << endl;
    cout << "[ID_EX] opcode: " << p->opcode << endl;
    cout << "[ID_EX] is_jump: " << p->is_jump << endl;
    cout << "[ID_EX] jump_target: " << p->jump_target << endl;
    cout << "[ID_EX] is_load_use: " << p->is_load_use << endl;
    printf("[ID_EX] current_pc: {%x}\n", p->current_pc);
}


void print_ex_mem_reg(EX_MEM_Register *p){
    printf("------------------------------------\n");
    printf("[EX_MEM] alu_out: {0x%x}\n", p->alu_out);
    cout << "[EX_MEM] br_target: " << p->br_target << endl;
    cout << "[EX_MEM] write_to_mem: " << p->write_to_mem << endl;
    cout << "[EX_MEM] write_to_mem_val: " << p->write_to_mem_val << endl;
    cout << "[EX_MEM] read_from_mem: " << p->read_from_mem << endl;
    cout << "[EX_MEM] write_to_reg: " << p->write_to_reg << endl;
    cout << "[EX_MEM] reg_num: " << p->reg_num << endl;    
    cout << "[EX_MEM] is_branch: " << p->is_branch << endl;
    cout << "[EX_MEM] predict_correct: " << p->predict_correct << endl;    

    printf("[EX_MEM] current_pc: {%x}\n", p->current_pc);
}


void print_mem_wb_reg(MEM_WB_Register *p){
    printf("------------------------------------\n");
    printf("[MEM_WB] alu_out: {0x%x}\n", p->alu_out);
    printf("[MEM_WB] mem_out: {0x%x}\n", p->mem_out);
    cout << "[MEM_WB] write_to_reg: " << p->write_to_reg << endl;
    cout << "[MEM_WB] reg_num: " << p->reg_num << endl;
    cout << "[MEM_WB] select_alu: " << p->select_alu << endl;
    printf("[MEM_WB] current_pc: {%x}\n", p->current_pc);
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


void run_IF_stage(){
    unsigned int instr;
    unsigned int next_pc;
    
    instr = memory[pc.value];
    pc_IF = pc.value;
    next_pc = pc.value + 4;

    if (instr == 0){
        next_pc = 0;
    }

    pc_IF = pc.value;
    current_state.if_id_reg.current_pc = pc.value;
    current_state.if_id_reg.instr = instr;
    current_state.if_id_reg.next_pc = next_pc;
}


void run_ID_stage(){
    unsigned int instr = prev_state.if_id_reg.instr;
    unsigned int next_pc = prev_state.if_id_reg.next_pc;
    unsigned int opcode_id_ex = prev_state.id_ex_reg.opcode;
    unsigned int rs_id_ex = prev_state.id_ex_reg.rs;
    unsigned int rt_id_ex = prev_state.id_ex_reg.rt;
    unsigned int rd_id_ex = prev_state.id_ex_reg.rd;
    unsigned int imm_id_ex = prev_state.id_ex_reg.imm;
    unsigned int funct_id_ex = (imm_id_ex << 26) >> 26;
    unsigned int opcode = instr >> 26;
    unsigned int rs = (instr << 6) >> 27;
    unsigned int rt = (instr << 11) >> 27;
    unsigned int rd = (instr << 16) >> 27;
    unsigned int imm = ((int)(instr << 16)) >> 16; // for signed-extension

    // LOAD-USE HAZARD DETECTION UNIT    
    if ((opcode_id_ex== 0x23 || opcode_id_ex==0x20) && ((rs == rt_id_ex) or (rt == rt_id_ex))){
        current_state.id_ex_reg.is_load_use = 1;

        if (opcode==0x2b || opcode==0x28){
            current_state.id_ex_reg.is_load_use = 0; // It can resolving by MEM/WB to MEM forwarding. 
        }
    }
    
    // if jump, jr, jal, make stall (IF_FLUSH)
    unsigned int funct = (instr << 26) >> 26;
    unsigned int addr;
    unsigned int jump_target;
    
    if (opcode==2 || opcode==3){ // j, jal
        current_state.id_ex_reg.is_jump = 1;
        addr = (instr << 6) >> 6;
        jump_target = addr * 4;
    }
        
    else if (opcode==0 && funct ==8){ // jr
        current_state.id_ex_reg.is_jump = 1;
        jump_target = registers[rs].value;
    }

    if (arg_atp && (opcode==4 or opcode==5)){ // always taken
        current_state.id_ex_reg.is_jump = 1;
        jump_target = next_pc + 4 * imm;
    }

    current_state.id_ex_reg.next_pc = next_pc;
    current_state.id_ex_reg.opcode = opcode;
    current_state.id_ex_reg.rs = rs;
    current_state.id_ex_reg.rt = rt;
    current_state.id_ex_reg.rd = rd;
    current_state.id_ex_reg.imm = imm;
    current_state.id_ex_reg.current_pc = prev_state.if_id_reg.current_pc;
    current_state.id_ex_reg.jump_target = jump_target;
    pc_ID = prev_state.if_id_reg.current_pc;
}

void run_EX_stage(){
    unsigned int next_pc = prev_state.id_ex_reg.next_pc;
    unsigned int opcode = prev_state.id_ex_reg.opcode;
    unsigned int rs = prev_state.id_ex_reg.rs;
    unsigned int rt = prev_state.id_ex_reg.rt;
    unsigned int rd = prev_state.id_ex_reg.rd;
    unsigned int imm = prev_state.id_ex_reg.imm;
    unsigned int reg_write_ex_mem = prev_state.ex_mem_reg.write_to_reg;
    unsigned int rd_ex_mem = prev_state.ex_mem_reg.reg_num;
    unsigned int reg_write_mem_wb = prev_state.mem_wb_reg.write_to_reg;
    unsigned int rd_mem_wb = prev_state.mem_wb_reg.reg_num;
    unsigned int a = registers[rs].value;
    unsigned int b = registers[rt].value;

    // Forwarding
    // EX Forward Unit
    if ((reg_write_ex_mem == 1) && (rd_ex_mem != 0) && (rd_ex_mem == rs)){
        a = prev_state.ex_mem_reg.alu_out;
    }
    if ((reg_write_ex_mem == 1) && (rd_ex_mem != 0) && (rd_ex_mem == rt)){
        b = prev_state.ex_mem_reg.alu_out;
    }

    // MEM Forward Unit
    if ((reg_write_mem_wb == 1) && (rd_mem_wb != 0) && (rd_ex_mem != rs) && (rd_mem_wb == rs)){
        if (prev_state.mem_wb_reg.select_alu)
            a = prev_state.mem_wb_reg.alu_out;
        else
            a = prev_state.mem_wb_reg.mem_out;
    }
    if ((reg_write_mem_wb == 1) && (rd_mem_wb != 0) && (rd_ex_mem != rt) && (rd_mem_wb == rt)){
        if (prev_state.mem_wb_reg.select_alu)
            b = prev_state.mem_wb_reg.alu_out;
        else
            b = prev_state.mem_wb_reg.mem_out;
    }   

    unsigned int alu_out = 0;
    unsigned int br_target = 0;
    unsigned int write_to_reg = 0; // for Forwarding Unit (bool)
    unsigned int read_from_mem = 0; // for MEM stage (bool)
    unsigned int write_to_mem = 0; // for MEM stage (bool)
    unsigned int write_to_mem_val; // for MEM stage (read from register)
    int reg_num = -1;
    unsigned int shamt;
    unsigned int funct;
    unsigned int is_branch = 0;
    unsigned int predict_correct = 0;

    if (opcode==0){ // R-format
        shamt = (imm << 21) >> 27;
        funct = (imm << 26) >> 26;
        reg_num = rd;

        if (funct==0x21){ // ADDU
            write_to_reg = 1;
            alu_out = a + b;
        }
        else if (funct==0x24){ // AND
            write_to_reg = 1;
            alu_out = a & b;
        }
        else if (funct==0x27){ // NOR
            write_to_reg = 1;
            alu_out = ~(a | b);                    
        }
        else if (funct==0x25){ // OR
            write_to_reg = 1;
            alu_out = a | b;
        }
        else if (funct==0x2b){ // SLTU
            write_to_reg = 1;
            if (a < b){
                alu_out = 1;
            }
            else{
                alu_out = 0;
            }
        }
        else if (funct==0){ // SLL
            write_to_reg = 1;
            alu_out = (b << shamt);
        }
        else if (funct==2){ // SRL
            write_to_reg = 1;
            alu_out = (b >> shamt);
        }
        else if (funct==0x23){ // SUBU
            write_to_reg = 1;
            alu_out = a - b;
        }
    }
    else if (opcode==3){ // JAL
        write_to_reg = 1;
        alu_out = next_pc;
        reg_num = 31;
    }
    else{ // I-format
        reg_num = rt;

        if (opcode == 9){ // ADDIU
            write_to_reg = 1;
            alu_out = a + imm;
        }
        else if (opcode == 0xc){ // ANDI
            write_to_reg = 1;
            unsigned int zero_imm;
            zero_imm = (((unsigned int) imm) << 16) >> 16;
            alu_out = a & zero_imm;
        }
        else if (opcode == 4){ // BEQ
            is_branch = 1;

            if (a == b){
                if (arg_atp){ // always taken
                    predict_correct = 1;
                }
                else{ // always not taken
                    predict_correct = 0;
                    br_target = next_pc + 4 * imm;
                }
            }
            else{
                if (arg_atp){ // always taken
                    predict_correct = 0;
                    br_target = next_pc;
                }
                else{ // always not taken
                    predict_correct = 1;
                }
            }
        }
        else if (opcode == 5){ // BNE
            is_branch = 1;
            br_target = next_pc + 4 * imm;

            if (a != b){
                if (arg_atp){ // always taken
                    predict_correct = 1;
                }
                else{ // always not taken
                    predict_correct = 0;
                    br_target = next_pc + 4 * imm;
                }
            }
            else{
                if (arg_atp){ // always taken
                    predict_correct = 0;
                    br_target = next_pc;
                }
                else{ // always not taken
                    predict_correct = 1;
                }
            }
        }
        else if (opcode == 0xf){ // LUI
            write_to_reg = 1;
            alu_out = imm<<16;
        }
        else if (opcode == 0x23){ // LW
            write_to_reg = 1;
            read_from_mem = 1;
            alu_out = a + imm;
        }
        else if (opcode == 0x20){ // LB
            write_to_reg = 1;
            read_from_mem = 2;
            alu_out = a + imm;
        }
        else if (opcode == 0xd){ // ORI
            write_to_reg = 1;
            unsigned int zero_imm;
            zero_imm = (((unsigned int) imm) << 16) >> 16;
            alu_out = a | zero_imm;
        }
        else if (opcode == 0xb){ // SLTIU
            write_to_reg = 1;
            if (a < imm){
                alu_out = 1;
            }
            else{
                alu_out = 0;
            }
        }
        else if (opcode == 0x2b){ // SW
            write_to_mem = 1;
            alu_out = a + imm;
            write_to_mem_val = b;
        }
        else if (opcode == 0x28){ // SB
            write_to_mem = 2;
            alu_out = a + imm;
            write_to_mem_val = (b << 24) >> 24;
        }
    }
    current_state.ex_mem_reg.alu_out = alu_out;
    current_state.ex_mem_reg.br_target = br_target;
    current_state.ex_mem_reg.write_to_mem = write_to_mem;
    current_state.ex_mem_reg.write_to_mem_val = write_to_mem_val;
    current_state.ex_mem_reg.read_from_mem = read_from_mem;
    current_state.ex_mem_reg.write_to_reg = write_to_reg;
    current_state.ex_mem_reg.reg_num = reg_num;
    current_state.ex_mem_reg.is_branch = is_branch;
    current_state.ex_mem_reg.predict_correct = predict_correct;
    current_state.ex_mem_reg.current_pc = prev_state.id_ex_reg.current_pc;
    pc_EX = prev_state.id_ex_reg.current_pc;
}

void run_MEM_stage(){
    unsigned int alu_out = prev_state.ex_mem_reg.alu_out;
    unsigned int br_target = prev_state.ex_mem_reg.br_target;
    unsigned int write_to_mem = prev_state.ex_mem_reg.write_to_mem; // for MEM stage
    unsigned int write_to_mem_val = prev_state.ex_mem_reg.write_to_mem_val; // for MEM stage
    unsigned int read_from_mem = prev_state.ex_mem_reg.read_from_mem; // for MEM stage
    unsigned int write_to_reg = prev_state.ex_mem_reg.write_to_reg; // for WB Stage and Forwarding Unit
    unsigned int reg_num = prev_state.ex_mem_reg.reg_num; // for WB Stage. Added -> registers[reg_num].value = alu_out
    unsigned int is_branch = current_state.ex_mem_reg.is_branch;
    unsigned int predict_correct = current_state.ex_mem_reg.predict_correct;

    // Forwarding (MEM/WB to MEM)
    if ((prev_state.mem_wb_reg.read_from_mem && write_to_mem) && reg_num==prev_state.mem_wb_reg.reg_num){
        write_to_mem_val = prev_state.mem_wb_reg.mem_out;
    }

    // interaction with memory
    int mem_out;

    if (write_to_mem == 1){ // sw
        if (memory.find(alu_out) != memory.end()) {
            memory[alu_out] = write_to_mem_val;
        }
        else {
            memory.insert({alu_out, write_to_mem_val});
        }
    }

    if (write_to_mem == 2){ // sb
        int mask;
        if ((alu_out % 4) == 0){
            mask = 0x00FFFFFF;
        }
        else if ((alu_out % 4) == 1){
            mask = 0xFF00FFFF;
        }
        else if ((alu_out % 4) == 2){
            mask = 0xFFFF00FF;
        }
        else if ((alu_out % 4) == 3){
            mask = 0xFFFFFF00;
        }

        if (memory.find(alu_out-(alu_out % 4)) != memory.end()){
            int temp = memory[alu_out-(alu_out % 4)];
            temp = temp & mask;
            temp = temp + (write_to_mem_val << ((3-(alu_out % 4))*8));
            memory[alu_out-(alu_out % 4)] = temp;
        }
        else {
            memory.insert({alu_out-(alu_out % 4), (write_to_mem_val << ((3-(alu_out % 4))*8))});
        }
    }

    if (read_from_mem==1){ // lw
        int load_val;
        if (memory.find(alu_out) != memory.end()) {
            mem_out = memory[alu_out];
        }
        else {
            mem_out = 0;
        }
    }

    if (read_from_mem==2){ // lb
        int mod = alu_out % 4;
        if (memory.find(alu_out-mod) != memory.end()) {
            mem_out = memory[alu_out-mod];
            if (mod == 0){
                mem_out = (mem_out >> 24);
            }
            else if (mod == 1){
                mem_out = ((mem_out << 8) >> 24);                            
            }
            else if (mod == 2){
                mem_out = ((mem_out << 16) >> 24);                                                        
            }
            else if (mod == 3){
                mem_out = ((mem_out << 24) >> 24);                                                        
            }
        }
        else {
            mem_out = 0;
        }
    }

    current_state.mem_wb_reg.alu_out = alu_out; // for Forwarding Unit
    current_state.mem_wb_reg.mem_out = mem_out; // memory에서 읽어온 값
    current_state.mem_wb_reg.write_to_reg = write_to_reg; // Added for forwarding
    current_state.mem_wb_reg.reg_num = reg_num; // for WB Stage. Added -> registers[reg_num].value = alu_out
    current_state.mem_wb_reg.select_alu = (read_from_mem || write_to_mem) ? 0 : 1;
    current_state.mem_wb_reg.current_pc = prev_state.ex_mem_reg.current_pc;
    current_state.mem_wb_reg.br_target = prev_state.ex_mem_reg.br_target;
    current_state.mem_wb_reg.is_branch = prev_state.ex_mem_reg.is_branch;
    current_state.mem_wb_reg.predict_correct = prev_state.ex_mem_reg.predict_correct;
    current_state.mem_wb_reg.read_from_mem = read_from_mem;
    current_state.mem_wb_reg.predict_correct = prev_state.ex_mem_reg.predict_correct;
}

void run_WB_stage(){
    unsigned int alu_out = prev_state.mem_wb_reg.alu_out; // for Forwarding Unit
    unsigned int mem_out = prev_state.mem_wb_reg.mem_out; // load from memory
    unsigned int write_to_reg = prev_state.mem_wb_reg.write_to_reg; // Added for forwarding
    int reg_num = prev_state.mem_wb_reg.reg_num; // for WB Stage. Added -> registers[reg_num].value = alu_out
    int select_alu = prev_state.mem_wb_reg.select_alu;

    if (write_to_reg){
        if (select_alu){
            registers[reg_num].value = alu_out;
        }
        else{
            registers[reg_num].value = mem_out;
        }
    }
    pc_WB = prev_state.mem_wb_reg.current_pc;
    if (pc_WB){
        inst_cnt += 1;
    }
}


void clear_prev_state_register(){
    State_Register zero_state = {0,};
    prev_state = zero_state;
}


void clear_current_state_register(){
    State_Register zero_state = {0,};
    current_state = zero_state;
}


int main(int argc, const char* argv[]){
    int program_cnt = 0x00400000; // init pc
    int general_ptr = 0x10000000; // init gp
    int cnt_text = 0; // counting number of instruction
    int cnt_data = 0; // counting number of data
    int inst_idx = 0; // check instrunction idx
    int val_m_start;
    int val_m_end;
    int val_n;
    int idx_m = -999;
    int idx_n = -999;
    int idx_d = -999;
    int idx_p = -999;
    int idx_atp_or_antp = -999;
    int idx_filename = -999;
    int end_flag = 0;
    string filename;

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
        else if (args[i] == "-p"){
            arg_p = 1;
            idx_p = i;
        }
        else if (args[i] == "-atp"){
            arg_atp = 1;
            idx_atp_or_antp = i;
        }
        else if (args[i] == "-antp"){
            arg_atp = 0;
            idx_atp_or_antp = i;
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
    
    for (size_t i = 0; i < args.size(); i++){ 
        if (i!=(idx_m) && i!=(idx_m+1) && i!=(idx_n) && i!=(idx_n+1) && i!=(idx_d) && i!=(idx_p) && i!=(idx_atp_or_antp)){
            filename = args[i];
            idx_filename = i;
            break;
        }
    }

    // init global variables;
    pc.name = "PC";
    pc.value = 0x400000;    
    clear_prev_state_register();
    clear_current_state_register();

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
    
    while (fgets(str, 128, binary_file) != NULL){
        string line(str);
        lines.push_back(line);
    }

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
    int cycle_cnt = 0;

    EOI = 0x400000 + text_sec_size;

    while(pc.value < EOI + 20){
        if (arg_n==1 & val_n==0){
            if (arg_p || arg_d){
                printf("\n===== Cycle %d =====\n", cycle_cnt);
            }

            if (arg_p){
                print_pipelined_pc();
            }

            if (arg_d){
                print_registers();
            }
            break;
        }

        run_IF_stage();
        run_ID_stage();
        run_EX_stage();
        run_MEM_stage();
        run_WB_stage();

        if (current_state.id_ex_reg.is_jump){ // J, JAL, JR
            IF_ID_Register zero_if_id_reg = {0,};
            current_state.if_id_reg = zero_if_id_reg;
            pc.value = current_state.id_ex_reg.jump_target;
        }
        else if (current_state.id_ex_reg.is_load_use){ // Load-use Hazard Detection
            current_state.if_id_reg = prev_state.if_id_reg;
            ID_EX_Register zero_id_ex_reg = {0,};
            current_state.id_ex_reg = zero_id_ex_reg;   
            pc.value += 0;   
        }
        else if (current_state.mem_wb_reg.is_branch && current_state.mem_wb_reg.predict_correct==0){ // Wrong Predict
            pc.value = current_state.mem_wb_reg.br_target;   
            IF_ID_Register zero_if_id_reg = {0,};
            ID_EX_Register zero_id_ex_reg = {0,};
            EX_MEM_Register zero_ex_mem_reg = {0,};

            current_state.if_id_reg = zero_if_id_reg;
            current_state.id_ex_reg = zero_id_ex_reg;
            current_state.ex_mem_reg = zero_ex_mem_reg;
        }
        else{
            pc.value += 4;
        }

        cycle_cnt += 1;

        pipeline_check();
        pc_IF = current_state.if_id_reg.current_pc;
        pc_ID = current_state.id_ex_reg.current_pc;
        pc_EX = current_state.ex_mem_reg.current_pc;
        pc_MEM = current_state.mem_wb_reg.current_pc;
        pc_WB = prev_state.mem_wb_reg.current_pc;

        if (arg_d){ // arg: d+p+m, d+m, d+p, d
            printf("\n===== Cycle %d =====\n", cycle_cnt);
            if (arg_p){ 
                print_pipelined_pc();
            }
            print_registers();
            if (arg_m){
                print_memory(val_m_start, val_m_end);
            }
        }
 
        else{ // p, p+m
            if (arg_p){ 
                printf("\n===== Cycle %d =====\n", cycle_cnt);
                print_pipelined_pc();
            }
        }

        if (end_flag){
            break;
        }

        if (pc.value != 0x400000 && pipline_empty){
            end_flag = 1; // after one loop, exit.
        }
        else if (pc.value != 0x400000 && pipline_empty && arg_n){
            break; // exit.
        }

        if (arg_n==1 & inst_cnt==val_n){ // end of instruction
            break;
        }
        prev_state = current_state;
        clear_current_state_register();
    }

    if (!arg_d && arg_m){ // arg: p+m, m 
        print_memory(val_m_start, val_m_end);
    }
    if (!arg_n && !arg_d && !arg_p && !arg_m){ // arg: None
        printf("===== Completion cycle: %d =====\n", cycle_cnt);
        pc_WB = 0;
        print_pipelined_pc();
        print_registers();
    }
    
    /*
    output arg
    d+m+p / d+m / d+p / d / m+p / m / p / None
    */
    return 0;
}