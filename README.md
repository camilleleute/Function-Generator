# P1 - Function Generator

## Reference Materials
- STM32L4xxxx Reference Manual (RM) – General Purpose Timers, SPI, GPIO
- Keypad Datasheet
- Microchip MCP4921 Datasheet

## Introduction
The purpose of this project is to design a function generator using a microcontroller. The microcontroller should be connected with an external DAC to generate the analog waveforms required. This DAC will have an SPI interface. The waveforms that the function generator must generate include a saw tooth waveform, a square wave with a variable duty cycle, and a sinusoidal waveform. The frequency of the waveforms will also be variable. The keypad will be used to select the output waveform type, set the frequency of the waveform and set the duty cycle of the square wave. 

## Objectives
- Understand how to use timers and interrupts to generate timing events
- Understand how to use a serial peripheral interface (SPI)
- Understand how to use digital-to-analog converters (DAC) to generate analog signals from a microcontroller

## Creating Waveforms
Waveforms can be created by the DAC by changing the output voltage at consistent time intervals. This would be like plotting points on a graph. By plotting points at equal intervals, the resulting graph will look uniform and be easy to control. Instead of changing the frequency of the graph by changing the interval between points on the graph, the frequency can also be adjusted by changing how much each point is increased or decreased. Look at Graph P2.1 and P2.2 below. Graph P2.1 is a 100 Hz ramp function. Graph P2.2 is a 200 Hz ramp function. Notice both graphs adjust values every 5 ms. The 100 Hz ramp function increases voltage by 0.3 V for each point while the 200 Hz ramp increases by 0.6 V. This means that lower frequency waveforms will look better than higher frequency ones. This is preferable to having all of the waveforms look equally poor as the highest frequency ones.

## Timing Calculations
The maximum resolution possible with the DAC is based on the time to output a single value to the DAC. You could find this by setting a bit high before calling your previously written DAC_write() function to set the DAC and then setting the bit low after the function call. This would define the maximum resolution if driving the DAC repeatedly without any timing. While this would be fast, it would make precise timing control difficult if not impossible.
To precisely control the timing of output waves, the DAC will need to be set at controlled intervals using a timer. This will add some overhead and reduce the maximum resolution. The time to run the timer ISR can be found in a similar manner with toggling a bit. Create an empty TIM2 ISR that only clears the interrupt flag and calls the DAC_write() function. At the beginning of the ISR set a bit high and then set it low at the end. Measuring this time on an oscilloscope will give you the minimal time for plotting a point of the waveform. This will not take into account the time to get into and out of an ISR. However, from the previous assignment using the timer you have already found this overhead timing. In the previous assignment you counted the number of MCO clock pulses to execute the ISR but found that the minimal CCR1 value you could use did not match. The difference between those two values was the overhead of getting into and out of the ISR. If you add the time for those clock pulses to the pulse time measured above, you will have the minimum time or maximum resolution possible. 

## System Requirements
- Document the maximum resolution (samples per second) using the timing measurements and accompanying calculations. (Add to the Software Architecture section of the report.)
- The function generator shall use a microcontroller with an external DAC, and the DAC will be implemented as a separate library (Header / Source files as shown in Technical Note 3)
- The function generator will be capable of producing
  - A square wave with a variable duty cycle
  - A sinusoidal waveform
  - A sawtooth (ramp) waveform
  - A triangle waveform
- All waveforms will have a Vpp of 3.0 V and be DC-biased at 1.5 V.
- All waveforms will have adjustable frequencies:
  - 100 Hz, 200 Hz, 300 Hz, 400 Hz, 500 Hz
  - The frequency of the waveforms should be within 2.5 Hz of these frequencies for full-credit.
  - The function generator shall have an output rate (points / sec) of at least 60% of the maximum determined from (1) above.
    - If the minimum time from (1) was 50 us, the maximum resolution is 20,000 samples per second. The requirement is to be within 60% of that resolution, or 12,000 samples per second. This is equivalent to 83.33 us between samples.
  - The output rate or samples per second will not change with waveform frequency. The time between outputs from the DAC (time between samples or points on the graph) should be the same for all frequencies. This means the 100 Hz waveform will output 5x the number of points in one period as the 500 Hz waveform. 
- Upon power-up, the function generator will display a 100 Hz square wave with 50% duty cycle.  
- The keypad buttons 1-5 will set the waveform frequency in 100 Hz increments (1 for 100 Hz, 2 for 200 Hz, etc).
- The keypad buttons 6, 7, 8, and 9 will change the output waveform.
  - The 6 key will change the output to a sine waveform
  - The 7 key will change the output to a triangle waveform
  - The 8 key will change the output to a sawtooth waveform
  - The 9 key will change the output to a square waveform
