CS110 Homework 6: ThreadPool
============================

Overview
--------

In this homework, you will implement a naive [thread pool](https://en.wikipedia.org/wiki/Thread_pool). A thread pool is a very common design, aims to reuse threads to avoid the overhead of creating and destroying threads.

Getting started
---------------

Download the starter code [here](https://toast-lab.sist.shanghaitech.edu.cn/cs110-23s-hw6.tar).

### Ringbuffer

Before start working with `pthread`, we must first learn a data structure named [`ringbuffer`](https://en.wikipedia.org/wiki/Circular_buffer).

A ringbuffer is a circular queue with a fixed maximum size. See `ringbuffer.h`

![ringbuffer_pic_wikipedia](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/hw/hw6/Circular_Buffer_Animation.gif)

*   `ringbuffer_t *ringbuffer_create(size_t size);`  
    Create a ringbuffer with size `size` and return the pointer of ringbuffer created.  
    Return NULL anything failed in the process.
*   `void ringbuffer_destroy(ringbuffer_t *ringbuffer);`  
    Destroy the ringbuffer `ringbuffer`. Release all the memory allocated.
*   `bool ringbuffer_is_empty(ringbuffer_t *ringbuffer);`  
    Return `true` if the ringbuffer `ringbuffer` is empty, otherwise return `false`.
*   `bool ringbuffer_is_full(ringbuffer_t *ringbuffer);`  
    Return `true` if the ringbuffer `ringbuffer` is full, otherwise return `false`.
*   `bool ringbuffer_push(ringbuffer_t *ringbuffer, threadpool_task_t value);`  
    Push the value `value` into the ringbuffer `ringbuffer`.  
    Return `true` if the push is successful, otherwise return `false`.
*   `bool ringbuffer_pop(ringbuffer_t *ringbuffer, threadpool_task_t *value);`  
    Retrieve the value from the ringbuffer `ringbuffer` and store it in `value`.  
    Return `true` if the operation is successful, otherwise return `false`.

### Threadpool

Now we are ready to implement the threadpool. See `threadpool.h`

We harness ringbuffer to store tasks added with `threadpool_add_task`.

Instead of directly initiate task functions, which cause thread creation overhead for every task, we run a worker function (`static void *threadpool_thread(void *threadpool);`) as a thread, it remains idle until destroyed by signals or fetch a new task.

*   `threadpool_t *threadpool_create(size_t thread_count, size_t queue_size, bool sync);`  
    Create a threadpool with `thread_count` threads and a ringbuffer with size `queue_size`.  
    Return the pointer of the threadpool created. Return NULL if anything failed in the process.
*   `bool threadpool_destroy(threadpool_t *threadpool);`  
    Destroy the threadpool `threadpool`. Release all the memory allocated.  
    If there are still remaining tasks in the ringbuffer:
    
    *   If `sync == true` when the threadpool was created, wait until all tasks are finished.
    *   Else, abort all waiting tasks immediately and return as soon as the threadpool finish its running tasks.
    
    Return `true` if the threadpool is successfully destroyed, otherwise return `false`.
*   `bool threadpool_add_task(threadpool_t *pool, void (*func)(void *), void *args);`  
    Add a task with function `func` and argument `args` into the threadpool `pool`.  
    Return `true` if the task is successfully added, otherwise return `false`.  
    The function should return `false` if the threadpool is full.

Submission
----------

Simply run `make submit`, and submit the tarball named `hw6_threadpool.tar` generated to autolab.

Guidance
--------

### Manual you should read (stop it by clicking on the pic)

![ringbuffer_pic_wikipedia](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/hw/hw6/rtfm.png)

*   [`pthread_create`](https://linux.die.net/man/3/pthread_create)
*   [`pthread_mutex_lock`](https://linux.die.net/man/3/pthread_mutex_lock)
*   [`pthread_mutex_unlock`](https://linux.die.net/man/3/pthread_mutex_unlock)
*   [`pthread_exit`](https://linux.die.net/man/3/pthread_exit)
*   [`pthread_join`](https://linux.die.net/man/3/pthread_join)

They might be a little intimidating, but rather helpful for debugging!

### How much code will I need to write?

One possible solution:

ringbuffer.c | 18 ++++--------------  
threadpool.c | 42 +++++++++++++-----------------------------  
2 files changed, 17 insertions(+), 43 deletions(-)

Note that there are lots of `TODO`s in the code, which is the cause of most of the deletion.  
You can generate your own statistics by running `diff -uN template answer | diffstat`, where `template` and `answer` are the directories of template given and your answer respectively.

* * *

Suting Chen <`chenst` AT `shanghaitech.edu.cn`\>  
Qing Xu <`xuqing2` AT `shanghaitech.edu.cn`\>  

  
Last modified: 2023-04-09