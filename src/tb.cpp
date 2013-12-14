#include <tb.hpp>
#include <debug.hpp>
#include <utility.hpp>

#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCDisassembler.h>
#include <llvm/MC/MCSubtargetInfo.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/StringRefMemoryObject.h>

#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/Triple.h>

namespace pa
{

    using namespace llvm;

    void tb::push_back(llvm::MCInst const & inst, size_t inst_size)
    {
        instructions_.push_back(inst);
        length_ += inst_size;
    }

    tb::const_iterator tb::begin() const
    { return std::begin(instructions_); }

    tb::const_iterator tb::end() const
    { return std::end(instructions_); }

    bool tb::empty() const
    { return instructions_.empty(); }

    size_t tb::size() const
    { return instructions_.size(); }

    void tb::swap(tb &other)
    {
        using std::swap;
        swap(addr_, other.addr_);
        swap(length_, other.length_);
        swap(instructions_, other.instructions_);
    }

    uint64_t tb::address() const
    { return addr_; }

    void tb::set_address(uint64_t addr)
    { addr_ = addr; }

    uint64_t tb::bytes() const
    { return length_; }

    OwningPtr<tb const> create_tb(std::string const &triple, Target const * target, MemoryObject const &bytes, uint64_t addr)
    {
        OwningPtr<const MCRegisterInfo> reginfo(target->createMCRegInfo(triple));
        if (!reginfo)
        {
            warn("no register info for ", triple);
            return OwningPtr<tb const>(nullptr);
        }

        OwningPtr<const MCInstrInfo> instrinfo(target->createMCInstrInfo());
        if (!instrinfo)
        {
            warn("no instruction info for ", triple);
            return OwningPtr<tb const>(nullptr);
        }

        OwningPtr<const MCSubtargetInfo> subtargetinfo(target->createMCSubtargetInfo(triple, "", ""));
        if (!subtargetinfo)
        {
            warn("no subtarget info for ", triple);
            return OwningPtr<tb const>(nullptr);
        }

        OwningPtr<const MCDisassembler> disassembler(target->createMCDisassembler(*subtargetinfo));
        if (!disassembler)
        {
            warn("no disassembler for ", triple);
            return OwningPtr<tb const>(nullptr);
        }

        OwningPtr<tb> block(new tb(addr));
        if (!block)
        {
            warn("cannot allocate translation block");
            return OwningPtr<tb const>(nullptr);
        }

        MCInst inst;
        uint64_t instr_size = 0u;
        uint64_t const lower = bytes.getBase();
        uint64_t const upper = lower + bytes.getExtent();
        for (uint64_t it = lower; it < upper; it += instr_size)
        {
            if (disassembler->getInstruction(inst, instr_size, bytes, it, nulls(), nulls()) != MCDisassembler::Success)
            {
                warn("cannot parse instruction [", hex_format(bytes, it, 16u), "]");
                return OwningPtr<tb const>(nullptr);
            }
            block->push_back(inst, instr_size);
            if (instrinfo->get(inst.getOpcode()).mayAffectControlFlow(inst, *reginfo))
                break;
        }

        return OwningPtr<tb const>(block.take());
    }

    OwningPtr<tb const> create_tb(std::string const &triple, char const * bytes, size_t size, uint64_t addr)
    {
        std::string err;
        Target const *target = TargetRegistry::lookupTarget(triple, err);
        if (!target)
        {
            warn(err);
            return OwningPtr<tb const>(nullptr);
        }

        return create_tb(triple, target, StringRefMemoryObject(StringRef(bytes, size)), addr);
    }

    OwningPtr<tb const> create_tb(std::string const &triple, MemoryObject const &bytes, uint64_t addr)
    {
        std::string err;
        Target const *target = TargetRegistry::lookupTarget(triple, err);
        if (!target)
        {
            warn(err);
            return OwningPtr<tb const>(nullptr);
        }

        return create_tb(triple, target, bytes, addr);
    }

} /*namespace pa*/
