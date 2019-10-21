#include "cpu.h"

#include <cstdio>
#include <stdexcept>

using namespace std;

void CPU::set_flag(Flag flag)
{
    reg[REG_P] |= 1 << flag;
}

void CPU::clr_flag(Flag flag)
{
    reg[REG_P] &= ~(1 << flag);
}

uint8_t CPU::get_flag(Flag flag)
{
    return (reg[REG_P] >> flag) & 1;
}

void CPU::set_flag(Flag flag, bool on)
{
    if (on)
        set_flag(flag);
    else
        clr_flag(flag);
}

void CPU::N_flag(uint8_t data)
{
    set_flag(FLAG_N, (data & 0x80) != 0);
}

void CPU::Z_flag(uint8_t data)
{
    set_flag(FLAG_Z, data == 0);
}

void CPU::NZ_flag(uint8_t data)
{
    N_flag(data);
    Z_flag(data);
}

void CPU::stack_push(uint8_t data)
{
#ifdef PRINT_TRACE
    printf("Stack Push: 0x%02X\n", data);
#endif // PRINT_TRACE
    dma.write(0x100 + (reg[REG_S]--), data);
}

uint8_t CPU::stack_pop()
{
    uint8_t ret = dma.read(0x100 + (++reg[REG_S]));
#ifdef PRINT_TRACE
    printf("Stack Pop: 0x%02X\n", ret);
#endif // PRINT_TRACE
    return ret;
}

uint8_t CPU::addr_A()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: A");
#endif // PRINT_TRACE
    return reg[REG_A];
}

uint16_t CPU::addr_abs()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: abs");
#endif // PRINT_TRACE
    uint16_t ret = dma.read_dword(pc);
    pc += 2;
    return ret;
}

uint16_t CPU::addr_absX()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: absX");
#endif // PRINT_TRACE
    uint16_t ret = dma.read_dword(pc) + reg[REG_X];
    pc += 2;
    return ret;
}

uint16_t CPU::addr_absY()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: absY");
#endif // PRINT_TRACE
    uint16_t ret = dma.read_dword(pc) + reg[REG_Y];
    pc += 2;
    return ret;
}

uint8_t CPU::addr_imm()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: imm");
#endif // PRINT_TRACE
    return dma.read(pc++);
}

uint16_t CPU::addr_ind()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: ind");
#endif // PRINT_TRACE
    uint16_t ret = dma.read_dword(dma.read_dword(pc));
    pc += 2;
    return ret;
}

uint16_t CPU::addr_Xind()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: Xind");
#endif // PRINT_TRACE
    return dma.read_dword((dma.read(pc++) + reg[REG_X]) & 0xFF);
}

uint16_t CPU::addr_indY()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: indY");
#endif // PRINT_TRACE
    return dma.read_dword(dma.read(pc++)) + reg[REG_Y];
}

uint16_t CPU::addr_rel()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: rel");
#endif // PRINT_TRACE
    int8_t offset = (int8_t) dma.read(pc++);
    return (uint16_t) ((int16_t) pc + offset);
}

uint16_t CPU::addr_zpg()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: zpg");
#endif // PRINT_TRACE
    return dma.read(pc++);
}

uint16_t CPU::addr_zpgX()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: zpgX");
#endif // PRINT_TRACE
    return (dma.read(pc++) + reg[REG_X]) & 0xFF;
}

uint16_t CPU::addr_zpgY()
{
#ifdef PRINT_TRACE
    puts("Addressing Mode: zpgY");
#endif // PRINT_TRACE
    return (dma.read(pc++) + reg[REG_Y]) & 0xFF;
}

void CPU::exec_ADC(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ADC");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    uint16_t result = (uint16_t) reg[REG_A] + operand + get_flag(FLAG_C);
    NZ_flag(reg[REG_A] = result);
    set_flag(FLAG_C, (result & 0x100) != 0);
    set_flag(FLAG_V, ((result ^ reg[REG_A]) & (result ^ operand) & 0x80) != 0);
}

void CPU::exec_AND(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: AND");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    NZ_flag(reg[REG_A] &= operand);
}

void CPU::exec_ASL_A(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ASL");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    uint16_t result = (uint16_t) operand << 1;
    NZ_flag(reg[REG_A] = result);
    set_flag(FLAG_C, (result & 0x100) != 0);
}

void CPU::exec_ASL_dma(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ASL");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    uint16_t result = (uint16_t) dma.read(operand) << 1;
    dma.write(operand, result);
    NZ_flag(result);
    set_flag(FLAG_C, (result & 0x100) != 0);
}

