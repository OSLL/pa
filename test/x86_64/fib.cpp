#include <x86_64/x86_64_translator.hpp>
#include <context.hpp>
#include <utility.hpp>
#include <tb.hpp>

#include "../macro.hpp"

#include <llvm/Support/MemoryObject.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/StringRefMemoryObject.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/Triple.h>
#include <llvm/ADT/OwningPtr.h>

#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

#include <llvm/PassManager.h>

#include <llvm/Transforms/Scalar.h>

void prepare()
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();
    llvm::InitializeNativeTarget();
}

uint64_t fib(uint64_t num)
{
    uint64_t fib0 = 0, fib1 = 1;
    while (num--)
    {
        uint64_t const temp = fib1;
        fib1 += fib0;
        fib0 = temp;
    }
    return fib0;
}

void test_fib_function(uint64_t number)
{
    unsigned char instrs[] = {
        0x48, 0xc7, 0xc1, 0x01, 0x00, 0x00, 0x00,   /* movq 1, %rcx */
        0x48, 0xc7, 0xc3, 0x00, 0x00, 0x00, 0x00,   /* movq 0, %rbx */
        0x48, 0x85, 0xff,                           /* testq %rdi, %rdi */
        0x74, 0x0e,                                 /* je +0x0e */
        0x48, 0x89, 0xca,                           /* movq %rcx, %rdx */
        0x48, 0x01, 0xd9,                           /* addq %rbx, %rcx */
        0x48, 0x89, 0xd3,                           /* movq %rdx, %rbx */
        0x48, 0xff, 0xcf,                           /* decq %rdi */
        0xeb, 0xed,                                 /* jmp -0x13 */
        0x48, 0x89, 0xd8                            /* movq %rdx, %rax */ 
    };

    llvm::LLVMContext ctx;
    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86_64);
    llvm::Module* module = nullptr;
    llvm::OwningPtr<pa::execution_context> exec(pa::context_builder(ctx).get_main(&module).create());

    FATAL_CHECK(module);
    FATAL_CHECK(exec.get());

    pa::x86_64::x86_64_registers regs;
    regs.rdi = number;

    llvm::OwningPtr<pa::x86_64::x86_64_translator> translator(llvm::dyn_cast<pa::x86_64::x86_64_translator>(pa::create_translator(triple).take()));
    FATAL_CHECK(translator.get());

    translator->set_registers_block(&regs);

    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt64Ty(ctx), std::vector<llvm::Type *>(), false);
    FATAL_CHECK(ftype);

    std::unordered_map<uint64_t, llvm::Function *> blocks;

    unsigned char const * rip = instrs;
    while (rip < instrs + sizeof(instrs))
    {
        uint64_t const address = reinterpret_cast<uint64_t>(rip);
        if (blocks.find(address) == std::end(blocks))
        {
            llvm::Function *func = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, stringify("block", address), module);
            FATAL_CHECK(func);
            llvm::IRBuilder<> builder(llvm::BasicBlock::Create(ctx, "body", func));
            llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.str(), (char const *)rip, sizeof(instrs) - (rip - instrs), address));
            translator->translate(block.get(), builder);
            llvm::verifyFunction(*func);
            blocks[address] = func;
            //func->dump();
        }

        uint64_t const next = exec->get_engine()->runFunction(blocks[address], std::vector<llvm::GenericValue>()).IntVal.getZExtValue();
        rip = reinterpret_cast<unsigned char const *>(next);
    }

    FATAL_CHECK(regs.rax == fib(number));
}

int main()
{
    prepare();
    test_fib_function(0);
    test_fib_function(1);
    test_fib_function(2);
    test_fib_function(3);
    test_fib_function(4);
    test_fib_function(5);

    return 0;
}
