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
#define P6_MODE_OUT (1U<<12)

#define GPIO_OUTPUT_DATA_REG_OFFSET 0x14UL
#define GPIOA_OUTPUT_DATA_REG (GPIOA_BASE_ADDR + GPIO_OUTPUT_DATA_REG_OFFSET)

#define GREEN_USR_LED (1UL<<5) //As it is the 5th pin and will be used to toggle LED
#define GREEN_EXT_LED (1UL<<6) //Connected to PA6

#define RCC_BASE_ADDR 0x40023800UL
#define AHB1ENR_OFFSET 0x30UL
#define AHB1ENR_ADDR (RCC_BASE_ADDR + AHB1ENR_OFFSET)
#define GPIOA_CLK_ENABLE 0x01UL 

//GLOBAL VARIABLES
volatile uint32_t tick;	//Ticks of clock
volatile uint32_t _tick; //A variable that is used for work on tick
	//Stack
uint32_t greenUsrStack[40];
uint32_t greenExtStack[40];

uint32_t *sp_green_usr = &greenUsrStack[40];		//Stack for task related to Green USR LED
uint32_t *sp_green_ext = &greenExtStack[40];		//Stack for task related to Green EXT LED

// FUNCTION DEFINITIONS
void GPIO_Init(void);
uint32_t getTick(void);
void Delays(uint32_t seconds);
void greenUsrOn(void);
void greenUsrOff(void);
void greenExtOn(void);
void greenExtOff(void);
int greenUsrMain(void);
int greenExtMain(void);
	
/////////////////////////////////////////////////////////////////////////////////
/// MAIN CODE
/////////////////////////////////////////////////////////////////////////////////
int main(){

	//1,2. Initializing the GPIO pins
	GPIO_Init();
	
	//We will initialize the stack in the order of exception STACK
	*(--sp_green_usr) = (1U << 24); 	/*XPSR REG - We set 24th bit 1 to tell uP that to run in thumb mode*/
	*(--sp_green_usr) = (uint32_t)&greenUsrMain;/*PC Register - Pointing to the Task Associated to it*/
	*(--sp_green_usr) = 0x0000000FU;  
	*(--sp_green_usr) = 0x0000000AU; 
	*(--sp_green_usr) = 0x0000000CU;
	*(--sp_green_usr) = 0x0000000CU;
	*(--sp_green_usr) = 0x00000001U;
	*(--sp_green_usr) = 0x00000003U;
	
	*(--sp_green_ext) = (1U << 24); 	/*XPSR REG - We set 24th bit 1 to tell uP that to run in thumb mode*/
	*(--sp_green_ext) = (uint32_t)&greenExtMain;/*PC Register - Pointing to the Task Associated to it*/
	*(--sp_green_ext) = 0x0000000BU;  
	*(--sp_green_ext) = 0x0000000AU; 
	*(--sp_green_ext) = 0x0000000BU;
	*(--sp_green_ext) = 0x0000000AU;
	*(--sp_green_ext) = 0x00000005U;
	*(--sp_green_ext) = 0x00000001U;
	
	while(1){
		//INFINITTE LOOP
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////
/// TWO MAIN FUNCTIONS THAT WILL BE LIKE TASKS
///////////////////////////////////////////////////////////////////////////////////////
int greenUsrMain(void){
	while(1){
		greenUsrOn();
		Delays(50);
		greenUsrOff();
		Delays(50);
	}
}

int greenExtMain(void){
	while(1){
		greenExtOn();
		Delays(50);
		greenExtOff();
		Delays(50);
	}
	
}


//Thouhg this compiler shows that there is no previous definition of SysTick_Handler, 
//I verified in debug mode that address 0x0000003C where SysTick address has to be saved (Refer vector table)
//has the address for this function!! So its working!
//A weak function defined in startup file
void SysTick_Handler(void){
	++tick;
}

////////////////////////////////////////////////////////////////////////////////////////
// Functions made for Delay
///////////////////////////////////////////////////////////////////////////////////////
uint32_t getTick(void){
	__disable_irq();
	_tick = tick;
	__enable_irq();
	
	return _tick;
}

void Delays(uint32_t milliSeconds){
	milliSeconds *= 16;						//Converting to number of ticks needed to achieve this delay
	uint32_t temp = getTick();
	while((getTick() - temp < milliSeconds)){}
}


///////////////////////////////////////////////////////////////
//The LEDs ON OFF FUNCTIONS, and initializing function
///////////////////////////////////////////////////////////////
//Initializing the GPIOS
void GPIO_Init(void){
	//1. Enabling the CLOCK FOR GPIOA
	//RCC->AHB1ENR |= GPIOA_CLK_ENABLE; //Either this way or - 
	uint32_t *abh1EnrReg = (uint32_t*) AHB1ENR_ADDR;
	*abh1EnrReg |= GPIOA_CLK_ENABLE;
	
	//2. SETTING GPIOA Pin 5 (GREEN LED) as OUTPUT 	
	//GPIOA->MODER |= P5_MODE_OUT;  //Either this way or
	uint32_t *gpioAModeR = (uint32_t*) GPIOA_MODE_REG;
	*gpioAModeR |= (P5_MODE_OUT | P6_MODE_OUT);

	//3. Configuring the System Clock
	SysTick_Config(SystemCoreClock/1000); //To make it in milli second
	__enable_irq();

}
void greenUsrOn(void){
	//3. BLINKING
	//volatile uint32_t *gpioAOutDataR = (uint32_t*) GPIOA_OUTPUT_DATA_REG;
	//*gpioAOutDataR ^= (GREEN_USR_LED); //Either this way or
	GPIOA->ODR |= GREEN_USR_LED;		
}
void greenUsrOff(void){
	//3. BLINKING
	//volatile uint32_t *gpioAOutDataR = (uint32_t*) GPIOA_OUTPUT_DATA_REG;
	//*gpioAOutDataR ^= (GREEN_USR_LED); //Either this way or
	GPIOA->ODR &= ~GREEN_USR_LED;		
}

void greenExtOn(void){
	GPIOA->ODR |= GREEN_EXT_LED;		
}
void greenExtOff(void){
	GPIOA->ODR &= ~GREEN_EXT_LED;		
}