void CPU::exec_BCC(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BCC");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (!get_flag(FLAG_C))
        pc = operand;
}

void CPU::exec_BCS(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BCS");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (get_flag(FLAG_C))
        pc = operand;
}

void CPU::exec_BEQ(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BEQ");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (get_flag(FLAG_Z))
        pc = operand;
}

void CPU::exec_BIT(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BIT");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    set_flag(FLAG_N, (operand & 0x80) != 0);
    set_flag(FLAG_V, (operand & 0x40) != 0);
    Z_flag(reg[REG_A] & operand);
}

void CPU::exec_BMI(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BMI");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (get_flag(FLAG_N))
        pc = operand;
}

void CPU::exec_BNE(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BNE");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (!get_flag(FLAG_Z))
        pc = operand;
}

void CPU::exec_BPL(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BPL");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (!get_flag(FLAG_N))
        pc = operand;
}

void CPU::exec_BRK()
{
#ifdef PRINT_TRACE
    puts("Instruction: BRK");
#endif // PRINT_TRACE
    stack_push((++pc) >> 8);
    stack_push(pc & 0xFF);
    set_flag(FLAG_B1);
    set_flag(FLAG_B2);
    stack_push(reg[REG_P]);
    pc = dma.read_dword(0xFFFE);
}

void CPU::exec_BVC(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BVC");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (!get_flag(FLAG_V))
        pc = operand;
}

void CPU::exec_BVS(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: BVS");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    if (get_flag(FLAG_V))
        pc = operand;
}

void CPU::exec_CLC()
{
#ifdef PRINT_TRACE
    puts("Instruction: CLC");
#endif // PRINT_TRACE
    clr_flag(FLAG_C);
}

void CPU::exec_CLD()
{
#ifdef PRINT_TRACE
    puts("Instruction: CLD");
#endif // PRINT_TRACE
    clr_flag(FLAG_D);
}

void CPU::exec_CLI()
{
#ifdef PRINT_TRACE
    puts("Instruction: CLI");
#endif // PRINT_TRACE
    clr_flag(FLAG_I);
}

void CPU::exec_CLV()
{
#ifdef PRINT_TRACE
    puts("Instruction: CLV");
#endif // PRINT_TRACE
    clr_flag(FLAG_V);
}

void CPU::exec_CMP(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: CMP");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    uint16_t result = (uint16_t) reg[REG_A] - operand;
    NZ_flag(result);
    set_flag(FLAG_C, reg[REG_A] >= operand);
}

void CPU::exec_CPX(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: CPX");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    uint16_t result = (uint16_t) reg[REG_X] - operand;
    NZ_flag(result);
    set_flag(FLAG_C, reg[REG_X] >= operand);
}

void CPU::exec_CPY(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: CPY");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    uint16_t result = (uint16_t) reg[REG_Y] - operand;
    NZ_flag(result);
    set_flag(FLAG_C, reg[REG_Y] >= operand);
}

void CPU::exec_DEC(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: DEC");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    uint8_t result = dma.read(operand) - 1;
    dma.write(operand, result);
    NZ_flag(result);
}

void CPU::exec_DEX()
{
#ifdef PRINT_TRACE
    puts("Instruction: DEX");
#endif // PRINT_TRACE
    NZ_flag(--reg[REG_X]);
}

void CPU::exec_DEY()
{
#ifdef PRINT_TRACE
    puts("Instruction: DEY");
#endif // PRINT_TRACE
    NZ_flag(--reg[REG_Y]);
}

void CPU::exec_EOR(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: EOR");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    NZ_flag(reg[REG_A] ^= operand);
}

void CPU::exec_INC(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: INC");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    uint8_t result = dma.read(operand) + 1;
    dma.write(operand, result);
    NZ_flag(result);
}

void CPU::exec_INX()
{
#ifdef PRINT_TRACE
    puts("Instruction: INX");
#endif // PRINT_TRACE
    NZ_flag(++reg[REG_X]);
}

void CPU::exec_INY()
{
#ifdef PRINT_TRACE
    puts("Instruction: INY");
#endif // PRINT_TRACE
    NZ_flag(++reg[REG_Y]);
}

void CPU::exec_JMP(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: JMP");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    pc = operand;
}

void CPU::exec_JSR(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: JSR");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    stack_push((--pc) >> 8);
    stack_push(pc & 0xFF);
    pc = operand;
}

