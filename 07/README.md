Lab 7
-----

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/) [ShanghaiTech University](http://www.shanghaitech.edu.cn/)  

PipeLine
--------

Setup
-----

*   Download source code from [here](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab7/lab07.tar)

### Shift and Add Multiplier

Here we review a simplest implementation of Multiplier: Shift and Add Multiplier. The block diagram is shown below:

![](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab7/picture/block_diagram.png)

In this practice, we assume the multiplier and multiplicand are 4-bit unsigned integers, and the output is an 8-bit unsigned integer. Hence the the whole multiplier can be flattened to a combined logic.

### Practice

#### 1\. non\_pipelined 4-bit multiplier

We're going to finish a 4-bit multiplier.

#### Action Item

Complete the following steps to create the splitter circuit, and show this to your TA (remember to save). When you've completed the circuit, answer the question in the checkoff session.

1.  Open up the Exercise 1 schematic (`File->Open->ex1.circ`) and go to the `move_and_add` circuit.
2.  Connect all Gates, Plexers, and Arithmetic Blocks to implement a single shift and add step. Once you finished, the circuit should correctly decide whether to add or not to add the input multiplicand on the current result, and output the shifted multiplicand as well.
    
3.  Now refer to the `non-pipelined` circuit. Play with the inputs to see if your implementation is correct, and adjust your design if necessary.
    
4.  Let the propagation delay of an adder block be 45ns, the propagation delay of MUX be 20ns, and the propagation delay of a shifters block (since we have a const offset, it is very efficient) be 5ns. The register has a CLK-to-Q delay of 10ns, setup time of 10ns, and hold time of 5ns. Calculate the maximum clock rate at which this circuit can operate. Assume that both inputs come from clocked registers that receive their data from an outside source.
    

#### Checkoff

*   Show your TA the calculations you performed to find the maximum clock rate (non-pipelined).

### 2\. Pipe that line

Recalling common operations in ALU, the shift addition multiplier we implemented is significantly inefficient compared to other operations, which reduces the latency performance of ALU. Try splitting the implementation of the shift addition multiplier into two stages to maximize the maximum clock cycle.

In order to check that your pipelining still produces correct outputs, we will consider the outputs from the circuit "correct" if and only if it corresponds to the sequence of outputs the non-pipelined version would emit, bar some leading zeroes. For example, if for some sequence of inputs, the non-pipelined version emits the sequence \[3, 5, 1, 2, 4, …\]. Then, the correct pipelined circuit might emit the sequence \[0, 3, 5, 1, 2, 4, …\] for the same sequence of inputs. You can check this by simulating the circuit (using the "Simulate" menu dropdown) and either ticking the clock manually or enabling continuous ticks.

Note that in order to pipeline the circuit, we need a register to hold the intermediate value of the computation between pipeline stages. This is a general theme with pipelines.

#### Action Item

*   Complete the sub-circuit pipelined. You will need to add a register to divide the multiplication and addition stages up.
*   Calculate the maximum clock rate for the pipelined version of the circuit that you have created.

#### Checkoff

*   Show your TA the completed, pipelined circuit.
*   Show your TA the calculations you performed to find the maximum clock rate (pipelined).

#### Futher thinking (Optional)

*   Is there any better implementation for Adder and Multiplier?
*   The pipeline multiplier we implemented requires more than one clock cycle to complete calculations. Do you know any instructions having similar situations in modern CPUs? How are modern CPUs compatible with this situation?