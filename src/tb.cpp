#include <tb.hpp>
#include <utility.hpp>

#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCDisassembler.h>
#include <llvm/ADT/OwningPtr.h>

#include <llvm/Support/raw_ostream.h>

#include <string>

namespace pa
{

    using namespace llvm;

    tb::tb(Target const * const target, uintptr_t offset) : target_(target), offset_(offset) {}

    void tb::push_back(llvm::MCInst const &inst)
    { instructions_.push_back(inst); }

    tb::const_iterator tb::begin() const
    { return instructions_.begin(); }

    tb::const_iterator tb::end() const
    { return instructions_.end(); }

    bool tb::empty() const
    { return instructions_.empty(); }

    size_t tb::size() const
    { return instructions_.size(); }

    size_t tb::bytes() const
    {
        OwningPtr<const MCInstrInfo> mcinfo(target_->createMCInstrInfo());
        size_t sz = 0u;
        for (tb::const_iterator it(begin()); it != end(); ++it)
        { sz += mcinfo->get(it->getOpcode()).getSize(); }
        return sz;
    }

    void tb::swap(tb &other)
    {
        using std::swap;
        swap(target_, other.target_);
        swap(offset_, other.offset_);
        swap(instructions_, other.instructions_);
    }

    uintptr_t tb::get_offset() const
    { return offset_; }

    Target const * tb::get_target() const
    { return target_; }

    tb * create_tb(std::string const &triple, MemoryObject const &bytes, uint64_t base, uint64_t offset)
    {
        std::string err;
        Target const *target = TargetRegistry::lookupTarget(triple, err);
        if (!target)
        {
            errs() << __func__ << ": " << err;
            return nullptr;
        }

        OwningPtr<const MCInstrInfo> instrinfo(target->createMCInstrInfo());
        if (!instrinfo)
        {
            errs() << __func__ << ": no instruction info for " << triple;
            return nullptr;
        }

        OwningPtr<const MCSubtargetInfo> subtargetinfo(target->createMCSubtargetInfo(triple, "", ""));
        if (!subtargetinfo)
        {
            errs() << __func__ << ": no subtarget info for " << triple;
            return nullptr;
        }

        OwningPtr<const MCDisassembler> disassembler(target->createMCDisassembler(*subtargetinfo));
        if (!disassembler)
        {
            errs() << __func__ << ": no disassembler for " << triple;
            return nullptr;
        }

        OwningPtr<tb> block(new tb(target, base + offset));
        if (!block)
        {
            errs() << __func__ << ": cannot allocate translation block";
            return nullptr;
        }

        uint64_t size = 0u;
        uint64_t const lower = bytes.getBase();
        uint64_t const upper = lower + bytes.getExtent();
        for (uint64_t it = lower + offset; it < upper; it += size)
        {
            MCInst inst;
            if (disassembler->getInstruction(inst, size, bytes, it, nulls(), nulls()) != MCDisassembler::Success)
            {
                errs() << __func__ << ": cannot parse instruction [" << hex_format(bytes, it, 16u) << "]";
                return nullptr;
            }
            block->push_back(inst);
            if (instrinfo->get(inst.getOpcode()).isTerminator())
                break;
        }
        return block.take();
    }

} /*namespace pa*/