void CPU::exec_LDA(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: LDA");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    NZ_flag(reg[REG_A] = operand);
}

void CPU::exec_LDX(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: LDX");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    NZ_flag(reg[REG_X] = operand);
}

void CPU::exec_LDY(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: LDY");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    NZ_flag(reg[REG_Y] = operand);
}

void CPU::exec_LSR_A(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: LSR");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    set_flag(FLAG_C, (operand & 0x01) != 0);
    NZ_flag(reg[REG_A] = operand >> 1);
}

void CPU::exec_LSR_dma(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: LSR");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    uint8_t tmp = dma.read(operand);
    set_flag(FLAG_C, (tmp & 0x01) != 0);
    NZ_flag(tmp >>= 1);
    dma.write(operand, tmp);
}

void CPU::exec_NOP()
{
#ifdef PRINT_TRACE
    puts("Instruction: NOP");
#endif // PRINT_TRACE
}

void CPU::exec_ORA(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ORA");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    NZ_flag(reg[REG_A] |= operand);
}

void CPU::exec_PHA()
{
#ifdef PRINT_TRACE
    puts("Instruction: PHA");
#endif // PRINT_TRACE
    stack_push(reg[REG_A]);
}

void CPU::exec_PHP()
{
#ifdef PRINT_TRACE
    puts("Instruction: PHP");
#endif // PRINT_TRACE
    stack_push(reg[REG_P]);
}

void CPU::exec_PLA()
{
#ifdef PRINT_TRACE
    puts("Instruction: PLA");
#endif // PRINT_TRACE
    NZ_flag(reg[REG_A] = stack_pop());
}

void CPU::exec_PLP()
{
#ifdef PRINT_TRACE
    puts("Instruction: PLP");
#endif // PRINT_TRACE
    reg[REG_P] = stack_pop();
}

void CPU::exec_ROL_A(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ROL");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    bool tmp_C = (operand & 0x80) != 0;
    NZ_flag(reg[REG_A] = (operand << 1) | get_flag(FLAG_C));
    set_flag(FLAG_C, tmp_C);
}

void CPU::exec_ROL_dma(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ROL");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    uint8_t tmp = dma.read(operand);
    bool tmp_C = (tmp & 0x80) != 0;
    NZ_flag(tmp = (tmp << 1) | get_flag(FLAG_C));
    dma.write(operand, tmp);
    set_flag(FLAG_C, tmp_C);
}

void CPU::exec_ROR_A(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ROR");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    bool tmp_C = (operand & 0x01) != 0;
    NZ_flag(reg[REG_A] = (operand >> 1) | (get_flag(FLAG_C) << 7));
    set_flag(FLAG_C, tmp_C);
}

void CPU::exec_ROR_dma(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: ROR");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    uint8_t tmp = dma.read(operand);
    bool tmp_C = (tmp & 0x01) != 0;
    NZ_flag(tmp = (tmp << 1) | (get_flag(FLAG_C) << 7));
    dma.write(operand, tmp);
    set_flag(FLAG_C, tmp_C);
}

void CPU::exec_RTI()
{
#ifdef PRINT_TRACE
    puts("Instruction: RTI");
#endif // PRINT_TRACE
    reg[REG_P] = stack_pop();
    pc = stack_pop();
    pc |= stack_pop() << 8;
}

void CPU::exec_RTS()
{
#ifdef PRINT_TRACE
    puts("Instruction: RTS");
#endif // PRINT_TRACE
    pc = stack_pop();
    pc |= stack_pop() << 8;
    ++pc;
}

void CPU::exec_SBC(uint8_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: SBC");
    printf("Operand: 0x%02X\n", operand);
#endif // PRINT_TRACE
    operand = ~operand;
    uint16_t result = (uint16_t) reg[REG_A] + operand + get_flag(FLAG_C);
    NZ_flag(reg[REG_A] = result);
    set_flag(FLAG_C, (result & 0x100) != 0);
    set_flag(FLAG_V, ((result ^ reg[REG_A]) & (result ^ operand) & 0x80) != 0);
}

void CPU::exec_SEC()
{
#ifdef PRINT_TRACE
    puts("Instruction: SEC");
#endif // PRINT_TRACE
    set_flag(FLAG_C);
}

void CPU::exec_SED()
{
#ifdef PRINT_TRACE
    puts("Instruction: SED");
#endif // PRINT_TRACE
    set_flag(FLAG_D);
}

void CPU::exec_SEI()
{
#ifdef PRINT_TRACE
    puts("Instruction: SEI");
#endif // PRINT_TRACE
    set_flag(FLAG_I);
}

