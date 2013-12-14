#ifndef __PA_INC_TRANSLATOR_HPP__
#define __PA_INC_TRANSLATOR_HPP__

#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/ADT/Triple.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/Support/TargetRegistry.h>

#include <string>

namespace pa
{

    class tb;

    class translator
    {
    public:
        translator(llvm::Triple const & triple, llvm::Target const * const target)
            : triple_(triple)
            , target_(target)
            , instr_info_(target_->createMCInstrInfo())
            , register_info_(target_->createMCRegInfo(triple_.str()))
        { }

        virtual ~translator() { }

        virtual void translate(tb const * const tb, llvm::IRBuilder<> & builder) = 0;

        llvm::Triple const & get_triple() const { return triple_; }
        llvm::Target const * const get_target() const { return target_; }
        llvm::MCInstrInfo const * const get_instr_info() const { return instr_info_.get(); }
        llvm::MCRegisterInfo const * const get_register_info() const { return register_info_.get(); }

    private:
        llvm::Triple const triple_;
        llvm::Target const * const target_;

        llvm::OwningPtr<const llvm::MCInstrInfo> instr_info_;
        llvm::OwningPtr<const llvm::MCRegisterInfo> register_info_;

        translator(translator const &) = delete;
        translator & operator=(translator const &) = delete;
    };

    template <llvm::Triple::ArchType ARCH>
    llvm::OwningPtr<translator> translator_factory(llvm::Triple const &, llvm::Target const * const);

    llvm::OwningPtr<translator> create_translator(llvm::Triple::ArchType type);
    llvm::OwningPtr<translator> create_translator(llvm::Triple const & triple);

} /* namespace pa */

#endif /*__PA_INC_TRANSLATOR_HPP__*/
