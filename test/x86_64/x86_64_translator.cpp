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

void test_instr(llvm::StringRefMemoryObject const & memory)
{
    llvm::LLVMContext ctx;
    llvm::Module* module = nullptr;
    pa::x86_64::x86_64_registers regs{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86_64);

    llvm::OwningPtr<pa::execution_context> exec(pa::context_builder(ctx).get_main(&module).create());
    FATAL_CHECK(module);
    FATAL_CHECK(exec.get());

    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), memory));
    FATAL_CHECK(block.get());

    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), std::vector<llvm::Type *>(), false);
    FATAL_CHECK(ftype);

    llvm::Function *func = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, "dummy", module);
    FATAL_CHECK(func);

    llvm::IRBuilder<> builder(llvm::BasicBlock::Create(ctx, "body", func));
    llvm::OwningPtr<pa::x86_64::x86_64_translator> translator(llvm::dyn_cast<pa::x86_64::x86_64_translator>(pa::create_translator(triple).take()));
    FATAL_CHECK(translator.get());

    translator->set_registers_block(&regs);
    translator->translate(block.get(), builder);
    builder.CreateRetVoid();

    FATAL_CHECK(llvm::verifyFunction(*func) == false);
    FATAL_CHECK(llvm::verifyModule(*module) == false);

    llvm::FunctionPassManager pm(module);
    pm.add(llvm::createInstructionCombiningPass());
    pm.add(llvm::createReassociatePass());
    pm.add(llvm::createGVNPass());
    pm.doInitialization();
    pm.run(*func);
    pm.doFinalization();

    //module->dump();

    exec->get_engine()->runFunction(func, std::vector<llvm::GenericValue>());
}

void test_instrs(std::vector<std::pair<std::string, std::string> > const & instrs)
{
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it(begin(instrs)); it != end(instrs); ++it)
    {
        llvm::outs() << it->first << " [" << pa::hex_format((uint8_t const *)it->second.data(), it->second.size()) << "]\n";
        llvm::StringRef code(it->second.data(), it->second.size());
        llvm::StringRefMemoryObject memory(code);
        test_instr(memory);
    }
}

void test_mov64rr_instrs()
{
    std::vector<std::pair<std::string, std::string> > instrs{
        {"movq %rax, %rbx", "\x48\x89\xc3"},
        {"movq %rbx, %rcx", "\x48\x89\xd9"},
        {"movq %rcx, %rdx", "\x48\x89\xca"},
        {"movq %rdx, %rsi", "\x48\x89\xd6"},
        {"movq %rsi, %rdi", "\x48\x89\xf7"},
        {"movq %rdi, %rbp", "\x48\x89\xfd"},
        {"movq %rbp, %rsp", "\x48\x89\xec"},
        {"movq %rsp, %r8",  "\x48\x89\xe0"},
        {"movq %r8, %r9",   "\x4d\x89\xc1"},
        {"movq %r9, %r10",  "\x4d\x89\xca"},
        {"movq %r10, %r11", "\x4d\x89\xd3"},
        {"movq %r11, %r12", "\x4d\x89\xdc"},
        {"movq %r12, %r13", "\x4d\x89\xe5"},
        {"movq %r13, %r14", "\x4d\x89\xee"},
        {"movq %r14, %r15", "\x4d\x89\xf7"},
        {"movq %r15, %rax", "\x4d\x89\xf8"}
    };

    test_instrs(instrs);
}

void test_mov32rr_instrs()
{
    std::vector<std::pair<std::string, std::string> > instrs{
        {"movq %eax, %ebx",   "\x89\xc3"},
        {"movq %ebx, %ecx",   "\x89\xd9"},
        {"movq %ecx, %edx",   "\x89\xca"},
        {"movq %edx, %esi",   "\x89\xd6"},
        {"movq %esi, %edi",   "\x89\xf7"},
        {"movq %edi, %ebp",   "\x89\xfd"},
        {"movq %ebp, %esp",   "\x89\xec"},
        {"movq %esp, %r8d",   "\x41\x89\xe0"},
        {"movq %r8d, %r9d",   "\x45\x89\xc1"},
        {"movq %r9d, %r10d",  "\x45\x89\xca"},
        {"movq %r10d, %r11d", "\x45\x89\xd3"},
        {"movq %r11d, %r12d", "\x45\x89\xdc"},
        {"movq %r12d, %r13d", "\x45\x89\xe5"},
        {"movq %r13d, %r14d", "\x45\x89\xee"},
        {"movq %r14d, %r15d", "\x45\x89\xf7"},
        {"movq %r15d, %eax",  "\x44\x89\xf8"}
    };

    test_instrs(instrs);
}

void test_add64rr_instrs()
{
    std::vector<std::pair<std::string, std::string> > instrs{
        {"addq %rax, %rbx",   "\x48\x01\xc3"},
        {"addq %rbx, %rcx",   "\x48\x01\xd9"},
        {"addq %rcx, %rdx",   "\x48\x01\xca"},
        {"addq %rdx, %rsi",   "\x48\x01\xd6"},
        {"addq %rsi, %rdi",   "\x48\x01\xf7"},
        {"addq %rdi, %rbp",   "\x48\x01\xfd"},
        {"addq %rbp, %rsp",   "\x48\x01\xec"},
        {"addq %rsp, %r8",    "\x49\x01\xe0"},
        {"addq %r8, %r9",     "\x4d\x01\xc1"},
        {"addq %r9, %r10",    "\x4d\x01\xca"},
        {"addq %r10, %r11",   "\x4d\x01\xd3"},
        {"addq %r11, %r12",   "\x4d\x01\xdc"},
        {"addq %r12, %r13",   "\x4d\x01\xe5"},
        {"addq %r13, %r14",   "\x4d\x01\xee"},
        {"addq %r14, %r15",   "\x4d\x01\xf7"},
        {"addq %r15, %rax",   "\x4c\x01\xf8"}
    };

    test_instrs(instrs);
}

void test_add32rr_instrs()
{
    std::vector<std::pair<std::string, std::string> > instrs{
        {"addq %eax, %ebx",   "\x01\xc3"},
        {"addq %ebx, %ecx",   "\x01\xd9"},
        {"addq %ecx, %edx",   "\x01\xca"},
        {"addq %edx, %esi",   "\x01\xd6"},
        {"addq %esi, %edi",   "\x01\xf7"},
        {"addq %edi, %ebp",   "\x01\xfd"},
        {"addq %ebp, %esp",   "\x01\xec"},
        {"addq %esp, %r8d",   "\x41\x01\xe0"},
        {"addq %r8d, %r9d",   "\x45\x01\xc1"},
        {"addq %r9d, %r10d",  "\x45\x01\xca"},
        {"addq %r10d, %r11d", "\x45\x01\xd3"},
        {"addq %r11d, %r12d", "\x45\x01\xdc"},
        {"addq %r12d, %r13d", "\x45\x01\xe5"},
        {"addq %r13d, %r14d", "\x45\x01\xee"},
        {"addq %r14d, %r15d", "\x45\x01\xf7"},
        {"addq %r15d, %eax",  "\x44\x01\xf8"}
    };

    test_instrs(instrs);
}

int main()
{
    prepare();
    test_mov64rr_instrs();
    test_mov32rr_instrs();
    test_add64rr_instrs();
    test_add32rr_instrs();

    return 0;
}
