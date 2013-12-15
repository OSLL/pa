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
    pa::x86_64::x86_64_registers regs{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86_64);

    llvm::OwningPtr<pa::execution_context> exec(pa::context_builder(ctx).get_main(&module).create());
    FATAL_CHECK(module);
    FATAL_CHECK(exec.get());

    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), memory));
    FATAL_CHECK(block.get());

    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt64Ty(ctx), std::vector<llvm::Type *>(), false);
    FATAL_CHECK(ftype);

    llvm::Function *func = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, "dummy", module);
    FATAL_CHECK(func);

    llvm::IRBuilder<> builder(llvm::BasicBlock::Create(ctx, "body", func));
    llvm::OwningPtr<pa::x86_64::x86_64_translator> translator(llvm::dyn_cast<pa::x86_64::x86_64_translator>(pa::create_translator(triple).take()));
    FATAL_CHECK(translator.get());

    translator->set_registers_block(&regs);
    translator->translate(block.get(), builder);

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

void test_instrs(std::vector<std::pair<std::string, std::vector<unsigned char> > > const & instrs)
{
    for (std::vector<std::pair<std::string, std::vector<unsigned char> > >::const_iterator it(begin(instrs)); it != end(instrs); ++it)
    {
        llvm::StringRef code((char const *)it->second.data(), it->second.size());
        llvm::StringRefMemoryObject memory(code);
        test_instr(memory);
    }
}

void test_mov64rr_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"movq %rax, %rbx", {0x48, 0x89, 0xc3}},
        {"movq %rbx, %rcx", {0x48, 0x89, 0xd9}},
        {"movq %rcx, %rdx", {0x48, 0x89, 0xca}},
        {"movq %rdx, %rsi", {0x48, 0x89, 0xd6}},
        {"movq %rsi, %rdi", {0x48, 0x89, 0xf7}},
        {"movq %rdi, %rbp", {0x48, 0x89, 0xfd}},
        {"movq %rbp, %rsp", {0x48, 0x89, 0xec}},
        {"movq %rsp, %r8",  {0x48, 0x89, 0xe0}},
        {"movq %r8, %r9",   {0x4d, 0x89, 0xc1}},
        {"movq %r9, %r10",  {0x4d, 0x89, 0xca}},
        {"movq %r10, %r11", {0x4d, 0x89, 0xd3}},
        {"movq %r11, %r12", {0x4d, 0x89, 0xdc}},
        {"movq %r12, %r13", {0x4d, 0x89, 0xe5}},
        {"movq %r13, %r14", {0x4d, 0x89, 0xee}},
        {"movq %r14, %r15", {0x4d, 0x89, 0xf7}},
        {"movq %r15, %rax", {0x4d, 0x89, 0xf8}}
    };

    test_instrs(instrs);
}

void test_mov32rr_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"movq %eax, %ebx",   {0x89, 0xc3}},
        {"movq %ebx, %ecx",   {0x89, 0xd9}},
        {"movq %ecx, %edx",   {0x89, 0xca}},
        {"movq %edx, %esi",   {0x89, 0xd6}},
        {"movq %esi, %edi",   {0x89, 0xf7}},
        {"movq %edi, %ebp",   {0x89, 0xfd}},
        {"movq %ebp, %esp",   {0x89, 0xec}},
        {"movq %esp, %r8d",   {0x41, 0x89, 0xe0}},
        {"movq %r8d, %r9d",   {0x45, 0x89, 0xc1}},
        {"movq %r9d, %r10d",  {0x45, 0x89, 0xca}},
        {"movq %r10d, %r11d", {0x45, 0x89, 0xd3}},
        {"movq %r11d, %r12d", {0x45, 0x89, 0xdc}},
        {"movq %r12d, %r13d", {0x45, 0x89, 0xe5}},
        {"movq %r13d, %r14d", {0x45, 0x89, 0xee}},
        {"movq %r14d, %r15d", {0x45, 0x89, 0xf7}},
        {"movq %r15d, %eax",  {0x44, 0x89, 0xf8}}
    };

    test_instrs(instrs);
}

void test_add64rr_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"addq %rax, %rbx",   {0x48, 0x01, 0xc3}},
        {"addq %rbx, %rcx",   {0x48, 0x01, 0xd9}},
        {"addq %rcx, %rdx",   {0x48, 0x01, 0xca}},
        {"addq %rdx, %rsi",   {0x48, 0x01, 0xd6}},
        {"addq %rsi, %rdi",   {0x48, 0x01, 0xf7}},
        {"addq %rdi, %rbp",   {0x48, 0x01, 0xfd}},
        {"addq %rbp, %rsp",   {0x48, 0x01, 0xec}},
        {"addq %rsp, %r8",    {0x49, 0x01, 0xe0}},
        {"addq %r8, %r9",     {0x4d, 0x01, 0xc1}},
        {"addq %r9, %r10",    {0x4d, 0x01, 0xca}},
        {"addq %r10, %r11",   {0x4d, 0x01, 0xd3}},
        {"addq %r11, %r12",   {0x4d, 0x01, 0xdc}},
        {"addq %r12, %r13",   {0x4d, 0x01, 0xe5}},
        {"addq %r13, %r14",   {0x4d, 0x01, 0xee}},
        {"addq %r14, %r15",   {0x4d, 0x01, 0xf7}},
        {"addq %r15, %rax",   {0x4c, 0x01, 0xf8}}
    };

    test_instrs(instrs);
}

