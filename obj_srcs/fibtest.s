    .text
    .global fib
    .type fib, @function
fib:
    movq $0, %rbx
    movq $1, %rcx
loop:
    testq %rdi, %rdi
    jz loopend

    movq %rcx, %rdx
    addq %rbx, %rcx
    movq %rdx, %rbx
    decq %rdi
    jmp loop

loopend:
    movq %rbx, %rax
    ret

    .size fib, .-fib
