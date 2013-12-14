#ifndef __PA_INC_UTILITY_HPP__
#define __PA_INC_UTILITY_HPP__

#include <llvm/Support/MemoryObject.h>
#include <llvm/Support/Format.h>

#include <algorithm>
#include <vector>
#include <cstdio>

namespace pa
{

    class hex_formatter : public llvm::format_object_base
    {
        std::vector<char> data;

    protected:
        virtual int snprint(char *buffer, unsigned size) const
        {
            if (data.size() < static_cast<uint64_t>(size))
                std::copy(data.data(), data.data() + data.size(), buffer);
            return data.size();
        }

    public:
        hex_formatter(uint8_t const *bytes, uint64_t size)
            : llvm::format_object_base(nullptr)
            , data(3 * size + 1u, ' ')
        {
            for (uint64_t it = 0u, off = 0u; it != size; ++it)
                off += snprintf(data.data() + off, data.size(), "%02x ", static_cast<unsigned>(bytes[it]));
        }
    };

    inline hex_formatter hex_format(uint8_t const *bytes, uint64_t size)
    { return hex_formatter(bytes, size); }

    inline hex_formatter hex_format(llvm::MemoryObject const &bytes, uint64_t addr, uint64_t size)
    {
        std::vector<uint8_t> buffer(size, 0u);
        uint64_t len = std::min(size, bytes.getBase() + bytes.getExtent() - addr);
        if (bytes.readBytes(addr, len, buffer.data()) != 0)
            return hex_formatter(buffer.data(), 0u);
        return hex_formatter(buffer.data(), len);
    }

} /* namespace pa */

#endif /*__PA_INC_UTILITY_HPP__*/
