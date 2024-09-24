#include<stdint.h>
#include "Bus.h"
class c6502{
    Bus &bus_;
    uint8_t regA, regX, regY;
    uint8_t regStackPointer;
    uint8_t regStatus;
    uint8_t regProgCounterHigh, regProgCounterLow;
    uint8_t currentopcode;

    enum class CC{
        Carry,
        Zero,
        IntMask,
        Decimal,
        NA,
        NA2,
        Overflow,
        Negative
    };

public:
    explicit c6502(Bus &bus): bus_(bus) {}

    void runCpu();
private:
    void handleInstruction();
    void advance_pc();
    Addr pc() const;

    uint8_t ccGet(CC cc) const;
    void ccSet(CC cc, bool value);

    Addr addrmode_immediate();
    Addr addrmode_zp();
    Addr addrmode_abs();
    Addr addrmode_stack();

    void op_lda(Addr addr);
    void op_sta(Addr addr);
    void op_rol(Addr addr);
    void op_pha(Addr addr);
    void op_pla(Addr addr);
};


