#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

#include <context.hpp>

#include "macro.hpp"

void prepare()
{
    llvm::InitializeNativeTarget();
}

void test_x86_module()
{
    std::string err;
    uint64_t rax = 0u;
    llvm::LLVMContext ctx;

    llvm::Module* module = nullptr;
    llvm::OwningPtr<pa::execution_context> exec(pa::context_builder(ctx).get_main(&module).create());
    FATAL_CHECK(module);
    FATAL_CHECK(exec);

    llvm::Value *raxaddr = llvm::ConstantInt::get(llvm::IntegerType::get(ctx, 64), uint64_t(&rax));
    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), std::vector<llvm::Type *>(), false);
    llvm::PointerType *regptr = llvm::PointerType::getUnqual(llvm::IntegerType::get(ctx, 64));
    llvm::Function *func = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, "inc_rax", module);

    llvm::IRBuilder<> builder(llvm::BasicBlock::Create(ctx, "body", func));
    llvm::Value *ptrtorax = builder.CreateIntToPtr(raxaddr, regptr);
    llvm::Value *reg = builder.CreateLoad(ptrtorax);
    llvm::Value *one = llvm::ConstantInt::get(llvm::IntegerType::get(ctx, 64), 1u);
    llvm::Value *incremented = builder.CreateAdd(reg, one);
    builder.CreateStore(incremented, ptrtorax);
    builder.CreateRetVoid();

    FATAL_CHECK(llvm::verifyModule(*module) == false);

    exec->get_engine()->runFunction(func, std::vector<llvm::GenericValue>());

    FATAL_CHECK(rax == 1ul);
}

int main()
{
    prepare();
    test_x86_module();

    return 0;
}
