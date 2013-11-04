#include <tb.hpp>
#include <debug.hpp>
#include <utility.hpp>

#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCDisassembler.h>
#include <llvm/MC/MCSubtargetInfo.h>

#include <llvm/Support/raw_ostream.h>

namespace pa
{

    using namespace llvm;

    tb::tb(Target const * const target) : target_(target) {}

    void tb::push_back(llvm::MCInst const &inst)
    { instructions_.push_back(inst); }

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
        swap(target_, other.target_);
        swap(instructions_, other.instructions_);
    }

    Target const * tb::get_target() const
    { return target_; }

    OwningPtr<tb const> create_tb(std::string const &triple, MemoryObject const &bytes, uint64_t & size, uint64_t offset)
    {
        std::string err;
        Target const *target = TargetRegistry::lookupTarget(triple, err);
        if (!target)
        {
            warn(err);
            return OwningPtr<tb const>(nullptr);
        }

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

        OwningPtr<tb> block(new tb(target));
        if (!block)
        {
            warn("cannot allocate translation block");
            return OwningPtr<tb const>(nullptr);
        }

        uint64_t instr_size = 0u, block_size = 0u;
        uint64_t const lower = bytes.getBase();
        uint64_t const upper = lower + bytes.getExtent();
        for (uint64_t it = lower + offset; it < upper; it += instr_size)
        {
            MCInst inst;
            if (disassembler->getInstruction(inst, instr_size, bytes, it, nulls(), nulls()) != MCDisassembler::Success)
            {
                warn("cannot parse instruction [", hex_format(bytes, it, 16u), "]");
                return OwningPtr<tb const>(nullptr);
            }
            block->push_back(inst);
            block_size += instr_size;
            if (instrinfo->get(inst.getOpcode()).mayAffectControlFlow(inst, *reginfo))
                break;
        }
        size = block_size;
        return OwningPtr<tb const>(block.take());
    }

} /*namespace pa*/
