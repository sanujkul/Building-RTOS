## WHAT WE DID?
### We saw 2 manual ways of achieving context switch at the time of exception handling:   
Exception is caused by SysTick_timer.  

1. Saw the messy way to achieving swithing between 2 tasks.  
   This is done bt directly changing PC register data when 
   our flow is at end of SysTicj_Handler.
   
2. a. Then we created 2 task stack of size 40 (x4 bytes)
   b. Then we initialized it in the main function 
   c. Important initializations were XPSR and PC register
   d. Then we ran our code in debug mode. 
   e. Here at the end of SysTick_Handler, we manipulated Stack pointer to
      now point to top of our newly created task stack.
      ### How we manipulated?
      We changed the data in SP register. 
      More specifically, we changed address in MSP register to now point to top of our new task stack
      ### What this does?
      While loading back data from stack to Processors's General Purpose Registers at the end of
      Expception, we are now loading new set of data to those registers