void CPU::exec_STA(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: STA");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    dma.write(operand, reg[REG_A]);
}

void CPU::exec_STX(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: STX");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    dma.write(operand, reg[REG_X]);
}

void CPU::exec_STY(uint16_t operand)
{
#ifdef PRINT_TRACE
    puts("Instruction: STY");
    printf("Operand: 0x%04X\n", operand);
#endif // PRINT_TRACE
    dma.write(operand, reg[REG_Y]);
}

void CPU::exec_TAX()
{
#ifdef PRINT_TRACE
    puts("Instruction: TAX");
#endif // PRINT_TRACE
    NZ_flag(reg[REG_X] = reg[REG_A]);
}

void CPU::exec_TAY()
{
#ifdef PRINT_TRACE
    puts("Instruction: TAY");
#endif // PRINT_TRACE
    NZ_flag(reg[REG_Y] = reg[REG_A]);
}

void CPU::exec_TSX()
{
#ifdef PRINT_TRACE
    puts("Instruction: TSX");
#endif // PRINT_TRACE
    NZ_flag(reg[REG_X] = reg[REG_S]);
}

void CPU::exec_TXA()
{
#ifdef PRINT_TRACE
    puts("Instruction: TXA");
#endif // PRINT_TRACE
    NZ_flag(reg[REG_A] = reg[REG_X]);
}

void CPU::exec_TXS()
{
#ifdef PRINT_TRACE
    puts("Instruction: TXS");
#endif // PRINT_TRACE
    reg[REG_S] = reg[REG_X];
}

void CPU::exec_TYA()
{
#ifdef PRINT_TRACE
    puts("Instruction: TYA");
#endif // PRINT_TRACE
    NZ_flag(reg[REG_A] = reg[REG_Y]);
}

CPU::CPU(DMA &dma) : dma(dma)
{
    reg[REG_P] = 0x34;
    reg[REG_A] = reg[REG_X] = reg[REG_Y] = 0x00;
    reg[REG_S] = 0xFD;
}

void CPU::reset()
{
    reg[REG_S] -= 0x03;
    set_flag(FLAG_I);
    pc = dma.read_dword(0xFFFC);
}

