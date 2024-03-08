Lab 9
-----

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/index.html) [ShanghaiTech University](http://www.shanghaitech.edu.cn/) 

Download the starter code [here](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab9/lab9.tar)

Introduction to SIMD
--------------------

[SIMD](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data) makes a program faster by executing the same instruction on multiple data at the same time. In this lab, we will use [Intel Intrinsics](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html) to implement simple programs.

Part 1: Vector addition
-----------------------

In this part, you will implement a vector addition program using SIMD. Please "translate" naive\_add() to simd\_add().  
You may use the following intrinsics, search in the [Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html):

*   \_mm\_loadu\_si128
*   \_mm\_storeu\_si128
*   \_mm\_add\_epi32

Try to tell the difference of the following "load" intrisics:

*   \_mm\_load\_si128
*   \_mm\_loadu\_si128
*   \_mm\_load\_pd
*   \_mm\_load1\_pd

Part 2: Matrix multiplication
-----------------------------

In this part, you will implement a matrix multiplication program using SIMD. Please "translate" naive\_matmul() to simd\_matmul().  
You may use the following intrinsics:

*   \_mm\_setzero\_ps
*   \_mm\_set1\_ps
*   \_mm\_loadu\_ps
*   \_mm\_add\_ps
*   \_mm\_mul\_ps
*   \_mm\_storeu\_ps

Explain why this makes the program faster.

Part 3: Loop unrolling
----------------------

Read Wikipedia and try to understand the concept of loop unrolling:

*   [Loop unrolling](https://en.wikipedia.org/wiki/Loop_unrolling)

Implement loop\_unroll\_matmul() and loop\_unroll\_simd\_matmul(), explain the performance boost they brought.

Part 4: Compiler optimization
-----------------------------

Run `make test`, explain why `-O3` makes the program much faster.  
For checkup: Put this piece of code into [godbolt.org](https://godbolt.org/) , compile them with a risc-v compiler, and tell the difference between `-O0` and `-O3`.  
```
int a = 0;

void modify(int j) {
    a += j;
}


int main() {
    for (int i = 0; i < 1000; i++) {
        a += 1;
    }

    for (int i = 0; i < 1000; i++) {
        a += i;
    }

    return a;
}
```

* * *

Suting Chen <`chenst` AT `shanghaitech.edu.cn`\>  

  
Last modified: 2023-04-12