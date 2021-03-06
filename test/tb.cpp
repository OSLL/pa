#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>

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
    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86);

    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), (char const *)bytes, sizeof(bytes)));

    FATAL_CHECK(block);
    CHECK(block->size() == 3u);
    CHECK(block->bytes() == 9u);
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
    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86_64);

    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), (char const *)bytes, sizeof(bytes)));

    FATAL_CHECK(block);
    CHECK(block->size() == 7u);
    CHECK(block->bytes() == 36u);

    block = pa::create_tb(triple.getTriple(), (char const *)(bytes + block->bytes()), sizeof(bytes) - block->bytes());

    FATAL_CHECK(block);
    CHECK(block->size() == 3u);
    CHECK(block->bytes() == 16u);
}

void test_x64_jmp()
{
    unsigned char bytes[] = {
                              0x48, 0x03, 0x4d, 0xe8,       // add
                              0x89, 0xc8,                   // mov
                              0x89, 0x45, 0xf0,             // mov
                              0xe9, 0xc0, 0xff, 0xff, 0xff, // jmp
                              0x8b, 0x45, 0xf4,             // mov
                              0x5b,                         // pop
                              0xc3,                         // ret
                              0x5d                          // pop
                            };
    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86_64);

    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), (char const *)bytes, sizeof(bytes)));

    FATAL_CHECK(block);
    CHECK(block->size() == 4u);
    CHECK(block->bytes() == 14u);

    block = pa::create_tb(triple.getTriple(), (char const *)(bytes + block->bytes()), sizeof(bytes) - block->bytes());

    FATAL_CHECK(block);
    CHECK(block->size() == 3u);
    CHECK(block->bytes() == 5u);
}

void test_x64_fib()
{
    unsigned char bytes[] = {
                              0x48, 0x89, 0xca,                           /* movq %rcx, %rdx */
                              0x48, 0x01, 0xd9,                           /* addq %rbx, %rcx */
                              0x48, 0x89, 0xd3,                           /* movq %rdx, %rbx */
                              0x48, 0xff, 0xcf,                           /* decq %rdi */
                              0xeb, 0xed,                                 /* jmp -0x13 */
                              0x48, 0x89, 0xd8,                           /* movq %rdx, %rax */
                              0x8b, 0x45, 0xf4
                            };
    llvm::Triple triple("unknown-unknown-unknown");
    triple.setArch(llvm::Triple::x86_64);

    llvm::OwningPtr<pa::tb const> block(pa::create_tb(triple.getTriple(), (char const *)bytes, sizeof(bytes)));

    FATAL_CHECK(block);
    CHECK(block->size() == 5u);
    CHECK(block->bytes() == 14u);
}

int main()
{
    prepare();
    
    test_x86_call();
    test_x64_call();
    test_x64_jmp();
    test_x64_fib();

    return 0;
}
