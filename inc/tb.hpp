#ifndef __PA_INC_TB_HPP__
#define __PA_INC_TB_HPP__

#include <llvm/MC/MCInst.h>
#include <llvm/ADT/OwningPtr.h>

#include <string>
#include <vector>
#include <utility>
#include <cstdint>

namespace llvm
{
    class MemoryObject;
    class Target;
}

namespace pa
{

    class tb
    {
        typedef std::vector<llvm::MCInst> container_type;

        public:
            typedef container_type::const_iterator const_iterator;

            tb(uint64_t addr = 0)
                : addr_(addr)
                , length_(0)
            { }

            void push_back(llvm::MCInst const & inst, size_t inst_size);

            const_iterator begin() const;
            const_iterator end() const;

            bool empty() const;
            size_t size() const;

            void swap(tb &other);

            uint64_t address() const;
            void set_address(uint64_t addr);

            uint64_t bytes() const;

        private:
            uint64_t addr_;
            uint64_t length_;
            container_type instructions_;
    };

    llvm::OwningPtr<tb const> create_tb(std::string const &triple, char const * bytes, size_t size, uint64_t addr = 0);
    llvm::OwningPtr<tb const> create_tb(std::string const &triple, llvm::MemoryObject const &bytes, uint64_t addr = 0);

} /* namespace pa */

namespace std
{

    template <>
    inline void swap<pa::tb>(pa::tb &l, pa::tb &r)
    { l.swap(r); }

}

#endif /*__PA_INC_TB_HPP__*/
