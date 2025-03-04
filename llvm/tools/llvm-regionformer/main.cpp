#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>

int main() {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();

    std::string TargetTriple = "x86_64-unknown-linux-gnu";
    std::string Error;
    const llvm::Target *TheTarget = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    if (!TheTarget) {
        llvm::errs() << Error;
        return 1;
    }

    llvm::MCRegisterInfo *MRI = TheTarget->createMCRegInfo(TargetTriple);
    llvm::MCTargetOptions MCOptions;
    llvm::MCAsmInfo *MAI = TheTarget->createMCAsmInfo(*MRI, TargetTriple, MCOptions);
    llvm::MCSubtargetInfo *STI = TheTarget->createMCSubtargetInfo(TargetTriple, "", "");
    llvm::MCInstrInfo *MII = TheTarget->createMCInstrInfo();
    llvm::MCTargetOptions TargetOpts;
    llvm::MCContext Ctx(llvm::Triple(TargetTriple), MAI, MRI, STI, nullptr, &TargetOpts);
    llvm::MCDisassembler *DisAsm = TheTarget->createMCDisassembler(*STI, Ctx);

    if (!DisAsm) {
        llvm::errs() << "Failed to create disassembler\n";
        return 1;
    }

    // Load your binary code into a MemoryBuffer
    auto Buffer = llvm::MemoryBuffer::getFile("/nfs/pdx/home/yubing/548_x/548");
    if (!Buffer) {
        llvm::errs() << "Failed to open binary file\n";
        return 1;
    }

    llvm::ArrayRef<uint8_t> Data(reinterpret_cast<const uint8_t *>(Buffer->get()->getBufferStart()), Buffer->get()->getBufferSize());
    // uint64_t Address = 0x41b180; // Starting address of the basic block

    // function's real offset in binary = function address -  text section 's address + text's offset
    // Define the addresses and offsets
    uint64_t FunctionAddress = 0x41b180; // Example function address
    uint64_t TextSectionAddress = 0x401140; // Example .text section address
    uint64_t TextSectionOffset = 0x1140; // Example .text section offset

    // Calculate the real offset
    uint64_t RealOffset = FunctionAddress - TextSectionAddress + TextSectionOffset;

    uint64_t Address = RealOffset; // Starting address of the basic block
    while (Address < 0x41c5fd -0x401140+ 0x1140 ) {
        llvm::MCInst Inst;
        uint64_t Size;
        llvm::MCDisassembler::DecodeStatus S = DisAsm->getInstruction(Inst, Size, Data.slice(Address), 0, llvm::nulls());

        if (S == llvm::MCDisassembler::Success) {
            // Print the instruction with opcode name
            llvm::outs() << llvm::format_hex(Address, 8) << ":\t";
            llvm::outs() << MII->getName(Inst.getOpcode()) << "\t";
            Inst.dump_pretty(llvm::outs());
            llvm::outs() << "\n";
            Address += Size;
        } else {
            llvm::errs() << "Failed to disassemble instruction at address " << Address << "\n";
            break;
        }
    }

    return 0;
}

