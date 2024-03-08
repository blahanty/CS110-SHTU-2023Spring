Lab 6
-----

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/) [ShanghaiTech University](http://www.shanghaitech.edu.cn/)  
[Lab 5](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab5/lab5.html) Lab 6

Advanced Logisim
----------------

Setup
-----

*   Download source code from [here](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/lab06.tar)
*   Feel free to do each part as separate sub-circuits in the same Logisim file.
*   The following parts will introduce you to more advanced techniques/concepts in Logisim.

### Advanced Features

Here are three Logisim features that should both save you a lot of time and make your circuits look much cleaner.

#### 1\. Splitters

Splitters allow you to take a multi-bit value and split it up into smaller parts, or (despite the name) combine multiple values that are one or more bits into a single value.  
Here, we split the 4-bit binary number `0111` into `01` and `11`, then recombine it with `10` into the final 6-bit number `100111`:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/splitters.PNG)

Click on a splitter to get its menu in the sidebar. This menu determine the number of arms on your splitter and how many bits should go on each arm. For the circuit above, the left splitter's menu looks like this:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/splitters_left.PNG)

While the right splitter's menu looks like this:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/splitters_right.PNG)

**Notice that there's an option called** `facing`. You can use this to rotate your splitter. Above, see that the splitter on the right is facing West while the splitter on the left is facing East.

If you see an error wire that is orange, this means that your bit width in does not match your bit width out. Make sure that if you're connecting two components with a wire, you correctly set the bit width in that component's menu.

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/splitters_error.PNG)

#### 2\. Tunnels

A tunnel allows you draw an "invisible wire" to bind two points together. Tunnels are grouped by case-sensitive labels give to a wire. They are used to connect wires like so:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/Tunnels.PNG)

Some care should be taken as to which wires are connected with tunnels to which other wires, such as in this case:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/Tunnels_error.PNG)

We _strongly_ recommend you use tunnels with Logisim, because they make your circuits much cleaner looking, and therefore easier to debug.

#### 3\. Extenders

When changing the width of a wire, you should use a bit extender for clarity. For example, consider the following implementation of extending an 8-bit wire into a 16-bit wire:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/Extenders.PNG)

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/Extenders_0.PNG)

Compared to the splitter, the extender is easier to understand at a glance. This becomes especially helpful when working with complex circuits.

Additionally, consider the case of throwing out bits. Despite its name, an extender can also perform this operation:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/Extenders_1.PNG)

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/Extenders_2.PNG)

### Practice

#### 1\. Practice with Advanced features

We're going to construct a circuit that manipulates an 8-bit number.

#### Action Item

Complete the following steps to create the splitter circuit, and show this to your TA (remember to save). When you've completed the circuit, answer the question in the checkoff session.

1.  Open up the Exercise 1 schematic (`File->Open->ex1.circ`) and go to the empty Split circuit.
2.  Go to the `Wiring` folder and select the `Splitter` circuit. This circuit will take a wire and split it into a set of wires of smaller width. Conversely, it can also take many sets of wires and combine them into one.
    
3.  Change the `Bit Width In` property (bus width) to `8`, and `Fan Out` property to `8`. Connect the inputs to tunnels
    
    ![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab6_html&code/picture/practice1.PNG)
    
4.  Now, judge the number of '1s' in the input whether odd. You can use `XOR` gates to eath bit of input and route the output to `OUT1`.
    
5.  Then, judge the number of '1s' in the input whether greater than '0s'. You can use `Bit Adder and Comparator` from `Arithmetic` to implement this circuit and route the output to `OUT2` .
    
6.  We need to add a parity bit to the input to ensure that output has odd number of '1s', you are not allowed to use parity circuits from logisim `Gates`. Place another splitter with the proper properties to combine the parity bit and the input to `OUT3`.
    
7.  We consider the `OUT2` is the sign bit of input, changing input into 2's complement format. Place another splitter with the proper properties to combine the sign bit and the 2's complement to `OUT4`.
    