void test_add32rr_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"addq %eax, %ebx",   {0x01, 0xc3}},
        {"addq %ebx, %ecx",   {0x01, 0xd9}},
        {"addq %ecx, %edx",   {0x01, 0xca}},
        {"addq %edx, %esi",   {0x01, 0xd6}},
        {"addq %esi, %edi",   {0x01, 0xf7}},
        {"addq %edi, %ebp",   {0x01, 0xfd}},
        {"addq %ebp, %esp",   {0x01, 0xec}},
        {"addq %esp, %r8d",   {0x41, 0x01, 0xe0}},
        {"addq %r8d, %r9d",   {0x45, 0x01, 0xc1}},
        {"addq %r9d, %r10d",  {0x45, 0x01, 0xca}},
        {"addq %r10d, %r11d", {0x45, 0x01, 0xd3}},
        {"addq %r11d, %r12d", {0x45, 0x01, 0xdc}},
        {"addq %r12d, %r13d", {0x45, 0x01, 0xe5}},
        {"addq %r13d, %r14d", {0x45, 0x01, 0xee}},
        {"addq %r14d, %r15d", {0x45, 0x01, 0xf7}},
        {"addq %r15d, %eax",  {0x44, 0x01, 0xf8}}
    };

    test_instrs(instrs);
}

void test_test64rr_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"testq %rax, %rbx",   {0x48, 0x85, 0xc3}},
        {"testq %rbx, %rcx",   {0x48, 0x85, 0xd9}},
        {"testq %rcx, %rdx",   {0x48, 0x85, 0xca}},
        {"testq %rdx, %rsi",   {0x48, 0x85, 0xd6}},
        {"testq %rsi, %rdi",   {0x48, 0x85, 0xf7}},
        {"testq %rdi, %rbp",   {0x48, 0x85, 0xfd}},
        {"testq %rbp, %rsp",   {0x48, 0x85, 0xec}},
        {"testq %rsp, %r8",    {0x49, 0x85, 0xe0}},
        {"testq %r8, %r9",     {0x4d, 0x85, 0xc1}},
        {"testq %r9, %r10",    {0x4d, 0x85, 0xca}},
        {"testq %r10, %r11",   {0x4d, 0x85, 0xd3}},
        {"testq %r11, %r12",   {0x4d, 0x85, 0xdc}},
        {"testq %r12, %r13",   {0x4d, 0x85, 0xe5}},
        {"testq %r13, %r14",   {0x4d, 0x85, 0xee}},
        {"testq %r14, %r15",   {0x4d, 0x85, 0xf7}},
        {"testq %r15, %rax",   {0x4c, 0x85, 0xf8}}
    };

    test_instrs(instrs);
}

void test_test32rr_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"testl %eax, %ebx",   {0x85, 0xc3}},
        {"testl %ebx, %ecx",   {0x85, 0xd9}},
        {"testl %ecx, %edx",   {0x85, 0xca}},
        {"testl %edx, %esi",   {0x85, 0xd6}},
        {"testl %esi, %edi",   {0x85, 0xf7}},
        {"testl %edi, %ebp",   {0x85, 0xfd}},
        {"testl %ebp, %esp",   {0x85, 0xec}},
        {"testl %esp, %r8d",   {0x41, 0x85, 0xe0}},
        {"testl %r8d, %r9d",   {0x45, 0x85, 0xc1}},
        {"testl %r9d, %r10d",  {0x45, 0x85, 0xca}},
        {"testl %r10d, %r11d", {0x45, 0x85, 0xd3}},
        {"testl %r11d, %r12d", {0x45, 0x85, 0xdc}},
        {"testl %r12d, %r13d", {0x45, 0x85, 0xe5}},
        {"testl %r13d, %r14d", {0x45, 0x85, 0xee}},
        {"testl %r14d, %r15d", {0x45, 0x85, 0xf7}},
        {"testl %r15d, %eax",  {0x44, 0x85, 0xf8}}
    };

    test_instrs(instrs);
}

void test_jmp_1_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"jmp 5",   {0xeb, 0x03}},
        {"jne 5",   {0x75, 0xfb}},
        {"je 21",   {0x74, 0x0e}}
    };

    test_instrs(instrs);
}

void test_mov64ri32_instrs()
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > instrs{
        {"movq 0, rbx",   {0x48, 0xc7, 0xc3, 0x00, 0x00, 0x00, 0x00}},
        {"movq 1, rcx",   {0x48, 0xc7, 0xc1, 0x01, 0x00, 0x00, 0x00}}
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
    test_test64rr_instrs();
    test_test32rr_instrs();
    test_jmp_1_instrs();
    test_mov64ri32_instrs();

    return 0;
}
