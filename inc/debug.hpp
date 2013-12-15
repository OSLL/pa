#ifndef __INC_DEBUG_HPP__
#define __INC_DEBUG_HPP__

#include <llvm/Support/raw_ostream.h>

#include <sstream>

namespace pa
{

template <typename Stream>
Stream & output(Stream & out)
{ return out; }

template <typename Stream, typename Type, typename ... Types>
Stream & output(Stream & out, Type const & obj, Types const & ... args)
{
    out << obj;
    return output(out, args ...);
}

} /* namespace pa */

#define warn(...) pa::output(llvm::errs(), "In ", __FILE__, "[", __LINE__, "]: ", __VA_ARGS__, "\n")
#define info(...) pa::output(llvm::outs(), "In ", __FILE__, "[", __LINE__, "]: ", __VA_ARGS__, "\n")
#define debug(...) pa::output(llvm::outs(), "In ", __FILE__, "[", __LINE__, "]: ", __VA_ARGS__, "\n")

template <typename ... Types>
std::string const stringify(Types const & ... args)
{
    std::stringstream stream;
    pa::output(stream, args ...);
    return stream.str();
}

#endif /*__INC_DEBUG_HPP__*/
