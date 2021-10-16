#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define OPCODELEN 32
#fedine REGSNUM 8
#define SRAMSIZE 65536
#define MAXPC 65535 //maximum number of instructions 0xFFFF

static int PC;
static int sram[SRAMSIZE] = {0};
static int regs[REGSNUM] = {0};
static char instructions[MAXPC][33] = {NULL};
const char opcodes[25][3] = {"ADD", "SUB", "LSF", "RSF", "AND", "OR", "XOR", "LHI", "LD", "ST", "", "", "", "", "", "", "JLT", "JLE", "JEQ", "JNE", "JIN", "", "", "", "HLT"};

void hex_binary(char* hex, char * res){ //converts an hexa string to binary string
    char binary[16][5] = {"0000", "0001", "0010", "0011", "0100", "0101","0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110","1111"};
    char digits [] = "0123456789abcdef";

    res[0] = '\0';
    int p = 0;
    int value =0;
    while(hex[p])
    {
        const char *v = strchr(digits, tolower(hex[p]));
        if(v[0]>96){
            value=v[0]-87;
        }
        else{
            value=v[0]-48;
        }
        if (v){
            strcat(res, binary[value]);
        }
        p++;
    }
    return;
}

int update_instructions(char* file_name){ //updates the instructions as binary in array and returns number of instructions
    int i=0;
    char line[20];
    FILE *input = fopen(file_name, "r");
    if (input == NULL) {
        fprintf(stderr, "Can't open input file \n");
        exit(1);
    }
    while (fgets(line, LINELEN, input) != NULL) {
        line[8] = '\0';
        char binary[33];
        hex_binary(line, binary);
        strcpy(instructions[i], binary);
        i++
    }
    fclose(input);
    return i;
}

void parse_instruction(char* immediate, char* src1, char* src0, char* dst, char* opcode){
    immediate[16]='\0';
    strncpy(immediate, &instructions[PC][0], 15);
    src1[3] = '\0';
    strncpy(src1, &instructions[PC][16], 3);
    src0[3] = '\0';
    strncpy(src0, &instructions[PC][19], 3);
    dst[3] = '\0';
    strncpy(dst, &instructions[PC][22], 3);
    opcode[6] = '\0';
    strncpy(opcode, &instructions[PC][25], 5);
    return;
}

bool execute_instruction(char* immediate, char* src1, char* src0, char* dst, char* opcode){//handles op and returns true iff a jump was taken
    int opcode_num = (int) strtol(opcode, NULL, 2);
    int dst_num = (int) strtol(dst, NULL, 2);
    int src0_num = (int) strtol(src0, NULL, 2);
    int src1_num = (int) strtol(src1, NULL, 2);
    int imm = (int) strtol(immediate, NULL, 2);
    regs[1] = imm;
    if(opcode_num == 0){//ADD
        regs[dst_num] = regs[src0_num] + regs[src1_num];
        return false;
    }
    if(opcode_num == 1){//SUB
        regs[dst_num] = regs[src0_num] - regs[src1_num];
        return false;
    }
    if(opcode_num == 2){//LSF
        regs[dst_num] = regs[src0_num] << regs[src1_num];
        return false;
    }
    if(opcode_num == 3){//RSF
        regs[dst_num] = regs[src0_num] >> regs[src1_num];
        return false;
    }
    if(opcode_num == 4){//AND
        regs[dst_num] = regs[src0_num] & regs[src1_num];
        return false;
    }
    if(opcode_num == 5){//OR
        regs[dst_num] = regs[src0_num] | regs[src1_num];
        return false;
    }
    if(opcode_num == 6){//XOR
        regs[dst_num] = regs[src0_num] ^ regs[src1_num];
        return false;
    }
    if(opcode_num == 7){//LHI
        regs[dst_num] = imm << 16;//needs to do
        return false;
    }
    if(opcode_num == 8){//LD
        regs[dst_num] = sram[regs[src1_num]];
        return false;
    }
    if(opcode_num == 9){//ST
        sram[regs[src1_num]] = regs[src0_num];
        return false;
    }
    if(opcode_num == 16){//JLT
        if(regs[src0_num] < regs[src1_num])
        {
            regs[7] = PC;
            PC = imm;
            return true;
        }
        return false;
    }
    if(opcode_num == 17){//JLE
        if(regs[src0_num] <= regs[src1_num])
        {
            regs[7] = PC;
            PC = imm;
            return true;
        }
        return false;
    }
    if(opcode_num == 18){//JEQ
        if(regs[src0_num] == regs[src1_num])
        {
            regs[7] = PC;
            PC = imm;
            return true;
        }
        return false;
    }
    if(opcode_num == 19){//JNE
        if(regs[src0_num] != regs[src1_num])
        {
            regs[7] = PC;
            PC = imm;
            return true;
        }
        return false;
    }
    if(opcode_num == 20){//JIN
        regs[7] = PC;
        PC = imm;
        return true;
    }
    if(opcode_num == 24){//HLT
        //print sram
        exit(0);
    }
}


int main(int argc, char** argv[]){
    char* input_name = argv[0];
    int num_instructions = update_instructions(input_name);
    PC=0;
    while(true){
        char immediate[20], src1[20], src0[20], dst[20], opcode[20];
        parse_instruction(immediate, src1, src0, dst, opcode);
        //print trace
        bool jump_taken = execute_instruction(immediate, src1, src0, dst, opcode);
        if(!jump_taken){
            if(PC == MAXPC){
                PC=0;
            }
            else{
                PC++;
            }
        }
    }
    exit(1);
}