- The keypad buttons ✱, 0, and # will change the duty cycle of the square wave. 
  - The ✱ key will decrease the duty cycle by 10% down to a minimum of 10%.
  - The # key will increase the duty cycle by 10% up to a maximum of 90%
  - The 0 key will reset the duty cycle to 50%
  - The keys ✱, 0, and # will not affect the sine, sawtooth, or triangle waveforms.
- You may not use software delays (ex: delay_us) to generate timing events or control when voltages are changed by the DAC. (It would actually be more difficult to use software delays.) You may use software delays to help debounce buttons (if necessary).

## Suggested Approach
Successful completion of this project requires you to assimilate a variety of information from different sources and implement a system solution. Although this process may seem daunting at the start, a methodical step-by-step approach can be helpful. Here are suggested steps for your project implementation.
- Give all datasheets and reference manuals a read to verify your understanding of both devices/components and the reference material available to you. There is no substitute for reading the datasheets to get started. Aside from understanding key aspects of the device required for successful interfacing and operation, reading the datasheets will potentially illuminate extended or extra features of the device which may be advantageous to the designer. While I was able to cover the highlights of each component in class, we did not have sufficient time to cover all of the details. Most questions can be answered by reading the documentation. 
- Utilize accurate schematic diagrams to guide your work. The final schematic for this project should include the STM32L476, the DAC, the keypad, and power connections. Common schematic practices include:
  - Labeling each IC, component, etc. with a reference designator, e.g. U1, R1, C21, etc. For ICs, this label can be either inside the IC or directly above or below the IC.
  - Label each chip/component with the value/part number or full model number for the component, e.g. 10K, .01uf, STM32L476RG, etc.
  - Numbering all pins above the trace outside of the IC. Refer to the IC diagram in the CubeIDE for their number.
  - Labeling the IC pin name, e.g. P1.0, CS, VCC, inside the chip where the wire attaches to the chip.
  - Labeling all wires with a signal name, e.g. A21, D7, CLK, etc.
- The process of drawing an initial schematic does not have to be long or involved. It can be a quick effort using pencil and paper in which key information such as pin names and numbers are given to aid in wiring and troubleshooting. A formal schematic can be constructed at a later time once the design is finalized. (Refer to Technical Note 3) 
- Do not skip designing your program before coding! Sketch out what the software will do. Aids such as flow charts, state diagrams, pseudo-code, etc. may be used to do this. 
- After you have your complete program logic designed, you can start the actual code writing. You can start by creating #define constants that will be useful. (For example #define DEFAULT_PIN, CPU_FREQ, LOCKED) 
  - Write your program in modules with separate functions that perform repeated tasks. Many of these were probably done durig the previous assignments, but you may find that a function could be extended with helpful features or a new function could be useful. Verify each function works individually before using it in your project. For example, a keypad functionality can be verified by writing values to a set of LEDs. 
  - After getting all of the individual functions working fully, build the project application as you designed in your flowcharts. If you run into an issue with the logic of your original design, go back and redesign your program with a new flowchart. Do not try to change aspects of your program without considering how it functions as a whole. This is hard to do when only looking at the code!
  - Comment and document your code as you write it. Comments are doing your future self a favor now. 
- Don’t forget to make plenty of backups along the way to preserve the good work that has been accomplished. Making frequent backups will allow the developer to roll back to a known development state when something goes wrong or breaks. Although there are various comprehensive version control products available for this purpose (git, svn), a simple approach such as maintaining version directories of a development project on Dropbox, Google Drive, or OneDrive may be satisfactory for projects such as this.

## Hints
- Speed up the MCU clock to improve performance and resolution.
- A while loop can be used to continuously read the keypad until a key is detected. (Pro tip: a while loop can also be used to detect when a button has been released)
- Measure the time needed to set a single point with a Timer ISR as described and calculate the maximum resolution. 
- Determine how many points you want to write to the DAC per period of each of the waveforms. (Pro Tip: Pick a value that meets spec and is divisible by 5, 4, and 3 if possible)
- Develop a plan for creating the sine wave function  
  - It is tempting to use the C programming language’s sin() function, however this function uses floating point numbers and should not be used while “plotting points” or creating the waveforms with the DAC. 
  - One common microcontroller “trick” is to use pre-computed values stored in a look-up table or array so that complex computations do not have to be done by the microcontroller. These tables can also be computed once when the device is initialized or the program starts. Please ensure that any use of floating point math, e.g. calculation with a sine function, does not degrade system performance or slow the resulting device in a noticeable way.
- The method used for creating a sine wave can be used to create other the waveforms. While some waveforms may be easier to implement, creating a new method for each waveform separately is typically more work than adapting the same method multiple times.
- The output from the DAC may not be ideal due to non-linearity errors. It may take some extra time to fine tune your function generator to make it meet specifications fully.


