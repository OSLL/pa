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
    llvm::LLVMContext ctx;
    llvm::OwningPtr<pa::execution_context> exec(pa::context_builder(ctx, "test").create());

    FATAL_CHECK(exec);
    FATAL_CHECK(exec->get_engine());
    FATAL_CHECK(exec->get_cpu_state());

    llvm::Module* module = new llvm::Module(__func__, ctx);
    FATAL_CHECK(module);

    exec->get_engine()->addModule(module);

    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), std::vector<llvm::Type *>(), false);
    llvm::Function *func = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, "inc_rax", module);

    llvm::IRBuilder<> builder(llvm::BasicBlock::Create(ctx, "body", func));
    llvm::Value *ptrtorax = builder.CreateStructGEP(exec->get_cpu_state(), 0);
    llvm::Value *rax = builder.CreateLoad(ptrtorax);
    llvm::Value *one = llvm::ConstantInt::get(llvm::IntegerType::get(ctx, 64), 1u);
    llvm::Value *incremented = builder.CreateAdd(rax, one);
    builder.CreateStore(incremented, ptrtorax);
    builder.CreateRetVoid();

    FATAL_CHECK(llvm::verifyModule(*module) == false);

    exec->get_engine()->runFunction(func, std::vector<llvm::GenericValue>());

    uint64_t *ptrtocontext = (uint64_t *)exec->get_engine()->getPointerToGlobal(exec->get_cpu_state());
    FATAL_CHECK(ptrtocontext);
    FATAL_CHECK(*ptrtocontext == 1ul);
}

int main()
{
    prepare();
    test_x86_module();

    return 0;
}
