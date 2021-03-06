#pragma once
#include <cstdint>
#include <AddressingModes.hpp>

#define SET_ZN(val)                                  \
    this->cpu->setFlag(Ricoh2A03::Z, (val) == 0x00); \
    this->cpu->setFlag(Ricoh2A03::N, (val)&0x80);

// Interrupt Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class BRK : public AddressingMode<T>
{
public:
    BRK(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BRK", cpu, numCycles) {}

    uint8_t exec() override final
    {
#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
#endif
        ++this->cpu->PC;
        this->cpu->setFlag(Ricoh2A03::I, true);
        this->cpu->setFlag(Ricoh2A03::B, true);

        this->cpu->pushDoubleWord(this->cpu->PC);
        this->cpu->pushWord(this->cpu->S);

        this->cpu->PC = this->cpu->readDoubleWord(0xFFFE);
        this->cpu->setFlag(Ricoh2A03::B, false);

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class RTI : public AddressingMode<T>
{
public:
    RTI(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("RTI", cpu, numCycles) {}

    uint8_t exec() override final
    {
#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
#endif

        this->cpu->S = this->cpu->popWord();
        this->cpu->setFlag(Ricoh2A03::U, false);
        this->cpu->setFlag(Ricoh2A03::B, false);

        this->cpu->PC = this->cpu->popDoubleWord();

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Subroutine Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class JSR : public AddressingMode<T>
{
public:
    JSR(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("JSR", cpu, numCycles) {}

    uint8_t exec() override final
    {
#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
#endif
        this->fetchAuxData(false);
        this->cpu->pushDoubleWord(this->cpu->PC - 1);
        this->cpu->PC = this->absoluteAddress;

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class RTS : public AddressingMode<T>
{
public:
    RTS(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("RTS", cpu, numCycles) {}

    uint8_t exec() override final
    {
#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
#endif
        this->cpu->PC = this->cpu->popDoubleWord();
        ++this->cpu->PC;

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Direct Stack Intructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class PLA : public AddressingMode<T>
{
public:
    PLA(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("PLA", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->A = this->cpu->popWord();
        SET_ZN(this->cpu->A)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class PHA : public AddressingMode<T>
{
public:
    PHA(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("PHA", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->pushWord(this->cpu->A);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class PLP : public AddressingMode<T>
{
public:
    PLP(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("PLP", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->S = this->cpu->popWord();
        this->cpu->setFlag(Ricoh2A03::U, true);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class PHP : public AddressingMode<T>
{
public:
    PHP(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("PHP", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::U, true);
        this->cpu->setFlag(Ricoh2A03::B, true);

        this->cpu->pushWord(this->cpu->S);

        this->cpu->setFlag(Ricoh2A03::U, false);
        this->cpu->setFlag(Ricoh2A03::B, false);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Load Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class LDA : public AddressingMode<T>
{
public:
    LDA(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("LDA", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        this->cpu->A = this->auxData;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class LDX : public AddressingMode<T>
{
public:
    LDX(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("LDX", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        this->cpu->X = this->auxData;
        SET_ZN(this->cpu->X)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class LDY : public AddressingMode<T>
{
public:
    LDY(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("LDY", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        this->cpu->Y = this->auxData;
        SET_ZN(this->cpu->Y)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

// Store Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class STA : public AddressingMode<T>
{
public:
    STA(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("STA", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData(false);
        this->auxData = this->cpu->A;
        this->writeBack();

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class STX : public AddressingMode<T>
{
public:
    STX(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("STX", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData(false);
        this->auxData = this->cpu->X;
        this->writeBack();

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class STY : public AddressingMode<T>
{
public:
    STY(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("STY", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData(false);
        this->auxData = this->cpu->Y;
        this->writeBack();

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Register Transfer Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class TAX : public AddressingMode<T>
{
public:
    TAX(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("TAX", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->X = this->cpu->A;
        SET_ZN(this->cpu->X)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class TXA : public AddressingMode<T>
{
public:
    TXA(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("TXA", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->A = this->cpu->X;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class TSX : public AddressingMode<T>
{
public:
    TSX(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("TSX", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->X = this->cpu->SP;
        SET_ZN(this->cpu->X)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class TXS : public AddressingMode<T>
{
public:
    TXS(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("TXS", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->SP = this->cpu->X;

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class TAY : public AddressingMode<T>
{
public:
    TAY(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("TAY", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->Y = this->cpu->A;
        SET_ZN(this->cpu->Y)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class TYA : public AddressingMode<T>
{
public:
    TYA(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("TYA", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->A = this->cpu->Y;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Branch Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class BEQ : public AddressingMode<T>
{
public:
    BEQ(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BEQ", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             this->cpu->getFlag(Ricoh2A03::Z));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class BNE : public AddressingMode<T>
{
public:
    BNE(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BNE", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             !this->cpu->getFlag(Ricoh2A03::Z));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class BCS : public AddressingMode<T>
{
public:
    BCS(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BCS", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             this->cpu->getFlag(Ricoh2A03::C));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class BCC : public AddressingMode<T>
{
public:
    BCC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BCC", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             !this->cpu->getFlag(Ricoh2A03::C));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class BVS : public AddressingMode<T>
{
public:
    BVS(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BVS", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             this->cpu->getFlag(Ricoh2A03::V));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class BVC : public AddressingMode<T>
{
public:
    BVC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BVS", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             !this->cpu->getFlag(Ricoh2A03::V));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class BMI : public AddressingMode<T>
{
public:
    BMI(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BMI", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             this->cpu->getFlag(Ricoh2A03::N));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class BPL : public AddressingMode<T>
{
public:
    BPL(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BPL", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        int cyclePenalty = this->cpu->branch(this->absoluteAddress,
                                             !this->cpu->getFlag(Ricoh2A03::N));

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

// Increment/Decrement Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class INC : public AddressingMode<T>
{
public:
    INC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("INC", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        ++this->auxData;

        this->writeBack();
        SET_ZN(this->auxData)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class DEC : public AddressingMode<T>
{
public:
    DEC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("DEC", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        --this->auxData;

        this->writeBack();
        SET_ZN(this->auxData)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class INX : public AddressingMode<T>
{
public:
    INX(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("INX", cpu, numCycles) {}

    uint8_t exec() override final
    {
        ++this->cpu->X;
        SET_ZN(this->cpu->X)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class DEX : public AddressingMode<T>
{
public:
    DEX(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("DEX", cpu, numCycles) {}

    uint8_t exec() override final
    {
        --this->cpu->X;
        SET_ZN(this->cpu->X)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class INY : public AddressingMode<T>
{
public:
    INY(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("INY", cpu, numCycles) {}

    uint8_t exec() override final
    {
        ++this->cpu->Y;
        SET_ZN(this->cpu->Y)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class DEY : public AddressingMode<T>
{
public:
    DEY(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("DEY", cpu, numCycles) {}

    uint8_t exec() override final
    {
        --this->cpu->Y;
        SET_ZN(this->cpu->Y)

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Compare Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class CMP : public AddressingMode<T>
{
public:
    CMP(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("CMP", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        uint8_t result = this->cpu->A - this->auxData;

        SET_ZN(result)
        this->cpu->setFlag(Ricoh2A03::C, this->cpu->A >= this->auxData);

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class CPX : public AddressingMode<T>
{
public:
    CPX(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("CPX", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        uint8_t result = this->cpu->X - this->auxData;

        SET_ZN(result)
        this->cpu->setFlag(Ricoh2A03::C, this->cpu->X >= this->auxData);

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class CPY : public AddressingMode<T>
{
public:
    CPY(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("CPY", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        uint8_t result = this->cpu->Y - this->auxData;

        SET_ZN(result)
        this->cpu->setFlag(Ricoh2A03::C, this->cpu->Y >= this->auxData);

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class BIT : public AddressingMode<T>
{
public:
    BIT(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("BIT", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();

        this->cpu->setFlag(Ricoh2A03::N, this->auxData & 0x80);
        this->cpu->setFlag(Ricoh2A03::V, this->auxData & 0x40);

        this->auxData &= this->cpu->A;
        this->cpu->setFlag(Ricoh2A03::Z, this->auxData == 0x00);

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Set/Reset Flag Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class SEC : public AddressingMode<T>
{
public:
    SEC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("SEC", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::C, true);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class CLC : public AddressingMode<T>
{
public:
    CLC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("CLC", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::C, false);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class SED : public AddressingMode<T>
{
public:
    SED(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("SED", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::D, true);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class CLD : public AddressingMode<T>
{
public:
    CLD(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("CLD", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::D, false);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class SEI : public AddressingMode<T>
{
public:
    SEI(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("SEI", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::I, true);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class CLI : public AddressingMode<T>
{
public:
    CLI(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("CLI", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::I, false);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class CLV : public AddressingMode<T>
{
public:
    CLV(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("CLV", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->cpu->setFlag(Ricoh2A03::V, false);

#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Bitwise Logic Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class AND : public AddressingMode<T>
{
public:
    AND(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("AND", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        this->cpu->A &= this->auxData;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class ORA : public AddressingMode<T>
{
public:
    ORA(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("ORA", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        this->cpu->A |= this->auxData;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class EOR : public AddressingMode<T>
{
public:
    EOR(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("EOR", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        this->cpu->A ^= this->auxData;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class LSR : public AddressingMode<T>
{
public:
    LSR(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("LSR", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();

        this->cpu->setFlag(Ricoh2A03::C, this->auxData & 0x01);

        this->auxData >>= 1;
        this->writeBack();
        SET_ZN(this->auxData)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class ASL : public AddressingMode<T>
{
public:
    ASL(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("ASL", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        this->cpu->setFlag(Ricoh2A03::C, this->auxData & 0x80);

        this->auxData <<= 1;
        this->writeBack();
        SET_ZN(this->auxData)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class ROR : public AddressingMode<T>
{
public:
    ROR(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("ROR", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        uint8_t oldCarry = static_cast<uint8_t>(this->cpu->getFlag(Ricoh2A03::C)) << 7;
        this->cpu->setFlag(Ricoh2A03::C, this->auxData & 0x01);

        this->auxData = (this->auxData >> 1) | oldCarry;
        this->writeBack();
        SET_ZN(this->auxData)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class ROL : public AddressingMode<T>
{
public:
    ROL(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("ROL", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData();
        uint8_t oldCarry = static_cast<uint8_t>(this->cpu->getFlag(Ricoh2A03::C));
        this->cpu->setFlag(Ricoh2A03::C, this->auxData & 0x80);

        this->auxData = (this->auxData << 1) | oldCarry;
        this->writeBack();
        SET_ZN(this->auxData)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

// Arithmetic Instructions ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class ADC : public AddressingMode<T>
{
public:
    ADC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("ADC", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        uint16_t overflowCheck = this->cpu->A + this->auxData +
                                 static_cast<uint8_t>(this->cpu->getFlag(Ricoh2A03::C));

        this->cpu->setFlag(Ricoh2A03::C, overflowCheck & 0xFF00);
        this->cpu->setFlag(Ricoh2A03::V,
                           ~(this->cpu->A ^ this->auxData) &
                               (this->cpu->A ^ static_cast<uint8_t>(overflowCheck)) &
                               0x80);

        this->cpu->A = overflowCheck & 0xFF;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

template <Ricoh2A03::AddressingType T>
class SBC : public AddressingMode<T>
{
public:
    SBC(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("SDC", cpu, numCycles) {}

    uint8_t exec() override final
    {
        uint8_t cyclePenalty = this->fetchAuxData();
        this->auxData = ~this->auxData;

        uint16_t overflowCheck = this->cpu->A + this->auxData +
                                 static_cast<uint8_t>(this->cpu->getFlag(Ricoh2A03::C));

        this->cpu->setFlag(Ricoh2A03::C, overflowCheck & 0xFF00);
        this->cpu->setFlag(Ricoh2A03::V,
                           (this->auxData ^ static_cast<uint8_t>(overflowCheck)) &
                               (this->cpu->A ^ static_cast<uint8_t>(overflowCheck)) &
                               0x80);

        this->cpu->A = overflowCheck & 0xFF;
        SET_ZN(this->cpu->A)

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles + cyclePenalty;
    }
};

// MISC ------------------------------------------------------
template <Ricoh2A03::AddressingType T>
class JMP : public AddressingMode<T>
{
public:
    JMP(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("JMP", cpu, numCycles) {}

    uint8_t exec() override final
    {
        this->fetchAuxData(false);
        this->cpu->PC = this->absoluteAddress;

#ifdef DUMP
        AddressingMode<T>::instrDump();
#endif
        return this->numCycles;
    }
};

template <Ricoh2A03::AddressingType T>
class NOP : public AddressingMode<T>
{
public:
    NOP(Ricoh2A03 *cpu, uint8_t numCycles)
        : AddressingMode<T>("NOP", cpu, numCycles) {}

    uint8_t exec() override final
    {
#ifdef DUMP
        this->oldPC = this->cpu->PC - 0x1;
        AddressingMode<T>::instrDump();
#endif

        return this->numCycles;
    }
};