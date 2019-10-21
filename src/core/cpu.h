#ifndef CPU_H
#define CPU_H

#include "dma.h"

class CPU {
private:
    enum Register {
        REG_A = 0,
        REG_X = 1,
        REG_Y = 2,
        REG_S = 3,
        REG_P = 4
    };
    enum Flag {
        FLAG_C = 0,
        FLAG_Z = 1,
        FLAG_I = 2,
        FLAG_D = 3,
        FLAG_B1 = 4,
        FLAG_B2 = 5,
        FLAG_V = 6,
        FLAG_N = 7
    };
    uint16_t pc;
    uint8_t reg[5];
    DMA &dma;
    void set_flag(Flag flag);
    void clr_flag(Flag flag);
    uint8_t get_flag(Flag flag);
    void set_flag(Flag flag, bool on);
    void N_flag(uint8_t data);
    void Z_flag(uint8_t data);
    void NZ_flag(uint8_t data);
    void stack_push(uint8_t data);
    uint8_t stack_pop();
    uint8_t addr_A();
    uint16_t addr_abs();
    uint16_t addr_absX();
    uint16_t addr_absY();
    uint8_t addr_imm();
    uint16_t addr_ind();
    uint16_t addr_Xind();
    uint16_t addr_indY();
    uint16_t addr_rel();
    uint16_t addr_zpg();
    uint16_t addr_zpgX();
    uint16_t addr_zpgY();
    void exec_ADC(uint8_t operand);
    void exec_AND(uint8_t operand);
    void exec_ASL_A(uint8_t operand);
    void exec_ASL_dma(uint16_t operand);
    void exec_BCC(uint16_t operand);
    void exec_BCS(uint16_t operand);
    void exec_BEQ(uint16_t operand);
    void exec_BIT(uint8_t operand);
    void exec_BMI(uint16_t operand);
    void exec_BNE(uint16_t operand);
    void exec_BPL(uint16_t operand);
    void exec_BRK();
    void exec_BVC(uint16_t operand);
    void exec_BVS(uint16_t operand);
    void exec_CLC();
    void exec_CLD();
    void exec_CLI();
    void exec_CLV();
    void exec_CMP(uint8_t operand);
    void exec_CPX(uint8_t operand);
    void exec_CPY(uint8_t operand);
    void exec_DEC(uint16_t operand);
    void exec_DEX();
    void exec_DEY();
    void exec_EOR(uint8_t operand);
    void exec_INC(uint16_t operand);
    void exec_INX();
    void exec_INY();
    void exec_JMP(uint16_t operand);
    void exec_JSR(uint16_t operand);
    void exec_LDA(uint8_t operand);
    void exec_LDX(uint8_t operand);
    void exec_LDY(uint8_t operand);
    void exec_LSR_A(uint8_t operand);
    void exec_LSR_dma(uint16_t operand);
    void exec_NOP();
    void exec_ORA(uint8_t operand);
    void exec_PHA();
    void exec_PHP();
    void exec_PLA();
    void exec_PLP();
    void exec_ROL_A(uint8_t operand);
    void exec_ROL_dma(uint16_t operand);
    void exec_ROR_A(uint8_t operand);
    void exec_ROR_dma(uint16_t operand);
    void exec_RTI();
    void exec_RTS();
    void exec_SBC(uint8_t operand);
    void exec_SEC();
    void exec_SED();
    void exec_SEI();
    void exec_STA(uint16_t operand);
    void exec_STX(uint16_t operand);
    void exec_STY(uint16_t operand);
    void exec_TAX();
    void exec_TAY();
    void exec_TSX();
    void exec_TXA();
    void exec_TXS();
    void exec_TYA();
public:
    CPU(DMA &dma);
    void reset();
    void exec_one();
    void start();
    void print_state();
};

#endif // CPU_H
