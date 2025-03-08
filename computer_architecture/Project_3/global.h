using namespace std;

struct Register{
    string name;
    unsigned int value;
};

struct IF_ID_Register{
    unsigned int instr;
    unsigned int next_pc;
    unsigned int current_pc; 
};

struct ID_EX_Register{
    unsigned int next_pc;
    unsigned int rs;
    unsigned int rt;
    unsigned int rd;
    unsigned int imm;
    unsigned int opcode; 
    unsigned int current_pc;
    unsigned int is_jump;
    unsigned int jump_target;
    unsigned int is_load_use;
};

struct EX_MEM_Register{
    unsigned int alu_out;
    unsigned int br_target;
    unsigned int write_to_mem;
    unsigned int write_to_mem_val;
    unsigned int read_from_mem;
    unsigned int write_to_reg; 
    int reg_num; 
    unsigned int is_branch;
    unsigned int predict_correct;
    unsigned int current_pc;
};

struct MEM_WB_Register{
    unsigned int alu_out;
    unsigned int mem_out;
    unsigned int write_to_reg;
    int reg_num;
    int select_alu;
    unsigned int br_target;
    unsigned int is_branch;
    unsigned int predict_correct;
    unsigned int read_from_mem;
    unsigned int current_pc;
};

struct State_Register{
    IF_ID_Register if_id_reg;
    ID_EX_Register id_ex_reg;
    EX_MEM_Register ex_mem_reg;
    MEM_WB_Register mem_wb_reg;    
};

extern Register pc;
extern vector<Register> registers; 
extern State_Register prev_state;
extern State_Register current_state;
extern map<unsigned int, unsigned int> memory;