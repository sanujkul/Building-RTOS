//1. Enable clock access to the LED or other PINs
//2. Set the Pin Node
//3. Set OUTPUT

/*
Note: 
1. In STM32F446RE, there is 1 USR LED - LD2.
2. This LED is connected to Pin PA5 - GPIOA Pin 5
3. GPIOs are connected to AHB1 bus !!
4. In GPIO Mode register, we need to set it to GPIO Mode
(Each GPIOs have 16 pins)
*/

#include "stm32f446xx.h"

#define GPIOA_BASE_ADDR 0x40020000UL //UL = Unsigned Long = 32 bits

#define GPIO_MODE_REG_OFFSET  0x00UL
#define GPIOA_MODE_REG (GPIOA_BASE_ADDR + GPIO_MODE_REG_OFFSET)
#define MODE_OUT 1
#define P5_MODE_OUT (1U<<10)

#define GPIO_OUTPUT_DATA_REG_OFFSET 0x14UL
#define GPIOA_OUTPUT_DATA_REG (GPIOA_BASE_ADDR + GPIO_OUTPUT_DATA_REG_OFFSET)

#define GREEN_LED (1UL<<5) //As it is the 5th pin and will be used to toggle LED

#define RCC_BASE_ADDR 0x40023800UL
#define AHB1ENR_OFFSET 0x30UL
#define AHB1ENR_ADDR (RCC_BASE_ADDR + AHB1ENR_OFFSET)
#define GPIOA_CLK_ENABLE 0x01UL 

//GLOBAL VARIABLES
volatile uint32_t tick;	//Ticks of clock
volatile uint32_t _tick; //

// FUNCTION DEFINITIONS
void GPIO_Init(void);




int main(){

	//1,2. Initializing the GPIO pins
	GPIO_Init();
	
	//3. BLINKING
	volatile uint32_t *gpioAOutDataR = (uint32_t*) GPIOA_OUTPUT_DATA_REG;
	volatile int i = 0;
	while(1){
		//3.a. ON	
		//GPIOA->ODR |= GREEN_LED;		//Either this way or
		*gpioAOutDataR |= GREEN_LED;
		
		//Delay
		for(i=0; i<1000000000; i++);
	
		//3.b. OFF
		*gpioAOutDataR &= ~GREEN_LED;
	
		//Delay
		for(i=0; i<1000000000; i++);

	}
	
}


void GPIO_Init(void){
	//1. Enabling the CLOCK FOR GPIOA
	//RCC->AHB1ENR |= GPIOA_CLK_ENABLE; //Either this way or - 
	uint32_t *abh1EnrReg = (uint32_t*) AHB1ENR_ADDR;
	*abh1EnrReg |= GPIOA_CLK_ENABLE;
	
	//2. SETTING GPIOA Pin 5 (GREEN LED) as OUTPUT 	
	//GPIOA->MODER |= P5_MODE_OUT;  //Either this way or
	uint32_t *gpioAModeR = (uint32_t*) GPIOA_MODE_REG;
	*gpioAModeR |= P5_MODE_OUT;

	//3. Configuring the System Clock
	SysTick_Config(SystemCoreClock/1000); //To make it in milli second
	__enable_irq();

}

//Thouhg this compiler shows that there is no previous definition of SysTick_Handler, 
//I verified in debug mode that address 0x0000003C where SysTick address has to be saved (Refer vector table)
//has the address for this function!! So its working!
//A weak function defined in startup file
void SysTick_Handler(void){
	++tick;
}

