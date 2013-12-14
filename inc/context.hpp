#ifndef __PA_INC_CONTEXT_HPP__
#define __PA_INC_CONTEXT_HPP__

#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/OwningPtr.h>

#include <vector>
#include <string>

namespace llvm
{
    class ExecutionEngine;
    class Module;
}

namespace pa
{

    class execution_context
    {
        public:
            execution_context(llvm::LLVMContext &ctx, llvm::OwningPtr<llvm::ExecutionEngine> &engine);

            llvm::LLVMContext & get_context();
            llvm::ExecutionEngine * get_engine();

        private:
            execution_context(execution_context const &) = delete;
            execution_context & operator=(execution_context const &) = delete;

            llvm::LLVMContext &ctx_;
            llvm::OwningPtr<llvm::ExecutionEngine> engine_;
    };

    class context_builder
    {
    public:
        context_builder(llvm::LLVMContext &ctx);

        context_builder& get_main(llvm::Module ** mptr);
        context_builder& create_module(std::string const & name, llvm::Module ** mptr = nullptr);
        context_builder& set_err_str(std::string * err);

        llvm::OwningPtr<execution_context> create();

    private:
        llvm::LLVMContext &ctx_;
        std::string *err_ptr_;
        llvm::OwningPtr<llvm::Module> main_;
        std::vector<llvm::OwningPtr<llvm::Module> > modules_;
        llvm::OwningPtr<llvm::ExecutionEngine> engine_;
    };

} /* namespace pa */

#endif /*__PA_INC_CONTEXT_HPP__*/
