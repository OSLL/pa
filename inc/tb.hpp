#ifndef __PA_INC_TB_HPP__
#define __PA_INC_TB_HPP__

#include <llvm/MC/MCInst.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/MemoryObject.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/Triple.h>

#include <string>
#include <vector>
#include <utility>
#include <cstdint>

namespace pa
{

    class tb
    {
        typedef std::vector<llvm::MCInst> container_type;

        public:
            typedef container_type::const_iterator const_iterator;

            tb(llvm::Target const * const target);

            void push_back(llvm::MCInst const &inst);

            const_iterator begin() const;
            const_iterator end() const;
            bool empty() const;
            size_t size() const;

            void swap(tb &other);

            llvm::Target const * get_target() const;

        private:
            llvm::Target const * target_;
            container_type instructions_;
    };

    llvm::OwningPtr<tb const> create_tb(std::string const &triple, llvm::MemoryObject const &bytes, uint64_t &size, uint64_t offset = 0u);

} /* namespace pa */

namespace std
{

    template <>
    inline void swap<pa::tb>(pa::tb &l, pa::tb &r)
    { l.swap(r); }

}

#endif /*__PA_INC_TB_HPP__*/
