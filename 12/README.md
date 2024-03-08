Lab 12
------

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/) [ShanghaiTech University](http://www.shanghaitech.edu.cn/)  

Objectives:
-----------

*   Get familiar with Longan Nano
*   Check if the Nano that we give you hardware is working
*   Understand how to program RISC-V code for Longan Nano

About Longan Nano
-----------------

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University_files/nano.png)

Longan Nano is a mini development board based on a RISC-V 32-bit core. And that's why we're using this - Project 4 is based on this board.

The official documentation can be found [here](https://longan.sipeed.com/en/).

Hardware connection
-------------------

#### Note: Likely the board is already setup for you. If not you'll have to do it - but you don't have to do it in lab.

### Board

The Hardware we distributed to you contains not only the Longan Nano board, but also a board contains peripherals:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University_files/board.jpg)

Please connect the Longan Nano board and the peripheral board like the picture below.

#### Note: **The direction of inserting the Longan Nano board cannot be changed. A wrong direction will lead to severe damage!**

**There are two different version of the Longan Nano board. Please carefully check the version of yours**

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University_files/connection.jpg)

**The picture above is the older version, so called version 0**

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University_files/connection1.jpg)

**The picture above is the newer version, so called version 1**

The peripheral board provides three directional buttons(right,left and down) , a center button and a general button(SW1). The Up button and SW2 cannot be used (Press them will cause the screen to crash).

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University_files/board-des.jpg)

Note:

*   You can push the directional buttons and press the center button.
*   The type-c interface on the peripheral board can only be used for power supply, not for downloading programs

Environment Setup
-----------------

First, since Longan Nano is based on RISC-V architecture and our PC is not, we have to do [cross compile](https://en.wikipedia.org/wiki/Cross_compiler) in order for our program to run on this board. Luckily, Longan Nano has official support for a third-party platform PlatformIO and it will handle all of these.

To install PlatformIO, make sure you have installed Python3 and pip3. Run this in command: (you may need to replace python with python3 or something)

```
python -m pip install platformio
```

Also, you can download the starter [here](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University_files/lab12-starter.zip).

To compile the example Project, run this in project folder:

```
python -m platformio run
```

It may take quite a while when first run, as it will automatically install necessary dependencies. The output file will be `.pio/build/sipeed-longan-nano/firmware.bin` together with other files.

To download the executable file to the development board, you need to first connect the board to your computer via a Type-C cable; then follow the below steps to enter DFU mode of the board (so that you can download files to the board.)

*   1\. Hold BOOT0 button (and do not release)
*   2\. Hold RESET button (and do not release)
*   3\. Release RESET button
*   4\. Release BOOT0 button

It may require several attempts for this process to succeed. If done successfully, the screen of board will be frozen or become pure black, and you'll be able to detect this device via the following command:

```
./dfu-util -l | grep 28e9:0189
```

If there is some output, it means that your board has entered dfu mode successfully and is found by your computer. Please don't use the `dfu-util` provided by `apt-get` or PlatformIO, as it's known to have bugs.

Then, you can download the executable to the board:

```
./dfu-util -a 0 --dfuse-address 0x08000000:leave -D .pio/build/sipeed-longan-nano/firmware.bin
```

This command may raise an error such as "Error during download get\_status" in the end; but it actually doesn't matter. If you meet some problems like "Permission denied" in the above steps, please use `sudo`.

After it's finished, press RESET button. Now you can see 5 lines of "Hello World" on the screen!

We've also provided you a Makefile in the example project folder (You may need to modify its first line). Run `make` to compile the source code and `make download` to download the executable to board.

### Exercise 1 - Test the hardware

To test whether the hardware works, we provide you a program for self-testing.

In order to be compatible with both devices, we have introduced BOARD\_VER in `utils.h` macro to identify the type of board. Please confirm the board before each burn\_ VER is set to the same value as the version you are using (0 or 1)

To enable the self-test program, please change the macro BOARD\_TEST\_ON at the begin of `main.c` to 1

```
#define BOARD_TEST_ON 1
```

Then compile and download the program. If the hardware works well, you can see some ouput when you press the buttons

#### Checkoff:

*   Show TA that your hardware works well or ask for a new one.

### Exercise 2 - Draw something in RISC-V!

In the following Project 4, you are asked to implement main functionalities in RISC-V. However, you can call C functions in RISC-V, just like calling a RISC-V function; conversely you can also call a RISC-V function in C.

To implement a RISC-V function that can be called via C, use `a0-a7` as input parameters sequentially, and use `a0` as output. For example:

```
// C code
int a = add(3, 4, 5); // a = 12

# RISC-V code
# ...
add t0, a0, a1  # a0 - 3, a1 - 4
add t0, t0, a2  # a2 - 5
mv a0, t0       # a0 - return value
# ...
ret
```

Another example for RISC-V code calling C functions:

```
# RISC-V code
li a0, 1;
li a1, 2;
jal sub      # a0 = -1 after this step

// C code
int sub(int a, int b) // a - 1, b - 2
{
	return a - b;
}
```

Most of RISC-V grammar here is the same as in Venus. You don't need to somehow "include" C headers in RISC-V files as PlatformIO will automatically detect and locate them.

Please read `/src/lcd/lcd.c` carefully. You'll definitely need many of those functions in Project 4.

For this lab, you need to modify example.S and call some LCD functions in example.S to draw a circle, a square and some arbitrary characters on screen. You can call function Get\_BOOT0() in `utils.c` to see whether BOOT0 is pressed.

#### Checkoff

*   Show the TA your modified code for example.S .
*   Compile the program and download to the board. Show the TA that after pressing the BOOT0 button, the figures appear on the screen.

* * *

Siting Liu <`liust` AT `shanghaitech.edu.cn`\>

Haoran Jiang <`jianghr1` AT `shanghaitech.edu.cn`\>

Cheng Peng <`pengcheng2` AT `shanghaitech.edu.cn`\>

Linjie Ma <`malj` AT `shanghaitech.edu.cn`\>

Last modified: 2023-05-12