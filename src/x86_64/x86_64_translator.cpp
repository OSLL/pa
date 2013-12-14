#include <debug.hpp>
#include <tb.hpp>
#include <x86_64/x86_64_translator.hpp>

#include <vector>

namespace pa
{

    using namespace llvm;

    template <>
    OwningPtr<translator> translator_factory<Triple::x86_64>(Triple const & triple, Target const * const target)
    { return OwningPtr<translator>(new x86_64::x86_64_translator(triple, target)); }

namespace x86_64
{

    static std::vector< std::vector<register_index> > physical_register{
        {NO_REG,  NO_REG,   NO_REG,   NO_REG, NO_REG},

        {REG_RAX, REG_EAX,  REG_AX,   REG_AH, REG_AL},
        {REG_RBX, REG_EBX,  REG_BX,   REG_BH, REG_BL},
        {REG_RCX, REG_ECX,  REG_CX,   REG_CH, REG_CL},
        {REG_RDX, REG_EDX,  REG_DX,   REG_DH, REG_DL},
        {REG_RSI, REG_ESI,  REG_SI,   NO_REG, REG_SIL},
        {REG_RDI, REG_EDI,  REG_DI,   NO_REG, REG_DIL},
        {REG_RBP, REG_EBP,  REG_BP,   NO_REG, REG_BPL},
        {REG_RSP, REG_ESP,  REG_SP,   NO_REG, REG_SPL},
        {REG_R8,  REG_R8D,  REG_R8W,  NO_REG, REG_R8B},
        {REG_R9,  REG_R9D,  REG_R9W,  NO_REG, REG_R9B},
        {REG_R10, REG_R10D, REG_R10W, NO_REG, REG_R10B},
        {REG_R11, REG_R11D, REG_R11W, NO_REG, REG_R11B},
        {REG_R12, REG_R12D, REG_R12W, NO_REG, REG_R12B},
        {REG_R13, REG_R13D, REG_R13W, NO_REG, REG_R13B},
        {REG_R14, REG_R14D, REG_R14W, NO_REG, REG_R14B},
        {REG_R15, REG_R15D, REG_R15W, NO_REG, REG_R15B},

        {REG_RAX, REG_EAX,  REG_AX,   REG_AH, REG_AL},
        {REG_RBX, REG_EBX,  REG_BX,   REG_BH, REG_BL},
        {REG_RCX, REG_ECX,  REG_CX,   REG_CH, REG_CL},
        {REG_RDX, REG_EDX,  REG_DX,   REG_DH, REG_DL},
        {REG_RSI, REG_ESI,  REG_SI,   NO_REG, REG_SIL},
        {REG_RDI, REG_EDI,  REG_DI,   NO_REG, REG_DIL},
        {REG_RBP, REG_EBP,  REG_BP,   NO_REG, REG_BPL},
        {REG_RSP, REG_ESP,  REG_SP,   NO_REG, REG_SPL},
        {REG_R8,  REG_R8D,  REG_R8W,  NO_REG, REG_R8B},
        {REG_R9,  REG_R9D,  REG_R9W,  NO_REG, REG_R9B},
        {REG_R10, REG_R10D, REG_R10W, NO_REG, REG_R10B},
        {REG_R11, REG_R11D, REG_R11W, NO_REG, REG_R11B},
        {REG_R12, REG_R12D, REG_R12W, NO_REG, REG_R12B},
        {REG_R13, REG_R13D, REG_R13W, NO_REG, REG_R13B},
        {REG_R14, REG_R14D, REG_R14W, NO_REG, REG_R14B},
        {REG_R15, REG_R15D, REG_R15W, NO_REG, REG_R15B},

        {REG_RAX, REG_EAX,  REG_AX,   REG_AH, REG_AL},
        {REG_RBX, REG_EBX,  REG_BX,   REG_BH, REG_BL},
        {REG_RCX, REG_ECX,  REG_CX,   REG_CH, REG_CL},
        {REG_RDX, REG_EDX,  REG_DX,   REG_DH, REG_DL},
        {REG_RSI, REG_ESI,  REG_SI,   NO_REG, REG_SIL},
        {REG_RDI, REG_EDI,  REG_DI,   NO_REG, REG_DIL},
        {REG_RBP, REG_EBP,  REG_BP,   NO_REG, REG_BPL},
        {REG_RSP, REG_ESP,  REG_SP,   NO_REG, REG_SPL},
        {REG_R8,  REG_R8D,  REG_R8W,  NO_REG, REG_R8B},
        {REG_R9,  REG_R9D,  REG_R9W,  NO_REG, REG_R9B},
        {REG_R10, REG_R10D, REG_R10W, NO_REG, REG_R10B},
        {REG_R11, REG_R11D, REG_R11W, NO_REG, REG_R11B},
        {REG_R12, REG_R12D, REG_R12W, NO_REG, REG_R12B},
        {REG_R13, REG_R13D, REG_R13W, NO_REG, REG_R13B},
        {REG_R14, REG_R14D, REG_R14W, NO_REG, REG_R14B},
        {REG_R15, REG_R15D, REG_R15W, NO_REG, REG_R15B},

        {REG_RAX, REG_EAX,  REG_AX,   REG_AH, REG_AL},
        {REG_RBX, REG_EBX,  REG_BX,   REG_BH, REG_BL},
        {REG_RCX, REG_ECX,  REG_CX,   REG_CH, REG_CL},
        {REG_RDX, REG_EDX,  REG_DX,   REG_DH, REG_DL},

        {REG_RAX, REG_EAX,  REG_AX,   REG_AH, REG_AL},
        {REG_RBX, REG_EBX,  REG_BX,   REG_BH, REG_BL},
        {REG_RCX, REG_ECX,  REG_CX,   REG_CH, REG_CL},
        {REG_RDX, REG_EDX,  REG_DX,   REG_DH, REG_DL},
        {REG_RSI, REG_ESI,  REG_SI,   NO_REG, REG_SIL},
        {REG_RDI, REG_EDI,  REG_DI,   NO_REG, REG_DIL},
        {REG_RBP, REG_EBP,  REG_BP,   NO_REG, REG_BPL},
        {REG_RSP, REG_ESP,  REG_SP,   NO_REG, REG_SPL},
        {REG_R8,  REG_R8D,  REG_R8W,  NO_REG, REG_R8B},
        {REG_R9,  REG_R9D,  REG_R9W,  NO_REG, REG_R9B},
        {REG_R10, REG_R10D, REG_R10W, NO_REG, REG_R10B},
        {REG_R11, REG_R11D, REG_R11W, NO_REG, REG_R11B},
        {REG_R12, REG_R12D, REG_R12W, NO_REG, REG_R12B},
        {REG_R13, REG_R13D, REG_R13W, NO_REG, REG_R13B},
        {REG_R14, REG_R14D, REG_R14W, NO_REG, REG_R14B},
        {REG_R15, REG_R15D, REG_R15W, NO_REG, REG_R15B},
    };

