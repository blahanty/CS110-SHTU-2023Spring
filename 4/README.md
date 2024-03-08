Project 4: Longan Nano minigame
=========================

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/index.html) [ShanghaiTech University](https://www.shanghaitech.edu.cn/)

Brief
-----

As CS110 is coming to an end, it's time to put the skills we've learned into practice.

In this project, you will code in `C` language and RISC-V assembly to implement a Sokoban game. You will also use `platformIO` to cross-compile and generate a program for the Longan Nano development board.

While we do have a reference implementation, you are not required to fully replicate it. Once you have completed the basic requirements, you are free to add or modify more features according to your preferences.

We hope you enjoy playing with Longan Nano in this project. GL; HF.

*   [Link to a Sokoban game](https://www.youtube.com/watch?v=j0bWcyVneGE)
    
*   [Link to a video demo on the reference implementation](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/project/demo.mp4)
    

Get started
-----------

Refer to the instructions provided in [CS110 lab 12](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University.html) to familiarize yourself with the hardware you have been assigned, setup the `platformIO` development environment following the instructions of [platformIO official documentation](https://docs.platformio.org/en/latest/what-is-platformio.html).

We have provided an initial code for the project. Get the code from [gitlab:project4\_starter](https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s/project4_starter). Try to compile the project with `platformio` and flash it onto the Longan Nano board.

Play around with the starter program:

*   input keys (see `utils.c` and `utils.h`)
    
    *   The button connected to GPIO port 0 is regarded as the _left direction key_.
        
    *   The button connected to GPIO port 2 is regarded as the _right direction key_.
        
    *   The button connected to GPIO port 4 is regarded as the _up direction key_.
        
    *   The button connected to GPIO port 1 is regarded as the _down direction key_.
        
*   The origin of the screen is on the left-top corner.
    
*   X-axis: the horizontal axis.
    
*   Y-axis: the vertical axis.
    

Your Task
---------

As mentioned before, we will have a few _required_ functionalities you must have. After meeting the basic requirements, you can further extend the game as you wish.

We've also provided a few exemplary extension for you, if you are uncertain about what to do, you can try to implement them

### (Required) Input Keys Debouncing

One of the requirements for your implementation is to prevent button bouncing.

When playing the initial program, you may have noticed that when holding down a button (e.g., the left button), the blocks on the screen will continuously move to the left, which is not suitable for a game involving discrete movements like `Sokoban`. Button bouncing may lead to unintended movements annoying the player.

To address this issue, we ask you to modify the `Get_Button(int ch)` function in `utils.c` to ensure that: _if a button is triggered at time $t\_0$ then the button cannot be triggered again before $t\_0+0.3$._

### (Required) Displaying a Character with Image

The next requirement is to display a sprite.

Basic graphic elements and color blocks can be dull. We require you to attach images on game objects to provide richer display effects.

### (Required) Level Selection Scene

After meeting the first two non-functional requirements, you will face the first real functional requirement, level selection.

Create **at least three levels** with different obstacle layouts.

*   Before starting the game, player can pick a level to play using the up and down buttons.
    
*   The number of boxes can also be adjusted with the left and right buttons.
    
*   When the player press the `BOOT` button, start the game and load the selected level.
    
*   **NOTE** Make sure that each level you designed is solvable.
    

### (Required) Game Play Scene

The most relevant part, game play:

The game is played on a `20 x 10` board with unmoveable obstacle, pushable boxes and target cells.

The player can move using the arrow keys, but cannot leave the map boundary or push obstacles. If a box is in front of the player and there is an empty cell behind it, the player will push the box one cell in the direction of movement. When a box is pushed onto a target cell, it will be displayed in a special way to distinguish it from other boxes.

The game should keep track of the number of moves made by the player and display it during gameplay. The level is considered completed when there is a box on each target cell.

Key points:

*   Use a sprite to represent the player. For other game objects such as the obstacles and empty cells you may simply use color boxes.
    
*   Boxes that are pushed onto target cells are displayed in a special manner so they can be distinguished form other boxes.
    
*   Ensure that player movements are limited by the game boundary and obstacles.
    
*   Record and display the number of moves made by the player during gameplay.
    

### (Optional) Incremental Rendering

You may have noticed that the screen flickers during animations. This is because the animation causes a full screen redraw, clearing of all pixels on the LCD display.

To address issue, you have to change the implementation of rendering so only pixels that have changed get updated.

### (Optional) Welcome Scene

Another additional feature, can be a welcome screen.

This welcome scene shall be displayed when the game is launched and will show a smooth animation along with the game's title. You can use any effect you like to make it look cool.

### (Optional) ScoreBoard Scene

To further extend the game, you may add a scoreboard to it.

After the player finishes an level, display a scoreboard for him. The player can go back to level selection by pressing any of the keys.

Recall that you are asked to have at least three levels for the game. You can draw a 3 by 3 table on the scene showing the lowest three records for completing each level.

### (Ultimate) Patrick's Paradox

Take a look at [Patrick's Paradox](https://www.patricksparabox.com/). It's cool.

Submission and Grading
----------------------

After you finish development, create a secret file `autolab.txt` in the repository and submit `autolab.txt` to Autolab OJ. We will test if your code can compile and bears enough comments.  
**NOTE** you will get a zero for this project if you failed to pass the test on Autolab.

Later, we will hold project interview for you guys to show the fascinating game. We are expecting:

1.  Meet with your TA. Compile and flash the project for Longan Nano on site.
    
2.  Demonstrate the required functionalities to your TA.
    
3.  Showcase any additional functionality you have implemented to your TA.
    

The grading policy of this project is listed as below:

*   Failed to compile on Autolab result in 0.
    
*   If the last submission on Autolab doesn't match with the latest version of your repository, you will also get 0.
    
*   By having a playable game scene, you can get 60% of the total score.
    
*   Successfully implementing **ALL** the required gives you 80% of the total score.
    
*   **At least two** additional functionalities for 20% of the total score.
    

Feedbacks
---------

Feel free to post your questions regarding this project on [Piazza](https://piazza.com/class/ld018f5gur54h4).

If you spot any error or confusing in this document, please contact [pengcheng2](mailto://pengcheng2ATshanghaitech.edu.cn).

* * *

This repo contains the framework code for your Project 4. 

In this project, you will be implementing a Pong game using the Longan Nano development board we've given to you. We've provided you a framework here.

You can also choose to implement your own minigame.

We hope this project will help you experience programming RISC-V on a real machine rather than in a simulator.

For full details of this project, please see course website

## PLEASE READ

The projects are part of your design project worth 2 credit points. As such they run in parallel to the actual course. So be aware that the due date for project and homework might be very close to each other! Start early and do not procrastinate.

## Download Framework

Download the framework from gitlab and Check if you already have the following files after pulling.

```
.
├── LICENSE
├── Makefile
├── README.md
├── dfu-util
├── include
│   ├── README
│   ├── fatfs
│   │   ├── diskio.h
│   │   ├── ff.h
│   │   ├── ffconf.h
│   │   └── tf_card.h
│   ├── gd32v_pjt_include.h
│   ├── gd32vf103_libopt.h
│   ├── lcd
│   │   ├── bmp.h
│   │   ├── lcd.h
│   │   └── oledfont.h
│   ├── systick.h
│   └── utils.h
├── platformio.ini
└── src
    ├── assembly
    │   └── example.S
    ├── fatfs
    │   ├── 00history.txt
    │   ├── 00readme.txt
    │   ├── ff.c
    │   ├── ffsystem.c
    │   ├── ffunicode.c
    │   └── tf_card.c
    ├── lcd
    │   └── lcd.c
    ├── main.c
    ├── systick.c
    └── utils.c
```
