#include <stdint.h>
#include <stdio.h>

static enum opcodes {
    LW = 0x01,
    SW = 0x02,
    LW_IO = 0x03,
    ADD = 0x05,
    SW_IO = 0x07,
};

static void Ex(uint16_t *prog_counter, uint16_t *accumulator, uint16_t instr, uint16_t *mem_ptr, uint8_t *io_ptr) {
    *prog_counter += 1;
    enum opcodes opcode = (instr >> 12);
    uint16_t address = instr & ~(0xF000);
    switch(opcode) {
        case SW:
            mem_ptr[address] = *accumulator;
            break;
        case LW_IO:
            *accumulator = io_ptr[address];
            break;
        case ADD:
            *accumulator += mem_ptr[address];
            break;
        default:
            break;
    }
}
static void Fetch(uint16_t prog_counter, uint16_t *instr_reg, uint16_t *mem) {
    *instr_reg = mem[prog_counter];
}

static void init(uint16_t *mem, uint8_t *io_devices) {
    mem[0x200] = 0x3005;
    mem[0x201] = 0x5500;
    mem[0x202] = 0x2501;
    mem[0x500] = 0x0005;
    io_devices[0x5] = 0x2;
}

int main(void) {
    uint16_t pc = 0x200;
    uint16_t ac = 0;
    uint16_t ir = 0;
    uint8_t cc = 0;

    uint16_t memory[0x600];
    uint8_t devices[0x9];

    init(memory, devices);
    while (pc < 0x203) {
        Fetch(pc, &ir, memory);
        cc += 1;
        printf("Fetch\nPC = %X\nAC = %X\nIR = %X\n", pc, ac, ir);
        printf("Total Clock Cycles = %d\n\n", cc);

        Ex(&pc, &ac, ir, memory, devices);
        cc += 1;
        printf("Execute\nPC = %X\nAC = %X\nIR = %X\n", pc, ac, ir);
        printf("Total Clock Cycles = %d\n\n", cc);
    }
    printf("The value in address 0x501: %X", memory[0x501]);
}