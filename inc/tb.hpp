#ifndef __PA_INC_TB_HPP__
#define __PA_INC_TB_HPP__

#include <llvm/MC/MCInst.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/MemoryObject.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/Triple.h>

#include <vector>
#include <utility>
#include <iterator>
#include <cstdint>

namespace pa
{

    class tb
    {
        typedef std::vector<llvm::MCInst> container_type;

        public:
            typedef container_type::const_iterator const_iterator;

            tb(llvm::Target const * const target, uintptr_t offset);

            void push_back(llvm::MCInst const &inst);

            const_iterator begin() const;
            const_iterator end() const;
            bool empty() const;
            size_t size() const;
            size_t bytes() const;

            void swap(tb &other);

            uintptr_t get_offset() const;
            llvm::Target const * get_target() const;

        private:
            llvm::Target const * target_;
            uintptr_t offset_;
            container_type instructions_;
    };

    tb * create_tb(llvm::Triple const &triple, llvm::MemoryObject const &bytes, uint64_t base, uint64_t offset = 0u);

} /* namespace pa */

namespace std
{

    template <>
    void swap<pa::tb>(pa::tb &l, pa::tb &r)
    { l.swap(r); }

}

#endif /*__PA_INC_TB_HPP__*/