**Hint 1**: An unsigned comparator is required in `OUT2`.

**Hint 2**: The parity bit can be obtained from `OUT1` because when input has odd bit '1s', the parity bit will be 0, otherwise, the parity bit will be 1.

**Hint 3**: 2's complement transform is different according to sign bit.

#### Checkoff

*   Show your `ex1.circ` to your TA.
*   In `OUT4`, if we decide to find the negative input with 2's complement format. What the output of this format ?

### 2\. Rotate Right

With your knowledge and experience of splitters and multiplexers, you are ready to implement a non-trivial combinational logic block: `rotr`, which stands for "Rotate Right". The idea is that `rotr A,B` will "rotate" the bit pattern of input `A` to the right by `B` bits. So, if `A` were `0b1011010101110011` and `B` were `0b0101` (`5` in decimal), the output of the block would be `0b1001110110101011`. Notice that the rightmost `5` bits were rotated off the right end of the value and back onto the left end. In RTL, the operation would be something like `R = A >> B | A << (16 - B)`.

#### Action Item

Implement a subcircuit named `rotr` with the following inputs:

*   `A` (16-bit), the 16-bit input to be rotated
*   `B` (4-bit), the rotation amount (why 4 bits?) You can find the starter subcircuit in `ex2.circ`.

The output should be `A` rotated right by `B` bit positions, as outlined above. You are **NOT** allowed to use Logisim shifters in your solution, though all other combinational logic (MUXes, constants, gates, adders, etc.) is allowed. Logisim's built-in MUXes (find them under the `Plexers` menu) might be especialy helpful. Your solution shouldn't involve a clock or any clocked elements, like registers.

**Hint 1**: Before you start wiring, you should think very carefully about how you might decompose this problem into smaller ones and join them together. You should feel very free to use subcircuits when implementing `rot4` and `rot8` as well as `rotr`.

**Hint 2**: Just because we gave you an RTL representation doesn't mean it's the best way to look at this problem. Think about the input bits of `B` and think about how to effectively use splitters! Can you do something with the binary form? Remember why binary is good for use in computers: a `1` is easy to represent as an `ON` signal, and a `0` is easy to represent as an `OFF` signal. Let's say we want to rotate `9` times. `9` is `1001` in binary, or `1*8 + 0*4 + 0*2 + 1*1`. Can you use this to make a cleaner circuit? Making use of the rot\* circuits we have provided is a good idea that will keep things clean!

#### Checkoff

*   Show your TA your `rotr` circuit and verify that it works.

### 3\. Runing LED

From practice 2, we obtained a combinational circuits which can rotate the input with different values. In this practice, for each clock, the input will rotate with step of 1 and the rotated value will be input next time. For example, if `input` is `0b10000000` and we set the rotate step of `1`. At clock 2, `output` is `0b01000000`, at clock 3, output is `0b00100000`...

#### Action Item

1\. Open up the Exercise 3 schematic (`File->Open->ex3.circ`) and fill in the `rot1`, you can use the circuits in pratice 2 and revise the width of input and output.

2\. Using the `Register` to store the previous value, and then using a `rot1` to rotate the value, the output is `OUT_LED`.

3\. Using a `Multiplexer` to set the initinal value `INITIAL_LED` of `Register` when reset signal `RST` is 1. When `RST` is 0, the input of Multiplexer will be `OUT_LED`.

#### Checkoff

*   Show your TA your `Runing LED` circuit and verify that it works.
*   How to automatic set the reset signal in this circuits ?
*   Can you use sequential circuits only to implement such circuits ?

Testing
-------

To test, run the testing script via:

```
./test.sh
```

Since Logisim will be running in one terminal window already, make sure to open up a new window to run the testing script.

If it says you don't have permission to test.sh, run the following code:

```
chmod +x test.sh
```

If it says you don't have permission to test.py, run the following code in ./testing file:

```
chmod +x test.py
```