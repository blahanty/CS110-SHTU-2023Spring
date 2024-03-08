Lab 10
------

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/index.html) [ShanghaiTech University](http://www.shanghaitech.edu.cn/)  

Objectives:
-----------

*   Learn about basic OpenMP directives
*   Write code to learn two ways of how `#pragma omp for` could be implemented. Learn about false sharing.
*   Learn about basic multi-processing programming

Setup
-----

Download source code from [here](https://toast-lab.sist.shanghaitech.edu.cn/lab_10.zip)

Multi-threading programming using OpenMP
----------------------------------------

OpenMP stands for Open specification for Multi-Processing. It is a framework that offers a C interface. It is not a built-in part of the language – most OpenMP features are directives to the compiler.

Benefits of multi-threaded programming using OpenMP include:

*   Very simple interface allows a programmer to separate a program into serial regions and parallel regions.
*   Convenient synchronization control (Data race bugs in POSIX threads are very hard to trace).

In this lab, we will practice on basic usage of OpenMP.

### Exercise 1 - OpenMP Hello World

Consider the implementation of Hello World (hello.c):

```
int main ()
{
  #pragma omp parallel
  {
    int thread_ID = omp_get_thread_num ();
    printf (" hello world %d\n", thread_ID);
  }
  return 0;
}
```

This program will fork off the default number of threads and each thread will print out "hello world" in addition to which thread number it is. You can change the number of OpenMP threads by setting the environment variable OMP\_NUM\_THREADS or by using the [omp\_set\_num\_threads](https://gcc.gnu.org/onlinedocs/libgomp/omp_005fset_005fnum_005fthreads.html) function in your program. The #pragma tells the compiler that the rest of the line is a directive, and in this case it is omp parallel. omp declares that it is for OpenMP and parallel says the following code block (what is contained in { }) can be executed in parallel. Give it a try:

```
$ make hello && ./hello
```

If you run ./hello a couple of times, you should see that the numbers are not always in numerical order and will most likely vary across runs. Think about the reason and explain to your TA.

It is also vital to note that the variable thread\_ID is local to a specific thread and not shared across all threads. In general with OpenMP, variables declared inside the parallel block will be private to each thread, but variables declared outside will be global and accessible by all the threads.

### Exercise 2 - Matrix Multiplication

Matrix multiplication is a common operation in scientific computing and machine learning. In this exercise, we will optimize a matrix multiplication implementation using OpenMP. The matrix multiplication is implemented in matmul.c.

Your task is to optimize matmul.c (speedup may plateau as the number of threads continues to increase). To aid you in this process, two useful OpenMP functions are:

*   [int omp\_get\_num\_threads()](https://gcc.gnu.org/onlinedocs/libgomp/omp_005fget_005fnum_005fthreads.html)
*   [int omp\_get\_thread\_num()](https://gcc.gnu.org/onlinedocs/libgomp/omp_005fget_005fthread_005fnum.html)

Divide up the work for each thread through two different methods (write different code for each of these methods):

1.  First task, **slicing**: have each thread handle adjacent rows: i.e. Thread 0 will compute the rows at indices i such that i % omp\_get\_num\_threads() is 0, Thread 1 will compute the rows where i % omp\_get\_num\_threads() is 1, etc.
2.  Second task, **chunking**: if there are N threads, break the matrices into N contiguous chunks along the first dimension (the rows), and have each thread compute the product of the chunk of matrix A and the entire matrix B.

Hints:

*   Use the two functions we listed above somehow in the for loop to choose which rows each thread handles in the slicing method.
*   You may need a special case to prevent going out of bounds for matmul\_optimized\_chunks. Don't be afraid to write one.
*   Be careful about cache line alignment and false sharing. To avoid false sharing, each thread should have its own output buffer to store the computed rows.

For this exercise, we are asking you to manually split the work amongst threads since this is a common pattern used in software optimization. The designers of OpenMP actually made the #pragma omp for directive to automatically split up independent work. Here is the function rewritten using it. **You may NOT use this directive in your solution to this exercise**.

```
void matmul (double \*a, double \*b, double \*c)
{
  #pragma omp parallel for 
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      for (int k = 0; k < MATRIX_SIZE; k++) {
        c[i * MATRIX_SIZE + j] += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
      }
    }
  }
}
```

Test the performance of your code with:

```
$ make matmul && ./matmul
```

#### Checkoff

*   Show the modified code for both the slicing and chunking methods in matmul.c.
*   Describe the performance differences between the methods you implemented and try to analyze the reason(Run more times to find a common pattern instead of just running once).
*   Explain why using OpenMP may not necessarily lead to optimal performance on a single compute node with multiple cores.
*   Bonus: Implement an additional optimization and discuss its impact on performance.

### Exercise 3 - Dot Product

The next task is to compute the dot product of two vectors. At first glance, implementing this might seem not too different from v\_add, but the challenge is how to sum up all of the products into the same variable (reduction). A sloppy handling of reduction may lead to **data races**: all the threads are trying to read and write to the same address simultaneously. One solution is to use a **critical section**. The code in a critical section can only be executed by a single thread at any given time. Thus, having a critical section naturally prevents multiple threads from reading and writing to the same data, a problem that would otherwise lead to data races. One way to avoid data races is to use the critical primitive provided by OpenMP. An implementation, dotp\_naive in dotp.c, protects the sum with a critical section.

Try out the code (make dotp &&./dotp). Notice how the performance gets much worse as the number of threads goes up. By putting all of the work of reduction in a critical section, we have flattened the parallelism and made it so only one thread can do useful work at a time (not exactly the idea behind thread-level parallelism). This contention is problematic; each thread is constantly fighting for the critical section and only one is making any progress at any given time. As the number of threads goes up, so does the contention, and the performance pays the price. Can we reduce the number of times that each thread needs to use a critical section?

In this exercise, you have 2 tasks:

1.  Fix the performance problem without using OpenMP's built-in Reduction keyword.
2.  Fix the performance problem using OpenMP's built-in Reduction keyword. (Note that your code should no longer contain #pragma omp critical)

#### Checkoff

*   Show the TA your manual fix to dotp.c that gets speedup over the single threaded case.
*   Show the TA your Reduction keyword fix for dotp.c, and explain the difference in performance.

* * *

Zongze Li <`lizz` AT `shanghaitech.edu.cn`\>  

  
Last modified: 2023-04-26