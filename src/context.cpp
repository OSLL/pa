#include <context.hpp>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>

#include <vector>

namespace pa
{

    using namespace llvm;

    struct Reg
    {
        char const * name;
        unsigned     size;
    };

    static std::vector<Reg> const regs{
                                          {"rax", 64u},
                                          {"rcx", 64u},
                                          {"rdx", 64u},
                                          {"rbx", 64u},
                                          {"rsp", 64u},
                                          {"rbp", 64u},
                                          {"rsi", 64u},
                                          {"rdi", 64u}
                                      };


    /* execution_context */

    execution_context::execution_context(llvm::LLVMContext &ctx) : ctx_(ctx) {}

    LLVMContext & execution_context::get_context()
    { return ctx_; }

    ExecutionEngine * execution_context::get_engine()
    { return engine_.get(); }

    GlobalVariable * execution_context::get_cpu_state()
    { return cpu_state_; }

    void execution_context::set_engine(OwningPtr<ExecutionEngine> & engine)
    { engine_.swap(engine); }

    void execution_context::set_cpu_type(StructType *cput)
    { cpu_type_ = cput; }

    void execution_context::set_cpu_state(GlobalVariable *cpus)
    { cpu_state_ = cpus; }


    /* context_builder */

    context_builder::context_builder(LLVMContext &ctx, std::string const & id)
        : ctx_(new execution_context(ctx))
        , module_(new Module(id, ctx))
        , id_(id)
        , err_ptr_(nullptr)
    {}

    context_builder & context_builder::set_engine(OwningPtr<ExecutionEngine> & engine)
    {
        engine_.swap(engine);
        return *this;
    }

    context_builder & context_builder::set_cpu_type(StructType * cpu_type)
    {
        cpu_type_ = cpu_type;
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
        Module *module = module_.get();

        if (!engine_)
            engine_.reset(EngineBuilder(module_.take()).setErrorStr(err_ptr_ ? err_ptr_ : &err).setEngineKind(EngineKind::JIT).create());
        else
            engine_->addModule(module_.take());

        if (!cpu_type_)
        {
            std::vector<Type *> members;
            for (Reg const &reg : regs)
                members.push_back(IntegerType::get(ctx_->get_context(), reg.size));
            cpu_type_ = StructType::create(ctx_->get_context(), members);
        }

        module->getOrInsertGlobal("context", cpu_type_);
        GlobalVariable *cpu_state = module->getGlobalVariable("context");
        cpu_state->setInitializer(llvm::Constant::getNullValue(cpu_type_));

        ctx_->set_engine(engine_);
        ctx_->set_cpu_type(cpu_type_);
        ctx_->set_cpu_state(cpu_state);

        return OwningPtr<execution_context>(ctx_.take());
    }

} /* namespace pa */
