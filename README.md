# Building-RTOS
Building up a RTOS for Texas Instruments' MSP432P4111 Development board. We are using TI's Code Composer Studio for programming

## Development Process:

### 1. RTOS-01-Kernel-Internals
The main puspose of this project was to understand how RTOS kernel works by manually making processor to achieving switching between two tasks by changing value of SP register and PC register.

   In this we created two tasks, created two stack and tried to achieve switching manually by changing the value of SP 
   register to stack pointer value of the task to switch to. Since the task stack was initialized accoring to the exception 
   stack frame, when the code leaves ISR, it saves the stack frame data from new loaction. Thus PC register is updated too 
   with the data at which we want to execute the instructions (in this case beginning of a function).
   
   P.S. Manually changing SP register in Code Composer Studio wont change MSP register data and context switching will not be achieved. Therefore MSP register data has to be changed.  
   (A good link to read about exception programming: https://www.sciencedirect.com/topics/engineering/main-stack-pointer)
