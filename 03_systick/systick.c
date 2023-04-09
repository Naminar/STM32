#include <stdint.h>
#include <stdbool.h>
#include "../stmMacro.h" 
//---------------
// RCC Registers
//---------------
/*
#define REG_RCC_CR     (volatile uint32_t*)(uintptr_t)0x40021000U // Clock Control Register
#define REG_RCC_CFGR   (volatile uint32_t*)(uintptr_t)0x40021004U // PLL Configuration Register
#define REG_RCC_AHBENR (volatile uint32_t*)(uintptr_t)0x40021014U // AHB1 Peripheral Clock Enable Register
#define REG_RCC_CFGR2  (volatile uint32_t*)(uintptr_t)0x4002102CU // Clock configuration register 2

//----------------
// GPIO Registers
//----------------

#define GPIOC_MODER (volatile uint32_t*)(uintptr_t)0x48000800U // GPIO port mode register
#define GPIOC_TYPER (volatile uint32_t*)(uintptr_t)0x48000804U // GPIO port output type register
*/#define GPIOC_ODR   (volatile uint32_t*)(uintptr_t)0x48000814U // GPIO port output data register
#define GPIOC_BSRR  (volatile uint32_t*)(uintptr_t)0x48000818U

//-------------------
// SysTick registers
//-------------------

#define SYSTICK_CSR   (volatile uint32_t*)(uintptr_t)0xE000E010U // SysTick Control and Status Register
#define SYSTICK_RVR   (volatile uint32_t*)(uintptr_t)0xE000E014U // SysTick Reload Value Register
#define SYSTICK_CVR   (volatile uint32_t*)(uintptr_t)0xE000E018U // SysTick Current Value Register
#define SYSTICK_CALIB (volatile uint32_t*)(uintptr_t)0xE000E01CU // SysTick Calibration Value Register

//-------------------
// RCC configuration
//-------------------

//#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
//#define ONE_MILLISECOND 48000U

/*void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    *REG_RCC_CR = 0x00010000U;
    while ((*REG_RCC_CR & 0x00020000U) != 0x00020000U);

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    *REG_RCC_CFGR2 |= 1U;

    // (3) Select PREDIV output as PLL input (4 MHz):
    *REG_RCC_CFGR |= 0x00010000U;

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    *REG_RCC_CFGR |= (12U-2U) << 18U;

    // (5) Enable PLL:
    *REG_RCC_CR |= 0x01000000U;
    while ((*REG_RCC_CR & 0x02000000U) != 0x02000000U);

    // (6) Configure AHB frequency to 48 MHz:
    *REG_RCC_CFGR |= 0b000U << 4U;

    // (7) Select PLL as SYSCLK source:
    *REG_RCC_CFGR |= 0b10U;
    while ((*REG_RCC_CFGR & 0xCU) != 0x8U);

    // (8) Set APB frequency to 24 MHz
    *REG_RCC_CFGR |= 0b001U << 8U;
}*/

void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    
    *REG_RCC_CR = HSE_ON_BIT_AND_WAIT_OSCILLATIONS;
    
    while (COMPARE_REG_AND_MASK(REG_RCC_CR, HSE_CLOCK_READY_FLAG))
    
    //===========================================
    // (2) Configure PLL:
    
    MODIFY_REG(REG_RCC_CFGR2, PREDIV_BIT, PREDIV_CLOCK_DIVIDED_TWO)
    
    //===========================================
    // (3) Select PREDIV output as PLL input (4 MHz):
    
    MODIFY_REG(REG_RCC_CFGR, PLL_INPUT_CLOCK_SOURCE_BIT, SELECT_PREDIV_OUTPUT_AS_PLL_INPUT)
    
    //===========================================
    // (4) Set PLLMUL to 12:

    MODIFY_REG(REG_RCC_CFGR, PLLMUL_BIT, SET_PLLMUL_TWELVE)

    //===========================================
    // (5) Enable PLL:

    MODIFY_REG(REG_RCC_CR, PLL_ON_BIT, ENABLE_PLL)

    while (COMPARE_REG_AND_MASK(REG_RCC_CR, PLLRDY_BIT))
    
    //===========================================
    // (6) Configure AHB frequency to 48 MHz:

    MODIFY_REG(REG_RCC_CFGR, HCLK_PRESCALER_BITS, HCLK_PRESCALER_BITS)
    
    //===========================================
    // (7) Select PLL as SYSCLK source:
    
    MODIFY_REG(REG_RCC_CFGR, SYSTEM_CLOCK_SWITCH_BIT, SELECT_PLL_AS_SYSCLK)

    while ((*REG_RCC_CFGR & 0xCU) != 0x8U);

    //===========================================
    // (8) Set APB frequency to 24 MHz:

    MODIFY_REG(REG_RCC_CFGR, PCLK_PRESCALER, SET_APB_FREQUENCY_TWTYFOUR)
}

