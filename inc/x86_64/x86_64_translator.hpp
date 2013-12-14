#ifndef __PA_INC_X86_64_TRANSLATOR_HPP__
#define __PA_INC_X86_64_TRANSLATOR_HPP__

#include <translator.hpp>

#include <unordered_map>
#include <string>

namespace llvm
{
    class Value;
}

namespace pa
{
namespace x86_64
{

    enum register_index {
        NO_REG = 0,
        /* 64 bit */
        REG_RAX, REG_RBX, REG_RCX, REG_RDX, REG_RSI, REG_RDI, REG_RBP, REG_RSP,
        REG_R8, REG_R9, REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_R15,
        /* 32 bit */
        REG_EAX, REG_EBX, REG_ECX, REG_EDX, REG_ESI, REG_EDI, REG_EBP, REG_ESP,
        REG_R8D, REG_R9D, REG_R10D, REG_R11D, REG_R12D, REG_R13D, REG_R14D, REG_R15D,
        /* 16 bit */
        REG_AX, REG_BX, REG_CX, REG_DX, REG_SI, REG_DI, REG_BP, REG_SP,
        REG_R8W, REG_R9W, REG_R10W, REG_R11W, REG_R12W, REG_R13W, REG_R14W, REG_R15W,
        /* 8 bit */
        REG_AH, REG_BH, REG_CH, REG_DH,
        REG_AL, REG_BL, REG_CL, REG_DL, REG_SIL, REG_DIL, REG_BPL, REG_SPL,
        REG_R8B, REG_R9B, REG_R10B, REG_R11B, REG_R12B, REG_R13B, REG_R14B, REG_R15B,

        REG_SRC, REG_DST, REG_CMP,

        MAX_REG_IDX
    };

    struct x86_64_registers
    {
        uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp;
        uint64_t r8,  r9,  r10, r11, r12, r13, r14, r15;

        uint64_t src, dst, op; /* lazy eflags evaluation */
    };

    enum compare_operation
    { OP_TEST, OP_CMP };

} /* namespace x86_64 */
} /* namespace pa */

namespace std
{

    template <>
    struct hash<pa::x86_64::register_index>
    {
        inline size_t operator()(pa::x86_64::register_index index) const
        { return static_cast<size_t>(index); }
    };

} /* namespace std */

namespace pa
{
namespace x86_64
{

    typedef std::unordered_map< register_index, std::pair<llvm::Value *, bool> > loaded_registers;

    class x86_64_translator : public translator
    {
    public:
        x86_64_translator(llvm::Triple const & triple, llvm::Target const * const target)
            : translator(triple, target)
            , regs_(nullptr)
        { }

        virtual void translate(tb const * const tb, llvm::IRBuilder<> & builder) override;

        void set_registers_block(x86_64_registers * regs) { regs_ = regs; }
        x86_64_registers * get_registers_block() { return regs_; }

        static bool classof(translator const * const tr)
        { return tr->get_triple().getArch() == llvm::Triple::x86_64; }

    private:
        x86_64_registers * regs_;

        void gen_mov_rr(llvm::MCInst const &, llvm::IRBuilder<> &, loaded_registers &) const;
        void gen_add_rr(llvm::MCInst const &, llvm::IRBuilder<> &, loaded_registers &) const;
        void gen_test_rr(llvm::MCInst const &, llvm::IRBuilder<> &, loaded_registers &) const;

        uint64_t get_register_ptr(register_index reg) const;
        llvm::Value * const load_register_value(register_index reg, llvm::IRBuilder<> & builder, loaded_registers & regset) const;
        void store_register_value(register_index reg, llvm::Value * val, llvm::IRBuilder<> & builder, loaded_registers & regset) const;
        void gen_store_registers(llvm::IRBuilder<> & builder, loaded_registers & regset) const;
        llvm::Value * const gen_load_register(register_index reg, llvm::IRBuilder<> & builder) const;
        void gen_store_register(register_index reg, llvm::Value * val, llvm::IRBuilder<> & builder) const;
    };

} /* namespace x86_64 */
} /* namespace pa */

#endif /*__PA_INC_X86_64_TRANSLATOR_HPP__*/
