Homework 3
==========

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/index.html) [ShanghaiTech University](http://www.shanghaitech.edu.cn/)

Introduction
------------

In this homework, you need to write a RISC-V program to implement a simple algorithm. You need to find the **longest palindromic substring** of a given string. The template can be found [here](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/hw/hw3template.tar)

Here is a simple example:

```
String : abbsdcds
```

The longest palindromic substring should be `sdcds`, the length is `5`.

**The program has to use at least 1 function call which follows the standard RISC-V conventions.**

Input
-----

A reference input is already provided to you in the `input.S` file. Input is provided to you in binary format to alleviate the burden of parsing and dealing with Venus's expermintal file system.

```
.data
.globl str
str:
        .string "abbbbb"
# the answer should be 5
```

Task
----

You should write a function called `LPS` in `func.S`, which returns the length of the longest palindromic substring. Our auto-grader will call this function to get answer.

```
# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.globl LPS
LPS:
# YOUR CODE HERE
```

Arguments and return values
---------------------------

*   You do not need to handle any input/output. The only thing your function should do is returning the correct answer.
*   The start address of the string will be passed to your function. Argument passing will follow RISC-V calling convention.
*   Make sure that your function returns the result in a correct way.

Running
-------

Make sure that `main.S`,`func.S` and `input.S` reside in the same directory. To run your program locally

```
java -jar venus-jvm-latest.jar main.S
```

To debug your program, you can modify the content of `input.S` to change the test case.

Test
----

The command that we use to test your program's correctness is

```
diff <your_output> <reference_output>
```

You can also test your result using this command. Be sure to calculate your `reference_output` correctly!

Calling Convention
------------------

Make sure that you strictly follow the calling convention defined in [RISC-V Green Card](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/lecture_notes/riscvcard.pdf). We will check this in EVERY test case.

Execution
---------

We will test your program using RISC-V emulator [venus](http://autolab.sist.shanghaitech.edu.cn/venus/). You probably want to read [this](https://github.com/kvakil/venus/wiki) before you started.

Tips
----

*   Handwritten assembly are postfixed with extension `.S` to distinguish from compiler generated assembly `.s`
*   While being a strong proponent for python's cult of `space`, I suggest you indent using `tab`s. Set your editor's tabwidth to 8 for best visual experience.
*   Learn save and load from memory using RISC-V
*   It is helpful writing in C first
*   Write comments
*   The test cases are very friendly! Don't focus too much on the edge cases, focus on the correctness on the common cases.
*   Information of `ecall` can be found [here](https://github.com/ThaumicMekanism/venus/wiki/Environmental-Calls)

Submission
----------

*   You need to have **meaningful** comments not less than 25% of the total lines of code you wrote. A comment is defined by a sentence followed by `#`.
*   You should submit exactly **one file**, `func.S`. Any other file found will result in a score of zero.
*   DeadLine: Mar 18, 11:59 PM

* * *

Last Modified: 2023/3/2