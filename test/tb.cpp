#include <llvm/Support/MemoryObject.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/StringRefMemoryObject.h>

#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/Triple.h>
#include <llvm/ADT/OwningPtr.h>

#include <tb.hpp>

#include "macro.hpp"

void prepare()
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();
}

void test_x86_call()
{
    unsigned char bytes[] = {
                             0x53,                               // push
                             0x83, 0xec, 0x08,                   // sub
                             0xe8, 0xdb, 0x00, 0x00, 0x00,       // call
                             0x81, 0xc3, 0xfb, 0x1a, 0x00, 0x00  // add
                           };
    llvm::StringRef code((char const *)bytes, sizeof(bytes));
    llvm::StringRefMemoryObject memory(code);
    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86);

    uint64_t block_size = 0;
    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), memory, block_size));

    FATAL_CHECK(block);
    CHECK(block->size() == 3u);
    CHECK(block_size == 9u);
}

void test_x64_call()
{
    unsigned char bytes[] = {
                             0x55,                                      // push
                             0x48, 0x89, 0xe5,                          // mov
                             0x48, 0x83, 0xec, 0x10,                    // sub
                             0x48, 0x8d, 0x3c, 0x25, 0x80, 0x10, 0x60,  // lea
                             0x00,
                             0x48, 0x8d, 0x34, 0x25, 0xb4, 0x09, 0x40,  // lea
                             0x00,
                             0xc7, 0x45, 0xfc, 0x00, 0x00, 0x00, 0x00,  // movl
                             0xe8, 0x6c, 0xfe, 0xff, 0xff,              // call
                             0x48, 0x8d, 0x34, 0x25, 0x70, 0x07, 0x40,  // lea
                             0x00,
                             0x48, 0x89, 0xc7,                          // mov
                             0xe8, 0x6c, 0xfe, 0xff, 0xff               // call
                           };
    llvm::StringRef code((char const *)bytes, sizeof(bytes));
    llvm::StringRefMemoryObject memory(code);
    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86_64);

    uint64_t block_size = 0;
    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), memory, block_size));

    FATAL_CHECK(block);
    CHECK(block->size() == 7u);
    CHECK(block_size == 36u);

    block = pa::create_tb(triple.getTriple(), memory, block_size, block_size);

    FATAL_CHECK(block);
    CHECK(block->size() == 3u);
    CHECK(block_size == 16u);
}

int main()
{
    prepare();
    test_x86_call();
    test_x64_call();
    return 0;
}