#define ONE_MS_DELAY_TIME 6000000U
void more_precise_delay_forbidden_by_quantum_mechanics_1000ms()
{
    for (uint32_t i = 0; i < ONE_MS_DELAY_TIME; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}

/*void timing_perfect_delay(uint32_t millis)
{
    
    
    ldr r3, [r7, 4]
    adds r3, #1
    str r3, [r7, #4]
    ldr r3, [r7, #4]
    ldr r2, []
}*/

//--------------------
// GPIO configuration
//--------------------

/*void board_gpio_init()
{
    // (1) Configure PC8 and PC9:
    *REG_RCC_AHBENR |= (1U << 19U);

    // (2) Configure LED modes:
    *GPIOC_MODER |= (0b01U << (2U*8U));
    *GPIOC_MODER |= (0b01U << (2U*9U));

    // (3) Configure LED types:
    *GPIOC_TYPER |= (0U << 8U);
    *GPIOC_TYPER |= (0U << 9U);
}*/

void board_gpio_init()
{
    // (1) Enable GPIOC clocking:

    //*REG_RCC_AHBENR |= 0x80000U; //IOPCEN:
    MODIFY_REG(REG_RCC_AHBENR, GPIOC_CLOCKING_BIT, ENABLE_GPIOC_CLOCKING)

    // (2) Configure PC8 mode:

    //*GPIOC_MODER |= 0b01U << (2*8U);
    MODIFY_REG(GPIOC_MODER, PC8_MODE_BIT, PC8_GENERAL_OUTPUT_MODE)

    // (3) Configure PC8 type:

    //*GPIOC_TYPER |= 0b0U << 8U;
    MODIFY_REG(GPIOC_TYPER, PC8_TYPE_BIT, PC8_OUTPUT_PUSH_PULL_MODE)

    // (4) Configure PC9 mode:

    //*GPIOC_MODER |= 0b01U << (2*9U);
    MODIFY_REG(GPIOC_MODER, PC9_MODE_BIT, PC9_GENERAL_OUTPUT_MODE)

    // (5) Configure PC9 type:

    //*GPIOC_TYPER |= 0b0U << 9U;
    MODIFY_REG(GPIOC_TYPER, PC9_TYPE_BIT, PC9_OUTPUT_PUSH_PULL_MODE)
}

//-----------------------
// SysTick configuration
//-----------------------

void systick_init(uint32_t period_us)
{
    // (0) Read STM32F051 SysTick configuration: (0x00FFFFFFU = RELOAD_BITS)
    uint32_t reload_value = period_us * (*SYSTICK_CALIB & RELOAD_BITS) / 1000U;

    // Ignore the SKEW in SYSTICK_CALIB register :(
    // What if there is no reference clock on the board?

    // (1) Program the reload value:
    //*SYSTICK_RVR = ;
    SET_REG_VALUE(SYSTICK_RVR, (reload_value - 1U) & RELOAD_BITS)

    // (2) Clear the current value:
    //*SYSTICK_CVR = 0U;
    SET_REG_VALUE(SYSTICK_CVR, ZERO)

    // (3) Program the CSR:
    //*SYSTICK_CSR = 0x3U;
    MODIFY_REG(SYSTICK_CSR, ENABLE_AND_TICKINT_BITS, ENABLE_AND_TICK)
}

void systick_handler(void)
{
    static int handler_ticks = 0U;

    handler_ticks += 1U;

    if (handler_ticks == 10000U)
    {
        handler_ticks = 0U;

        //uint32_t reg_gpio_odr = *GPIOC_ODR;
        //*GPIOC_ODR = (reg_gpio_odr & ~0x0100U) | (~reg_gpio_odr & 0x0100U);
        
        // MISTAKE HERE ^^^
        // this laps didn't light on and off
        
        //GPIOC_BSRR = 0x48000818U; // ADDED
        //0x18 - BSRR
        
        MODIFY_REG(GPIOC_BSRR, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE) // ADDED


        //MODIFY_REG(GPIOC_ODR, 0x0100U, 0x0100U)

        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();

        //MODIFY_REG(GPIOC_ODR, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
        MODIFY_REG(GPIOC_ODR, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
    }
}

//------
// Main
//------

int main(void)
{
    board_clocking_init();

    systick_init(100U);

    board_gpio_init();

    while (1)
    {
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();

        //*GPIOC_ODR |= 0x0200U;
        MODIFY_REG(GPIOC_ODR, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)

        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();

        //*GPIOC_ODR &= ~0x0200U;
        MODIFY_REG(GPIOC_ODR, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
    }
}
