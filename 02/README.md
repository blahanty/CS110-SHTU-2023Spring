CS110 homework 2: Programming in C & GNU toolchains
===================================================

[download the starter code](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/hw/academic_integrity_and_data_integrity.tar)

Brief
-----

> No prerequisite regarding file system is required.  
> We do not include a reference count for memory safety guarantee

In this homework, you will build a C/C++ program that simulates a file explorer(e.g. _finder_ on MacOS, _explorer.exe_ on Windows, _dolphin_ in KDE plasma and _nautilus_ in Gnome DE). We have provided a template for you and some of the functions have already been implemented. You should implement all functions in `file.c` and `dir.c` and part of the `explorer.c`. There is also bonus functions in `explore.c`. We will give you some handy testers. You can use them to write your own test case. The example are given in `main.c`.

Make sure that the compilation of the source code generates no errors/warnings and conformity to C89 standard. We have provided a `Makefile`, you can run make to compile your code. You can run make memcheck to check whether there is any memory leak (you will need to install valgrind first). You can also run make clean to delete all compiled files.

Function description
--------------------

### M.I.S.C.

*   enumeration type alias type `bool`: see `bool.h`.  
    ISO C89 i.e., ANSI C does not ship with a builtin boolean type.
*   function pointer type synonym `open_func`: see `file.h:21`.  
    The function takes a file and return nothing,  
    representing an external application used to process file of certain type.
*   function pointer type synonym `find_func`: see `explorer.h:142`.  
    The function takes a file and the path to the file.  
    Only used in recursive search bonus task.

### Entities

**Notation**: We use the extended regular expression `A{X,Y}` to represent the set `{AX, AY}`.

*   **File**: `file.{h,c}`  
    A file is simply a extensible byte sequence. File objects shall provide basic I/O interfaces.  
    Every file got its file type which determines the external application used to process this file.
*   **Directory**: `dir.{h,c}`  
    A directory is a collection of files and directories.
*   **Node**: `node.{h,c}`  
    A node in the file tree is either a file or a directory.  
    `struct file` and `struct directory` does not store any metadata (e.g. file and directory `name` and `path`). The metadata of a file/directory is held the node into which the file/directory is embedded.
*   **File Explorer**: `explorer.{h,c}`  
    User interface and client side library too the file tree. File explorer shall forward every operation to `node`, `dir` or `file`.  
    Explorer keep track of the current working directory and allow traveling in the file tree via `chdir` and `cdpar`.  
    Explorer also support file/directory creation/deletion as well as the basic bytes I/O.

Your tasks
----------

For task 1, 2 and 3, you are required to complete the C source code files.  
For task 4 and 5, you are required to modify the makefile.

### Task 1: Implementing Extensible File

In our explore simulator, the size of the file is exactly equal to the memory reserved for the file. When more bytes need to be stored in the file, we will reallocate more memory for the new data.

In this task, you are required to implement all the empty functions in `file.c`. `file_new(int, char *)` and `file_release(struct file *file)` are provided by us and you are not suggested to change them. Please read the description in the header file carefully and follow the instruction. Deal with the memory allocation carefully.

### Task 2: Implementing Directory Tree

The directory in the simulator is tree-structured. Each dir node will have several subordinates(children), each of them is either a dir node or a file node. A file node does not contain any subordinate. We identify the nodes by their names, so all the nodes with the same parent should have different names.

In this task, you are required to implement all the empty functions in `dir.c`. `dir_new(int, char *)` and `dir_release(struct file *file)` are provided by us and you are not suggested to change them. Please read the description in the header file carefully and follow the instruction. Deal with the memory allocation carefully.

### Task 3: Completing the file explorer

The explorer contains a directory tree, one pointer points to the root of the tree and one pointer records the current working directory(cwd). The cwd is initially set to the root. The explore also maintain a list of supported file types and the functions to open them (e.g. pdf reader for .pdf in real world, music player for .wav). Before opening a file, its type and corresponding function should be submitted to the expolorer. Our explorer only supports limited number of types.

In this task, you are required to implement

*   `bool explorer_cdpar(struct explorer *)`
*   `bool explorer_chdir(struct explorer *, const char *)`
*   `bool explorer_support_filetype(struct explorer *, open_func, int)`
*   `bool explorer_open(const struct explorer *, const char *)`

in `explore.c`.

The following functions are provided by us and you are not suggested to change them.

