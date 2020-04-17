# Building-RTOS
Building up a RTOS for Texas Instruments' MSP432P4111 Development board

## Development Process:

### 1. RTOS-01-Kernel-Internals
    In this we created two tasks, created two stack and tried to achieve switching manually by changing the value of SP 
    register to stack pointer value of the task to switch to. Since the task stack was initialized accoring to the exception 
    stack frame, when the code leaves ISR, it saves the stack frame data from new loaction. Thus PC register is updated too 
    with the data at which we want to execute the instructions (in this case beginning of a function).
