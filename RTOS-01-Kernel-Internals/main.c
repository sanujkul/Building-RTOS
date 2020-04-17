/*
 * Kernel Internals
 */


#include "msp.h"
////////////////////////////////////
#define DIR_OUT_P1_0 (uint8_t)0x01   //OR WITH THIS
#define OUT_HIGH_P1_0 (uint8_t)0x01  //OR WITH THIS MASK
#define OUT_LOW_P1_0 (uint8_t)0xFE   //AND with this MASK

#define RGBLED_OUTPUT_DIR (uint8_t)0x07 //OR WITH THIS 0x00000111 wull set All leds of RGB as output mode
#define OUT_HIGH_P2_2 (uint8_t)0x04      //0x00000100, OR with this will set P2.2 HIGH
#define OUT_LOW_P2_2 ~OUT_HIGH_P2_2  //AND with this MASK will set P2.2 LOW
////////////////////////////////////
#define RGB_LOOP_TIME 180000
////////////////////////////////////
#define STACK_SIZE 40

//RTOS related variables:
volatile uint32_t tick; //In the systick handler we will increase its count
volatile uint32_t _tick;

//Creating Tasks Stack
uint32_t blue_stack[STACK_SIZE];                //Reserving stack memory for the blue task
uint32_t red_stack[STACK_SIZE];                 //Reserving stack memory for the red task

uint32_t* sp_blue = &blue_stack[STACK_SIZE]; //Creating two stack pointers for blue and red main tasks
uint32_t* sp_red  = &red_stack[STACK_SIZE];  //STACK_SIZE is one more than last address
////////////////////////////////////
//Functions:
void setupUSER_LEDS(void);
void delays(uint32_t seconds);

void blueOn(void);
void blueOff(void);
void redOn(void);
void redOff(void);

int blue_main_Task(void);
int red_main_Task(void);

/**
 * main.c
 * 1. Stop  WDT
 * 2. Setup GPIOs
 * 3. Systick Timer setup (= 30KHz)
 * 4. Enabling Interrupt
 */
void main(void)
{
    uint32_t volatile start = 0U; //Initialized to 0 unsigned integer
    //1. Stopping watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //2. Setting up GPIOs
    setupUSER_LEDS();

    //3. SystickTimer
//  uint32_t system_clk = SystemCoreClock; //By default SystemCoreClock = 3 MHz;
    SysTick_Config(SystemCoreClock/100);

    //4.Enable global interrupt so that Systick handler works
    __enable_irq();

    //Initializing blue_main Task stack
    *(--sp_blue) = (1U << 24);                        // XPSR                 register of stack frame
    *(--sp_blue) = (uint32_t)&blue_main_Task;       // PC                   register of stack frame
    *(--sp_blue) = 0x0000000A;                       // LR (Link register)   register of stack frame
    *(--sp_blue) = 0x0000000B;                       // R12                  register of stack frame
    *(--sp_blue) = 0x0000000C;                       // R3                   register of stack frame
    *(--sp_blue) = 0x0000000D;                       // R2                   register of stack frame
    *(--sp_blue) = 0x0000000E;                       // R1                   register of stack frame
    *(--sp_blue) = 0x0000000F;                       // R0                   register of stack frame

    //Initializing red_main Task stack
    *(--sp_red) = (1U << 24);                       // XPSR                 register of stack frame
    *(--sp_red) = (uint32_t)&red_main_Task;        // PC                   register of stack frame
    *(--sp_red) = 0x0000000F;                       // LR (Link register)   register of stack frame
    *(--sp_red) = 0x0000000A;                       // R12                  register of stack frame
    *(--sp_red) = 0x0000000C;                       // R3                   register of stack frame
    *(--sp_red) = 0x0000000E;                       // R2                   register of stack frame
    *(--sp_red) = 0x0000000A;                       // R1                   register of stack frame
    *(--sp_red) = 0x0000000B;                       // R0                   register of stack frame

    while(1){

    }

}

////////////////////////////////////////////////////////////
/////////////// TASKS
////////////////////////////////////////////////////////////

/*
 * This function  is like a task
 * It blinks blue LED
 */

int blue_main_Task(void){
    while(1){
        blueOn();
        delays(1);
        blueOff();
        delays(1);
    }
}

/*
 * This function  is like a task
 * It blinks RED LED
 */

int red_main_Task(void){
    while(1){
        redOn();
        delays(1);
        redOff();
        delays(1);
    }
}
////////////////////////////////////////////////////////////
/////////////// RTOS Helper functions and IRS
////////////////////////////////////////////////////////////

/*
 * This is standard weak function in ARM CORTEX MCUs
 */

void SysTick_Handler(void){
    ++tick;
}

/*
 * Below function is similar to STM32 HAL delay
 * that is use to get the tick difference.
 * _tick = tick; is a critical section.
 * therefore we make it mutually exclusive
 * by disabling all all interrupts
 */
uint32_t getTick(void){
    __disable_irq();
    _tick = tick;
    __enable_irq();

    return tick;
}
/*
 * This delay function causes some waste of time.
 * .\i.e. CPU is being used without doing anything.
 * We can use this to get a delay
 */
void delays(uint32_t seconds){
    seconds *= 100;
    uint32_t temp = getTick();

    while((getTick() - temp) < seconds){
        //waste time
    }

}

////////////////////////////////////////////////////////////
/////////////// NORMAL GPIO Functions
////////////////////////////////////////////////////////////


void setupUSER_LEDS(void){
    //1. FOR RED USER LED:
    P1->DIR = P1->DIR | DIR_OUT_P1_0;
    P1->OUT = OUT_HIGH_P1_0; //ONLY P1.1 HIGH, OTHER RESET to 0

    //2. RGB LED (ONLY BLUE LED control)
    P2->DIR = P2->DIR | RGBLED_OUTPUT_DIR;
    P2->OUT = OUT_HIGH_P2_2; // ONLY BLUE HIGH,  OTHER RESET to 0

}

void blueOn(void){
    P2->OUT |= OUT_HIGH_P2_2;
}

void blueOff(void){
    P2->OUT &= OUT_LOW_P2_2;
}


void redOn(void){
    P1->OUT |= OUT_HIGH_P1_0;
}

void redOff(void){
    P1->OUT &= OUT_LOW_P1_0;
}