*   `struct explorer *explorer_new(void)`
*   `void explorer_release(struct explorer *)`
*   `bool explorer_read(const struct explorer *, const char *, int, int, char *)`
*   `bool explorer_write(struct explorer *, const char *, int, int, const char *)`
*   `bool explorer_create(struct explorer *, char *, int)`
*   `bool explorer_mkdir(struct explorer *, char *)`
*   `bool explorer_delete(struct explorer *, const char *)`
*   `bool explorer_contain(struct explorer *, const char *)`

Please read the description in the header file carefully and follow the instruction. Deal with the memory allocation carefully.

#### Bonus task

We would like our explore to support search and have a flexible configuration on the result. Implementing `explorer_search_recursive(struct explorer*, const char *, find_func)`. Note that the bonus only worth 1 point. It will be fine if you do not feel like to finish it.

### Task 4: creating and linking static library

To allow code reuse, we shall provide the explorer as a _static library_.

Your task is to add a static library (archive of objects) target and a statically linked executable target in the `Makefile`. More precisely:

*   In `Makefile`, add one target `libexplorer.a`.  
    When building the target, generate a static library `libexplorer.a` from the `.c` source code files.
*   In `Makefile`, add one target `test.static.out`.  
    When building the target, link `libexplorer.a` with other objects to obtain an statically linked standalone ELF executable `test.static.out`.

You may use the following commands to verify your result

*   `./test.static.out`: run the executable, should see no error coming out.
*   `ar t libexplorer.a`: list the files included in the static library.
*   `file libexplorer.a`: inspect the file type.
*   `nm test.static.out`: list the symbols in the ELF file, you should see a `T` which stands for text segment in front of every file/directory/explorer function.

### Task 5: creating and linking dynamic library

Providing a _dynamic library_ of the explorer is a favorable choice as it enables code reuse with less memory and disk footprint.

Your task is to add a dynamic library (shared objects) target and a dynamically linked executable target in the `Makefile`. More precisely:

*   In `Makefile`, add one target `libexplorer.so`.  
    When building the target, generate a dynamic library `libexplorer.a` from the `.c` source code files.
*   In `Makefile`, add one target `test.dynamic.out`.  
    When building the target, link `libexplorer.so` with other objects to obtain an ELF executable `test.static.out` with dynamically loaded dependencies.

You may use the following commands to verify your result

*   `./test.dynamic.out`: run the executable, should see no error coming out.
*   `file libexplorer.so`: inspect the file type.
*   `ldd test.dynamic.out`: print the shared object dependencies.
*   `nm test.dynamic.out`: list the symbols in the ELF file, you should see a `U` in front of every file/directory/explorer function.

You may take a look at [this blog post](https://www.herongyang.com/Linux-Apps/GCC-shared-to-Build-Dynamic-Library-Files.html) for a concise instruction.

Code quality
------------

*   code readability: Write perspicuous and concise code. ~Make it better place. For you and for me and the entire CS110 staff.~
*   comments:
    1.  You should include two lines of comments for every eight non-blank lines.
    2.  Interface doc comment before every C function/macro function is a must. Make sure to include sufficient doc explaining the parameters, return values and behaviors. You may want to mimic the doxygen annotations.
    3.  The comments have to be meaningful English sentences.
    4.  Your submission is ignored if the source code does not contain enough comments.
    5.  We will manually check your code after the deadline. You will receive a score deduction if you violated any of the rules.
*   resources management:
    1.  No memory leak is allowed.
    2.  `valgrind` and address sanitizer is used to detect memory leak on runtime.
    3.  You will lose 10% of your scores if a memory leak is spotted.
*   UB (undefined behaviors):
    1.  Avoid UBs and platform/implementation defined behaviors when writing code.
    2.  Undefined behaviors sanitizer is used to detect UBs on runtime.
    3.  You will lose 10% of your scores if an UB is detected.

Submission
----------

Submit a tarball named `academic_integrity_and_data_integrity.tar` containing `Makefile`, `node.c`, `dir.c`, `file.c` and `explorer.c`.  
You may use the following command to create such archive file.

```
# make sure that these files are indeed situated in the current working directory 
tar cvf academic_integrity_and_data_integrity.tar \
    Makefile \
    node.c \
    dir.c \
    file.c \
    explorer.c
```

Contacts and feedbacks
----------------------

Still have any question? Post it on piazza!

Contact [TA: Cheng Peng](mailto://pengcheng2@shanghaitech.edu.cn) if you have any problem regarding this homework assignment.

Having a strong opinion regarding the homework? You may

1.  Post a note on piazza.
2.  Send an email/IM message to any TA or the professors.
3.  Still having a strong opinion? Add a comment on [CourseBench](https://coursebench.geekpie.club/about).