    static std::unordered_map<std::string, register_index> register_index_map{
        {"", NO_REG},
        /* 64 bit */
        {"RAX", REG_RAX},
        {"RBX", REG_RBX},
        {"RCX", REG_RCX},
        {"RDX", REG_RDX},
        {"RSI", REG_RSI},
        {"RDI", REG_RDI},
        {"RBP", REG_RBP},
        {"RSP", REG_RSP},
        {"R8", REG_R8},
        {"R9", REG_R9},
        {"R10", REG_R10},
        {"R11", REG_R11},
        {"R12", REG_R12},
        {"R13", REG_R13},
        {"R14", REG_R14},
        {"R15", REG_R15},
        /* 32 bit */
        {"EAX", REG_EAX},
        {"EBX", REG_EBX},
        {"ECX", REG_ECX},
        {"EDX", REG_EDX},
        {"ESI", REG_ESI},
        {"EDI", REG_EDI},
        {"EBP", REG_EBP},
        {"ESP", REG_ESP},
        {"R8D", REG_R8D},
        {"R9D", REG_R9D},
        {"R10D", REG_R10D},
        {"R11D", REG_R11D},
        {"R12D", REG_R12D},
        {"R13D", REG_R13D},
        {"R14D", REG_R14D},
        {"R15D", REG_R15D},
        /* 16 bit */
        {"AX", REG_AX},
        {"BX", REG_BX},
        {"CX", REG_CX},
        {"DX", REG_DX},
        {"SI", REG_SI},
        {"DI", REG_DI},
        {"BP", REG_BP},
        {"SP", REG_SP},
        {"R8W", REG_R8W},
        {"R9W", REG_R9W},
        {"R10W", REG_R10W},
        {"R11W", REG_R11W},
        {"R12W", REG_R12W},
        {"R13W", REG_R13W},
        {"R14W", REG_R14W},
        {"R15W", REG_R15W},
        /* 8 bit */
        {"AH", REG_AH},
        {"BH", REG_BH},
        {"CH", REG_CH},
        {"DH", REG_DH},
        {"AL", REG_AL},
        {"BL", REG_BL},
        {"CL", REG_CL},
        {"DL", REG_DL},
        {"SIL", REG_SIL},
        {"DIL", REG_DIL},
        {"BPL", REG_BPL},
        {"SPL", REG_SPL},
        {"R8B", REG_R8B},
        {"R9B", REG_R9B},
        {"R10B", REG_R10B},
        {"R11B", REG_R11B},
        {"R12B", REG_R12B},
        {"R13B", REG_R13B},
        {"R14B", REG_R14B},
        {"R15B", REG_R15B},
    };

