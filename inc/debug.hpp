#ifndef __INC_DEBUG_HPP__
#define __INC_DEBUG_HPP__

#include <llvm/Support/raw_ostream.h>

template <typename Stream>
Stream & debug_output(Stream & out)
{ return out; }

template <typename Stream, typename Type, typename ... Types>
Stream & debug_output(Stream & out, Type const & obj, Types const & ... args)
{
    out << obj;
    return debug_output(out, args ...);
}

#define warn(...) debug_output(llvm::errs(), "In ", __func__, ": ", __VA_ARGS__, "\n")
#define info(...) debug_output(llvm::outs(), "In ", __func__, ": ", __VA_ARGS__, "\n")
#define debug(...) debug_output(llvm::outs(), "In ", __func__, ": ", __VA_ARGS__, "\n")

#endif /*__INC_DEBUG_HPP__*/
