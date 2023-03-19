#include <stdint.h>

//---------------
// RCC Registers
//---------------

#define REG_RCC_CR     (volatile uint32_t*)(uintptr_t)0x40021000U // Clock Control Register
#define REG_RCC_CFGR   (volatile uint32_t*)(uintptr_t)0x40021004U // PLL Configuration Register
#define REG_RCC_AHBENR (volatile uint32_t*)(uintptr_t)0x40021014U // AHB1 Peripheral Clock Enable Register
#define REG_RCC_CFGR2  (volatile uint32_t*)(uintptr_t)0x4002102CU // Clock configuration register 2

//----------------
// GPIO Registers
//----------------

#define GPIOC_OUT   (volatile uint32_t*)(uintptr_t)0x48000814U // GPIO port output data register
#define GPIOC_MODER (volatile uint32_t*)(uintptr_t)0x48000800U // GPIO port mode register
#define GPIOC_TYPER (volatile uint32_t*)(uintptr_t)0x48000804U // GPIO port output type register

// All unmarked bits of the MODIFYMASK REG register will remain unchanged

#define MODIFY_REG(REG, MODIFYMASK, VALUE)  ( *REG) = ( ( *REG) & ~ ( MODIFYMASK)) | ( ( VALUE) & ( MODIFYMASK) );

#define COMPARE_REG_AND_MASK(REG, COMPARISON_MASK) ( ( *REG) & ( COMPARISON_MASK)) ^ ( COMPARISON_MASK)

#define SET_GREEN_DIODE_BIT  0x100U
#define SET_BLUE_DIODE_BIT   0x200U

#define LIGHT_GREEN_DIODE  0x100U
#define LIGHT_BLUE_DIODE   0x200U
#define TURN_OFF_DIODE     0u

//------
// Main
//------

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define ONE_MILLISECOND CPU_FREQENCY/1000U

#define HSE_ON_BIT_AND_WAIT_OSCILLATIONS 0x00010000U
#define HSE_CLOCK_READY_FLAG 0x00020000U

#define PREDIV_BIT 1U
#define PREDIV_CLOCK_DIVIDED_TWO 1U

#define PLL_INPUT_CLOCK_SOURCE_BIT 0x00010000U
#define SELECT_PREDIV_OUTPUT_AS_PLL_INPUT 0x00010000U

#define PLLMUL_BIT (12U-2U) << 18U
#define SET_PLLMUL_TWELVE (12U-2U) << 18U // frequency = 48 MHz

#define PLL_ON_BIT 0x01000000U 
#define ENABLE_PLL 0x01000000U

#define PLLRDY_BIT 0x02000000U
#define HCLK_PRESCALER_BITS 0b000U << 4U // prescaler

#define SYSTEM_CLOCK_SWITCH_BIT 0b11U // System clock switch
#define SELECT_PLL_AS_SYSCLK 0b10U

#define PCLK_PRESCALER 0b001U << 8U
#define SET_APB_FREQUENCY_TWTYFOUR 0b001U << 8U // frequency

void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    
    //*REG_RCC_CR = 0x00010000U; // HSEON hse clock enable
    //while ((*REG_RCC_CR & 0x00020000U) != 0x00020000U); // HSE clock ready flag

    *REG_RCC_CR = HSE_ON_BIT_AND_WAIT_OSCILLATIONS;
    
    while (COMPARE_REG_AND_MASK(REG_RCC_CR, HSE_CLOCK_READY_FLAG))
    
    //===========================================
    // (2) Configure PLL:
    
    // PREDIV output: HSE/2 = 4 MHz
    //*REG_RCC_CFGR2 |= 1U; //: PREDIV input clock divided by 2

    MODIFY_REG(REG_RCC_CFGR2, PREDIV_BIT, PREDIV_CLOCK_DIVIDED_TWO)
    
    //===========================================
    // (3) Select PREDIV output as PLL input (4 MHz):
    
    //*REG_RCC_CFGR |= 0x00010000U; // PLLSRC - PLL input clock source

    MODIFY_REG(REG_RCC_CFGR, PLL_INPUT_CLOCK_SOURCE_BIT, SELECT_PREDIV_OUTPUT_AS_PLL_INPUT)
    
    //===========================================
    // (4) Set PLLMUL to 12:

    // SYSCLK frequency = 48 MHz
    //*REG_RCC_CFGR |= (12U-2U) << 18U;

    MODIFY_REG(REG_RCC_CFGR, PLLMUL_BIT, SET_PLLMUL_TWELVE)

    //===========================================
    // (5) Enable PLL:

    //*REG_RCC_CR |= 0x01000000U;
    //while ((*REG_RCC_CR & 0x02000000U) != 0x02000000U); //PLLRDY

    MODIFY_REG(REG_RCC_CR, PLL_ON_BIT, ENABLE_PLL)

    while (COMPARE_REG_AND_MASK(REG_RCC_CR, PLLRDY_BIT))
    
    //===========================================
    // (6) Configure AHB frequency to 48 MHz:

    //*REG_RCC_CFGR |= 0b000U << 4U; // HPRE[3:0]: HCLK prescaler // Set and cleared by software to control the division factor of the AHB clock.

    MODIFY_REG(REG_RCC_CFGR, HCLK_PRESCALER_BITS, HCLK_PRESCALER_BITS)
    
    //===========================================

    // (7) Select PLL as SYSCLK source:
    //*REG_RCC_CFGR |= 0b10U; //SW[1:0]: System clock switch
    
    MODIFY_REG(REG_RCC_CFGR, SYSTEM_CLOCK_SWITCH_BIT, SELECT_PLL_AS_SYSCLK)

    while ((*REG_RCC_CFGR & 0xCU) != 0x8U);

    //===========================================

    // (8) Set APB frequency to 24 MHz:
    //*REG_RCC_CFGR |= 0b001U << 8U; //PPRE[2:0]: PCLK prescaler //Set and cleared by software to control the division factor of the APB clock (PCLK).

    MODIFY_REG(REG_RCC_CFGR, PCLK_PRESCALER, SET_APB_FREQUENCY_TWTYFOUR)
}

#define PC8_MODE_BIT 0b11U << (2*8U)
#define PC9_MODE_BIT 0b11U << (2*9U)

//General purpose output mode macro

#define PC8_GENERAL_OUTPUT_MODE 0b01U << (2*8U)
#define PC9_GENERAL_OUTPUT_MODE 0b01U << (2*9U)

#define PC8_TYPE_BIT 0b1U << 8U
#define PC9_TYPE_BIT 0b1U << 9U

#define PC8_OUTPUT_PUSH_PULL_MODE 0b0U << 8U
#define PC9_OUTPUT_PUSH_PULL_MODE 0b0U << 9U

#define GPIOC_CLOCKING_BIT    0x80000U
#define ENABLE_GPIOC_CLOCKING 0x80000U

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

void totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms()
{
    for (uint32_t i = 0; i < 1000U * ONE_MILLISECOND; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}


int main()
{
    #ifndef INSIDE_QEMU
        board_clocking_init();
    #endif

    board_gpio_init();

    while (1)
    {
        MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE)

        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
        
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)

        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();
        
        MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
        
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();
    }
}