    static register_index get_register_index(std::string const & name)
    {
        std::unordered_map< std::string, register_index >::const_iterator it(register_index_map.find(name));
        if (it != std::end(register_index_map))
            return it->second;
        return NO_REG;
    }

    static register_index get_super_register(register_index index)
    {
        if (index <= NO_REG || index >= MAX_REG_IDX) return NO_REG;

        std::vector<register_index> const & descr = physical_register.at(index);
        for (size_t off = 0; off != 5; ++off)
            if (descr.at(off) != NO_REG) return descr.at(off);

        return NO_REG;
    }

    static size_t get_register_offset(register_index index)
    {
        if (index <= NO_REG || index >= MAX_REG_IDX) return 5;

        std::vector<register_index> const & descr = physical_register.at(index);
        for (size_t off = 0; off != 5; ++off)
            if (descr.at(off) == index) return off;

        return 5;
    }

    static uint64_t get_width_by_offset(size_t index)
    {
        switch (index)
        {
        case 0: return 64;
        case 1: return 32;
        case 2: return 16;
        case 3: return 8;
        case 4: return 8;
        default: warn("wrong index ", index);
        }
        return 0;
    }

    static uint64_t get_shift_by_offset(size_t index)
    {
        switch (index)
        {
        case 0: return 0;
        case 1: return 0;
        case 2: return 0;
        case 3: return 8;
        case 4: return 0;
        default: warn("wrong index ", index);
        }
        return 64;
    }

    static uint64_t get_mask_by_offset(size_t index)
    {
        switch (index)
        {
        case 0: return 0xffffffffffffffffull;
        case 1: return 0x00000000ffffffffull;
        case 2: return 0x000000000000ffffull;
        case 3: return 0x000000000000ff00ull;
        case 4: return 0x00000000000000ffull;
        default: warn("wrong index ", index);
        }
        return 0ull;
    }

    uint64_t x86_64_translator::get_register_ptr(register_index reg) const
    {
        uint64_t const * regptr = nullptr;

        switch (reg)
        {
        case REG_RAX:
            regptr = &(regs_->rax);
            break;
        case REG_RBX:
            regptr = &(regs_->rbx);
            break;
        case REG_RCX:
            regptr = &(regs_->rcx);
            break;
        case REG_RDX:
            regptr = &(regs_->rdx);
            break;
        case REG_RSI:
            regptr = &(regs_->rsi);
            break;
        case REG_RDI:
            regptr = &(regs_->rdi);
            break;
        case REG_RBP:
            regptr = &(regs_->rbp);
            break;
        case REG_RSP:
            regptr = &(regs_->rsp);
            break;
        case REG_R8:
            regptr = &(regs_->r8);
            break;
        case REG_R9:
            regptr = &(regs_->r9);
            break;
        case REG_R10:
            regptr = &(regs_->r10);
            break;
        case REG_R11:
            regptr = &(regs_->r11);
            break;
        case REG_R12:
            regptr = &(regs_->r12);
            break;
        case REG_R13:
            regptr = &(regs_->r13);
            break;
        case REG_R14:
            regptr = &(regs_->r14);
            break;
        case REG_R15:
            regptr = &(regs_->r15);
            break;
        default:
            warn("unknown register ", reg);
            break;
        };
        return reinterpret_cast<uint64_t>(regptr);
    }

    Value * const x86_64_translator::load_register_value(register_index reg, IRBuilder<> & builder, loaded_registers & regset) const
    {
        register_index const super = get_super_register(reg);
        if (regset.find(super) == std::end(regset))
            regset[super] = std::make_pair(gen_load_register(super, builder), false);
        Value * const super_value = regset[super].first;

        if (super != reg)
        {
            size_t const offset = get_register_offset(reg);
            Value * const mask = ConstantInt::get(builder.getInt64Ty(), get_mask_by_offset(offset));
            Value * const shift = ConstantInt::get(builder.getInt64Ty(), get_shift_by_offset(offset));
            Value * const masked = builder.CreateAnd(super_value, mask);
            Value * const shifted = builder.CreateLShr(masked, shift);
            switch (get_width_by_offset(offset))
            {
            case 32: return builder.CreateIntCast(shifted, builder.getInt32Ty(), false);
            case 16: return builder.CreateIntCast(shifted, builder.getInt16Ty(), false);
            case 8: return builder.CreateIntCast(shifted, builder.getInt8Ty(), false);
            }
            return nullptr;
        }

        return super_value;
    }

