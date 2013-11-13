#ifndef __PA_INC_CONTEXT_HPP__
#define __PA_INC_CONTEXT_HPP__

#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/OwningPtr.h>

#include <string>

namespace llvm
{
    class ExecutionEngine;
    class GlobalVariable;
    class StructType;
    class Module;
}

namespace pa
{

    class execution_context
    {
        public:
            llvm::LLVMContext & get_context();
            llvm::ExecutionEngine * get_engine();
            llvm::GlobalVariable * get_cpu_state();

        private:
            execution_context(llvm::LLVMContext &ctx);

            execution_context(execution_context const &) = delete;
            execution_context & operator=(execution_context const &) = delete;

            void set_engine(llvm::OwningPtr<llvm::ExecutionEngine> & engine);
            void set_cpu_type(llvm::StructType *cput);
            void set_cpu_state(llvm::GlobalVariable *cpus);

            llvm::LLVMContext &ctx_;
            llvm::StructType *cpu_type_;
            llvm::GlobalVariable *cpu_state_;
            llvm::OwningPtr<llvm::ExecutionEngine> engine_;

            friend class context_builder;
    };

    class context_builder
    {
    public:
        context_builder(llvm::LLVMContext &ctx, std::string const & id);

        context_builder& set_engine(llvm::OwningPtr<llvm::ExecutionEngine> & engine);
        context_builder& set_cpu_type(llvm::StructType * cpu_type);
        context_builder& set_err_str(std::string * err);

        llvm::OwningPtr<execution_context> create();

    private:
        llvm::OwningPtr<execution_context> ctx_;
        llvm::OwningPtr<llvm::Module> module_;
        std::string id_;
        std::string *err_ptr_;

        llvm::OwningPtr<llvm::ExecutionEngine> engine_;
        llvm::StructType * cpu_type_;
    };

} /* namespace pa */

#endif /*__PA_INC_CONTEXT_HPP__*/
