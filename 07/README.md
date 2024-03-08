Homework 7 - Cashier: the naive cache simulator
===============================================

> pengcheng2

**Contents**

*   [Homework 7 - Cashier: the naive cache simulator](about:blank#homework-7---cashier-the-naive-cache-simulator)
    *   [Introduction](about:blank#introduction)
    *   [Starter code](about:blank#starter-code)
    *   [Task description](about:blank#task-description)
    *   [Specifications](about:blank#specifications)
        *   [A few rules](about:blank#a-few-rules)
        *   [Spec on each function](about:blank#spec-on-each-function)
    *   [Testing on local machine](about:blank#testing-on-local-machine)
    *   [Code quality requirements](about:blank#code-quality-requirements)
    *   [Submission](about:blank#submission)
    *   [Contacts](about:blank#contacts)

Homework 7 - Cashier: the naive cache simulator
===============================================

Introduction
------------

Exploiting locality in the memory access with data cache is one of the key idea in CS110 Computer Architecture.  
This homework helps you to consolidate your understanding of caching through implementing cashier, the simple cache simulator.

Starter code
------------

Download the code template via [this link to starter.tar](https://toast-lab.sist.shanghaitech.edu.cn/starter.zip).  
Use the command `tar xvf starter.tar` to extract all the files from the archive file.

You should see the following directory structure

```
# output of `exa --tree starter`
starter
├── cashier.c
├── cashier.h
├── check_comment.py
├── main.c
└── Makefile
```

*   `check_comment.py` is a python script which enforces a line of comment in every 6 lines of code (empty lines, lines with only brackets and preprocessing directives do not count)
*   `cashier.h` includes declarations of
    *   the cache simulator structure
    *   operations (functions) on the cache simulator: read, write, etc.
    *   interfaces to the environment i.e., accessing the time stamp, reading/writing from data memory and eviction callback function.
*   `cashier.c` includes the implementation of the cache access functions, which should be currently empty.
*   `main.c` includes an exemplary dot-product computation program that employs the cache simulator. The implementation of environmental interfaces is also implemented in this file.
*   `Makefile` includes the following targets
    *   `hw7.tar`: generate a tarball from the source code for submission
    *   `simulator`: build an executable from `main.c` for testing

Task description
----------------

Give correct implementation for the empty functions in `cashier.c` so you get a data cache simulator that:

*   contains a single level
*   employs LRU replacement policy
*   uses write-back on eviction
*   has configurable associative, cache line size and total cache data capacity.

Short explanation of the structure members, function parameters can be found in `cashier.h`.  
Brief description of the expected behavior are also given as comments in `cashier.c`.  
You should at least read through them before start composing your solution.

Specifications
--------------

### A few rules

*   Access to cache are done via call to `cashier_read` and `cashier_write`.
*   On cache-miss, you should make several calls to `mem_read` to fetch the cache line from data memory.
*   If one cache line with dirty bit set is evicted, the contents should be written to memory via `mem_write`.
*   All the cache lines are considered evicted on `cashier_release`.
*   Before evicting a cache line, you should call the `before_eviction` function on that line. The exception to this is when you are replacing is invalidated cache line. Do not call `before_eviction` on invalid ones.
*   No memory leak is allowed.

### Spec on each function

*   function `cashier_init`: create a cache simulator with a set of parameters.
    *   parameters: `config: struct cache_config`
        *   specifies the number of bits of the address space `config.address_bits`.
        *   the number of cache lines in the cache `config.lines`.
        *   the size of each cache line `config.line_size`.
        *   the associativity, i.e., the number of ways in the set-associative cache. `config.ways`.
        *   the configuration validity is guaranteed
            *   `line_size` is a power of 2
            *   `lines` is a power of 2
            *   `ways` is a power of 2, `ways` is a factor of `lines`.
    *   returns: `struct cashier*` address to the initialized cache simulator.
    *   note
        *   You should return `NULL` on error.
        *   You don’t need to validate the parameters.
        *   Make sure no memory leak on error.
*   function `cashier_release`: release the resources allocated for the cache simulator.
    *   parameters: `cache: struct cashier *` address to a cache simulator created by `cashier_init`.
    *   returns: NONE.
    *   note
        *   `NULL` parameters is allowed.
        *   Make sure to eliminate possibility of memory leak.
*   function `cashier_read`: read one byte of data at a specific address.
    *   parameters:
        *   `cache: struct cashier *` a cache simulator created by `cashier_init`, not `NULL`.
        *   `addr: uint64_t` the byte address.
        *   `byte: uint8_t *` used to store the result, guaranteed not to be a valid writable memory address.
    *   returns: `true` on cache hit, `false` on cache miss.
    *   note:
        *   To access memory on miss, always use `mem_read` and `mem_write`. Do not directly dereference `addr` or `SIGSEGV` we be raised.
        *   Call `before_eviction` if you have to evict a cache line.
        *   Placing the cache line at a previously invalidated slot is not considered as an eviction.
*   function `cashier_write`: write one byte of data at a specific address.
    *   parameters:
        *   `cache: struct cashier *` a cache simulator created by `cashier_init`, not `NULL`.
        *   `addr: uint64_t` the byte address.
        *   `byte: uint8_t` the data to be written into memory.
    *   returns: `true` on cache hit, `false` on cache miss.
    *   note:
        *   To access memory on miss, always use `mem_read` and `mem_write`. Do not directly dereference `addr` or `SIGSEGV` we be raised.
        *   Call `before_eviction` if you have to evict a cache line.
        *   Placing the cache line at a previously invalidated slot is not considered as an eviction.
        *   Write-back is expected rather than write-through.

Testing on local machine
------------------------

A little feedback on each submission is provided for your convenience.  
With that said, please **do NOT** use Autolab OJ for debugging.  
The grading feedback will not be sufficient for identifying and fixing bugs in your program.

We strongly recommend you to construct your own test cases for validating the solution on your local machine.  
You really should do that every time before submission.

The `dot_test` function in `main.c` servers as an example of the test cases.

You are **not** expected to change the environmental interfaces.

Code quality requirements
-------------------------

We ask you to write code of high quality and easy to maintain. It is hard to check code quality automatically. So we only check if your code bears enough number of comments.  
To put it simple, we ask you to include at least one line of comment in every six non-empty lines of code (preprocessing directives such as `#define` and `#if`, lines only contains brackets `{ } ( ) [ ]` are not counted towards code lines).  
Submission with inadequate comments will receive zero score.

Submission
----------

Create a tarball `hw7.tar` containing your `cashier.c` and upload it to Autolab.  
The `hw7.tar` target in `Makefile` can do this job for you.

Contacts
--------

You are encouraged to post the challenges you encountered on piazza. We will appreciate it if you can document the process of overcoming them. A mere sentence _solved_ is not going to help anyone.

Contact [pengcheng2](mailto://pengcheng2ATshanghaitech.edu.cn) if you have any issue regarding this homework.  
Bug reports and language use fix-ups are welcome.