void CPU::exec_one()
{
#ifdef PRINT_TRACE
    print_state();
#endif // PRINT_TRACE
    uint8_t opcode = dma.read(pc++);
    switch (opcode) {
    case 0x00: exec_BRK(); break;
    case 0x01: exec_ORA(dma.read(addr_Xind())); break;
    case 0x05: exec_ORA(dma.read(addr_zpg())); break;
    case 0x06: exec_ASL_dma(addr_zpg()); break;
    case 0x08: exec_PHP(); break;
    case 0x09: exec_ORA(addr_imm()); break;
    case 0x0A: exec_ASL_A(addr_A()); break;
    case 0x0D: exec_ORA(dma.read(addr_abs())); break;
    case 0x0E: exec_ASL_dma(addr_abs()); break;
    case 0x10: exec_BPL(addr_rel()); break;
    case 0x11: exec_ORA(dma.read(addr_indY())); break;
    case 0x15: exec_ORA(dma.read(addr_zpgX())); break;
    case 0x16: exec_ASL_dma(addr_zpgX()); break;
    case 0x18: exec_CLC(); break;
    case 0x19: exec_ORA(dma.read(addr_absY())); break;
    case 0x1D: exec_ORA(dma.read(addr_absX())); break;
    case 0x1E: exec_ASL_dma(addr_absX()); break;
    case 0x20: exec_JSR(addr_abs()); break;
    case 0x21: exec_AND(dma.read(addr_Xind())); break;
    case 0x24: exec_BIT(dma.read(addr_zpg())); break;
    case 0x25: exec_AND(dma.read(addr_zpg())); break;
    case 0x26: exec_ROL_dma(addr_zpg()); break;
    case 0x28: exec_PLP(); break;
    case 0x29: exec_AND(addr_imm()); break;
    case 0x2A: exec_ROL_A(addr_A()); break;
    case 0x2C: exec_BIT(dma.read(addr_abs())); break;
    case 0x2D: exec_AND(dma.read(addr_abs())); break;
    case 0x2E: exec_ROL_dma(addr_abs()); break;
    case 0x30: exec_BMI(addr_rel()); break;
    case 0x31: exec_AND(dma.read(addr_indY())); break;
    case 0x35: exec_AND(dma.read(addr_zpgX())); break;
    case 0x36: exec_ROL_dma(addr_zpgX()); break;
    case 0x38: exec_SEC(); break;
    case 0x39: exec_AND(dma.read(addr_absY())); break;
    case 0x3D: exec_AND(dma.read(addr_absX())); break;
    case 0x3E: exec_ROL_dma(addr_absX()); break;
    case 0x40: exec_RTI(); break;
    case 0x41: exec_EOR(dma.read(addr_Xind())); break;
    case 0x45: exec_EOR(dma.read(addr_zpg())); break;
    case 0x46: exec_LSR_dma(addr_zpg()); break;
    case 0x48: exec_PHA(); break;
    case 0x49: exec_EOR(addr_imm()); break;
    case 0x4A: exec_LSR_A(addr_A()); break;
    case 0x4C: exec_JMP(addr_abs()); break;
    case 0x4D: exec_EOR(dma.read(addr_abs())); break;
    case 0x4E: exec_LSR_dma(addr_abs()); break;
    case 0x50: exec_BVC(addr_rel()); break;
    case 0x51: exec_EOR(dma.read(addr_indY())); break;
    case 0x55: exec_EOR(dma.read(addr_zpgX())); break;
    case 0x56: exec_LSR_dma(addr_zpgX()); break;
    case 0x58: exec_CLI(); break;
    case 0x59: exec_EOR(dma.read(addr_absY())); break;
    case 0x5D: exec_EOR(dma.read(addr_absX())); break;
    case 0x5E: exec_LSR_dma(addr_absX()); break;
    case 0x60: exec_RTS(); break;
    case 0x61: exec_ADC(dma.read(addr_Xind())); break;
    case 0x65: exec_ADC(dma.read(addr_zpg())); break;
    case 0x66: exec_ROR_dma(addr_zpg()); break;
    case 0x68: exec_PLA(); break;
    case 0x69: exec_ADC(addr_imm()); break;
    case 0x6A: exec_ROR_A(addr_A()); break;
    case 0x6C: exec_JMP(addr_ind()); break;
    case 0x6D: exec_ADC(dma.read(addr_abs())); break;
    case 0x6E: exec_ROR_dma(addr_abs()); break;
    case 0x70: exec_BVS(addr_rel()); break;
    case 0x71: exec_ADC(dma.read(addr_indY())); break;
    case 0x75: exec_ADC(dma.read(addr_zpgX())); break;
    case 0x76: exec_ROR_dma(addr_zpgX()); break;
    case 0x78: exec_SEI(); break;
    case 0x79: exec_ADC(dma.read(addr_absY())); break;
    case 0x7D: exec_ADC(dma.read(addr_absX())); break;
    case 0x7E: exec_ROR_dma(addr_absX()); break;
    case 0x81: exec_STA(addr_Xind()); break;
    case 0x84: exec_STY(addr_zpg()); break;
    case 0x85: exec_STA(addr_zpg()); break;
    case 0x86: exec_STX(addr_zpg()); break;
    case 0x88: exec_DEY(); break;
    case 0x8A: exec_TXA(); break;
    case 0x8C: exec_STY(addr_abs()); break;
    case 0x8D: exec_STA(addr_abs()); break;
    case 0x8E: exec_STX(addr_abs()); break;
    case 0x90: exec_BCC(addr_rel()); break;
    case 0x91: exec_STA(addr_indY()); break;
    case 0x94: exec_STY(addr_zpgX()); break;
    case 0x95: exec_STA(addr_zpgX()); break;
    case 0x96: exec_STX(addr_zpgY()); break;
    case 0x98: exec_TYA(); break;
    case 0x99: exec_STA(addr_absY()); break;
    case 0x9A: exec_TXS(); break;
    case 0x9D: exec_STA(addr_absX()); break;
    case 0xA0: exec_LDY(addr_imm()); break;
    case 0xA1: exec_LDA(dma.read(addr_Xind())); break;
    case 0xA2: exec_LDX(addr_imm()); break;
    case 0xA4: exec_LDY(dma.read(addr_zpg())); break;
    case 0xA5: exec_LDA(dma.read(addr_zpg())); break;
    case 0xA6: exec_LDX(dma.read(addr_zpg())); break;
    case 0xA8: exec_TAY(); break;
    case 0xA9: exec_LDA(addr_imm()); break;
    case 0xAA: exec_TAX(); break;
    case 0xAC: exec_LDY(dma.read(addr_abs())); break;
    case 0xAD: exec_LDA(dma.read(addr_abs())); break;
    case 0xAE: exec_LDX(dma.read(addr_abs())); break;
    case 0xB0: exec_BCS(addr_rel()); break;
    case 0xB1: exec_LDA(dma.read(addr_indY())); break;
    case 0xB4: exec_LDY(dma.read(addr_zpgX())); break;
    case 0xB5: exec_LDA(dma.read(addr_zpgX())); break;
    case 0xB6: exec_LDX(dma.read(addr_zpgY())); break;
    case 0xB8: exec_CLV(); break;
    case 0xB9: exec_LDA(dma.read(addr_absY())); break;
    case 0xBA: exec_TSX(); break;
    case 0xBC: exec_LDY(dma.read(addr_absX())); break;
    case 0xBD: exec_LDA(dma.read(addr_absX())); break;
    case 0xBE: exec_LDX(dma.read(addr_absY())); break;
    case 0xC0: exec_CPY(addr_imm()); break;
    case 0xC1: exec_CMP(dma.read(addr_Xind())); break;
    case 0xC4: exec_CPY(dma.read(addr_zpg())); break;
    case 0xC5: exec_CMP(dma.read(addr_zpg())); break;
    case 0xC6: exec_DEC(addr_zpg()); break;
    case 0xC8: exec_INY(); break;
    case 0xC9: exec_CMP(addr_imm()); break;
    case 0xCA: exec_DEX(); break;
    case 0xCC: exec_CPY(dma.read(addr_abs())); break;
    case 0xCD: exec_CMP(dma.read(addr_abs())); break;
    case 0xCE: exec_DEC(addr_abs()); break;
    case 0xD0: exec_BNE(addr_rel()); break;
    case 0xD1: exec_CMP(dma.read(addr_indY())); break;
    case 0xD5: exec_CMP(dma.read(addr_zpgX())); break;
    case 0xD6: exec_DEC(addr_zpgX()); break;
    case 0xD8: exec_CLD(); break;
    case 0xD9: exec_CMP(dma.read(addr_absY())); break;
    case 0xDD: exec_CMP(dma.read(addr_absX())); break;
    case 0xDE: exec_DEC(addr_absX()); break;
    case 0xE0: exec_CPX(addr_imm()); break;
    case 0xE1: exec_SBC(dma.read(addr_Xind())); break;
    case 0xE4: exec_CPX(dma.read(addr_zpg())); break;
    case 0xE5: exec_SBC(dma.read(addr_zpg())); break;
    case 0xE6: exec_INC(addr_zpg()); break;
    case 0xE8: exec_INX(); break;
    case 0xE9: exec_SBC(addr_imm()); break;
    case 0xEA: exec_NOP(); break;
    case 0xEC: exec_CPX(dma.read(addr_abs())); break;
    case 0xED: exec_SBC(dma.read(addr_abs())); break;
    case 0xEE: exec_INC(addr_abs()); break;
    case 0xF0: exec_BEQ(addr_rel()); break;
    case 0xF1: exec_SBC(dma.read(addr_indY())); break;
    case 0xF5: exec_SBC(dma.read(addr_zpgX())); break;
    case 0xF6: exec_INC(addr_zpgX()); break;
    case 0xF8: exec_SED(); break;
    case 0xF9: exec_SBC(dma.read(addr_absY())); break;
    case 0xFD: exec_SBC(dma.read(addr_absX())); break;
    case 0xFE: exec_INC(addr_absX()); break;
    default: throw runtime_error("invalid opcode: " + to_string(opcode));
    }
#ifdef PRINT_TRACE
    puts("");
    puts("================================================================================");
    puts("");
#endif // PRINT_TRACE
}

void CPU::start()
{
    for (;;)
        exec_one();
}

void CPU::print_state()
{
    printf("PC: 0x%04X\n", pc);
    printf("A: 0x%02X\n", reg[REG_A]);
    printf("X: 0x%02X\n", reg[REG_X]);
    printf("Y: 0x%02X\n", reg[REG_Y]);
    printf("S: 0x%02X\n", reg[REG_S]);
    printf("Flag C: %u\n", get_flag(FLAG_C));
    printf("Flag Z: %u\n", get_flag(FLAG_Z));
    printf("Flag I: %u\n", get_flag(FLAG_I));
    printf("Flag D: %u\n", get_flag(FLAG_D));
    printf("Flag V: %u\n", get_flag(FLAG_V));
    printf("Flag N: %u\n", get_flag(FLAG_N));
    puts("");
}