    void x86_64_translator::store_register_value(register_index reg, Value * val, IRBuilder<> & builder, loaded_registers & regset) const
    {
        register_index const super = get_super_register(reg);
        if (super == reg)
        {
            regset[super] = std::make_pair(builder.CreateIntCast(val, builder.getInt64Ty(), false), true);
            return;
        }

        if (regset.find(super) == std::end(regset))
            regset[super] = std::make_pair(gen_load_register(super, builder), false);

        Value * const super_value = regset[super].first;

        size_t const offset = get_register_offset(reg);
        Value * const mask = ConstantInt::get(builder.getInt64Ty(), ~get_mask_by_offset(offset));
        Value * const masked = builder.CreateAnd(super_value, mask);

        Value * const shift = ConstantInt::get(builder.getInt64Ty(), get_shift_by_offset(offset));
        Value * const casted = builder.CreateIntCast(val, builder.getInt64Ty(), false);
        Value * const shifted = builder.CreateShl(casted, shift);

        regset[super] = std::make_pair(builder.CreateOr(masked, shifted), true);
    }

    void x86_64_translator::translate(tb const * const tb, IRBuilder<> & builder)
    {
        loaded_registers regset;
        tb::const_iterator beg(tb->begin()), end(tb->end());
        for (tb::const_iterator it(beg); it != end; ++it)
        {
            switch (it->getOpcode())
            {
            case 0x05DB: /* mov64rr */
            case 0x05CC: /* mov32rr */
                gen_mov_rr(*it, builder, regset);
                break;
            case 0x0064: /* add64rr */
            case 0x0058: /* add32rr */
                gen_add_rr(*it, builder, regset);
                break;
            default:
                warn("unknown instruction ", get_instr_info()->getName(it->getOpcode()), "[", it->getOpcode(), "]");
                break;
            }
        }
        gen_store_registers(builder, regset);
    }

    void x86_64_translator::gen_mov_rr(MCInst const & inst, IRBuilder<> & builder, loaded_registers & regset) const
    {
        MCRegisterInfo const * const reg_info = get_register_info();
        register_index const to = get_register_index(reg_info->getName(inst.getOperand(0).getReg()));
        register_index const from = get_register_index(reg_info->getName(inst.getOperand(1).getReg()));

        store_register_value(to, load_register_value(from, builder, regset), builder, regset);
    }

    void x86_64_translator::gen_add_rr(MCInst const & inst, IRBuilder<> & builder, loaded_registers & regset) const
    {
        MCRegisterInfo const * const reg_info = get_register_info();
        register_index const to = get_register_index(reg_info->getName(inst.getOperand(0).getReg()));
        register_index const lhs = get_register_index(reg_info->getName(inst.getOperand(1).getReg()));
        register_index const rhs = get_register_index(reg_info->getName(inst.getOperand(2).getReg()));

        Value * const sum = builder.CreateAdd(load_register_value(lhs, builder, regset), load_register_value(rhs, builder, regset));
        store_register_value(to, sum, builder, regset);
    }

    void x86_64_translator::gen_store_registers(IRBuilder<> & builder, loaded_registers & regset) const
    {
        for (loaded_registers::const_iterator it(std::begin(regset)); it != std::end(regset); ++it)
            if (it->second.second)
                gen_store_register(it->first, it->second.first, builder);
    }

    Value * const x86_64_translator::gen_load_register(register_index reg, IRBuilder<> & builder) const
    {
        Value * const addr = ConstantInt::get(builder.getInt64Ty(), get_register_ptr(reg));
        Value * const rptr = builder.CreateIntToPtr(addr, PointerType::getUnqual(builder.getInt64Ty()));
        return builder.CreateLoad(rptr);
    }

    void x86_64_translator::gen_store_register(register_index reg, Value * val, IRBuilder<> & builder) const
    {
        Value * const addr = ConstantInt::get(builder.getInt64Ty(), get_register_ptr(reg));
        Value * const rptr = builder.CreateIntToPtr(addr, PointerType::getUnqual(builder.getInt64Ty()));
        builder.CreateStore(val, rptr);
    }

} /* namespace x86_64 */
} /* namespace pa */
