#include "c6502.h"
static Addr  compose(uint8_t high, uint8_t low){
    return high<<8 | low;
}

void c6502::runCpu(){
    while(true){
        handleInstruction();
    }
}

void c6502::handleInstruction(){
    currentopcode= bus_.read(compose(regProgCounterHigh, regProgCounterLow), true);
    advance_pc();
    switch(currentopcode){
        case 0xa9:
            op_lda(addrmode_immediate());
            break;
        case 0xa5:
            op_lda(addrmode_zp());
            break;
        case 0x85:
            op_sta(addrmode_zp());
            break;
        case 0x26:
            op_rol(addrmode_zp());
            break;
        case 0x2e:
            op_rol(addrmode_abs());
            break;
        case 0xad:
            op_lda(addrmode_abs());
            break;
        case 0x48:
            op_pha(addrmode_stack());
            break;
        case 0x68:
            op_pla(addrmode_stack());
            break;
    }
}

void c6502:: advance_pc(){
    Addr pc_ = compose(regProgCounterHigh, regProgCounterLow);
    pc_++;
    regProgCounterHigh=pc_ >>8;
    regProgCounterLow= pc_ & 0xff;
}

Addr c6502::pc() const{
    return compose(regProgCounterHigh, regProgCounterLow);
}

uint8_t c6502::ccGet(CC cc) const{
    return bool(regStatus & (1<<int(cc)));
}
void c6502::ccSet(CC cc, bool value){
    if(value){
        regStatus |= 1<<int(cc);
    }
    else{
        regStatus &=-( 1<<int(cc));
    }
}
//address modes
Addr c6502::addrmode_abs(){
    Addr res=bus_.read(pc());
    advance_pc();
    res |= bus_.read(pc())<<8;
    advance_pc();
    return res;
}

Addr c6502::addrmode_immediate(){
    Addr stored_pc= pc();
    advance_pc();
    return stored_pc;
}

Addr c6502::addrmode_stack(){
    bus_.read(pc());
    return compose(0x01, regStackPointer);
}

Addr  c6502::addrmode_zp(){
    Addr pc_= pc();
    uint8_t addr = bus_.read(pc_);
    advance_pc();
    return addr;
}
//
void c6502::op_lda(Addr addr){
    regA=bus_.read(addr);
}

void c6502::op_pla(Addr addr){
    bus_.read(addr);
    regStackPointer++;
    regA=bus_.read(compose(0x01, regStackPointer));
    ccSet(CC::Negative, regA & 0x80);
    ccSet(CC::Zero, regA==0);
}

void c6502::op_sta(Addr addr){
    regA=bus_.read(addr);
    bus_.write(addr, regA);
}
void c6502::op_rol(Addr addr){
    uint16_t value= bus_.read(addr);
    bus_.write(addr, value);
    value <<=1;
    value |=ccGet(CC::Carry);
    ccSet(CC::Carry, value & 0x100);
    ccSet(CC::Negative, value & 0x80);
    ccSet(CC::Zero, (value & 0xff)==0);

    bus_.write(addr, value);
}
void c6502::op_pha(Addr addr){
    bus_.write(addr, regA);
    regStackPointer--;
}
