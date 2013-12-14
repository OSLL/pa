#include <context.hpp>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/Module.h>

namespace pa
{

    using namespace llvm;

    /* execution_context */

    execution_context::execution_context(llvm::LLVMContext &ctx, llvm::OwningPtr<llvm::ExecutionEngine> &engine)
        : ctx_(ctx)
        , engine_(engine.take())
    {}

    LLVMContext & execution_context::get_context()
    { return ctx_; }

    ExecutionEngine * execution_context::get_engine()
    { return engine_.get(); }


    /* context_builder */

    context_builder::context_builder(LLVMContext &ctx)
        : ctx_(ctx)
        , err_ptr_(nullptr)
        , main_(new Module("main", ctx_))
    { }

    context_builder& context_builder::get_main(llvm::Module ** mptr)
    {
        *mptr = main_.get();
        return *this;
    }

    context_builder & context_builder::create_module(std::string const & name, llvm::Module ** mptr)
    {
        modules_.emplace_back(new Module(name, ctx_));
        if (mptr) *mptr = modules_.back().get();
        return *this;
    }

    context_builder & context_builder::set_err_str(std::string * err)
    {
        err_ptr_ = err;
        return *this;
    }

    OwningPtr<execution_context> context_builder::create()
    {
        std::string err;

        engine_.reset(EngineBuilder(main_.take()).setErrorStr(err_ptr_ ? err_ptr_ : &err).setEngineKind(EngineKind::JIT).create());
        for (std::vector<OwningPtr<Module> >::iterator it(modules_.begin()); it != modules_.end(); ++it)
            engine_->addModule(it->take());

        return OwningPtr<execution_context>(new execution_context(ctx_, engine_));
    }

} /* namespace pa */
