
#ifndef STM_MACRO_
#define STM_MACRO_

/* NEWS AND NOTES:::::::

    HAVE A TRANSFORMATION IN DEFINE LIKE 

    PC8 -> PIN8 
    PC9 -> PIN9
    
    CAN HAVE SOME PROBLEMS WITH OTHER LABS
*/
//====================LAB 1===========================

// All unmarked bits of the MODIFYMASK REG register will remain unchanged

#define MODIFY_REG(REG, MODIFYMASK, VALUE)  ( *(REG)) = ( ( *(REG)) & ~ ( MODIFYMASK)) | ( ( VALUE) & ( MODIFYMASK) );

#define COMPARE_REG_AND_MASK(REG, COMPARISON_MASK) ( ( *(REG)) & ( COMPARISON_MASK)) ^ ( COMPARISON_MASK)

#define READ_REG(REG, MASK) ( ( *(REG)) & ( MASK) )

//===============================================

#define SET_GREEN_DIODE_BIT  ( 0x200U)
#define SET_BLUE_DIODE_BIT   ( 0x100U)

#define LIGHT_GREEN_DIODE  ( 0x200U)
#define LIGHT_BLUE_DIODE   ( 0x100U)
#define TURN_OFF_DIODE     ( 0U)

//===============================================

#define CPU_FREQENCY ( 48000000U) // CPU frequency: 48 MHz
#define ONE_MILLISECOND ( CPU_FREQENCY / 1000U)

#define HSE_ON_BIT_AND_WAIT_OSCILLATIONS ( 0x00010000U)
#define HSE_CLOCK_READY_FLAG ( 0x00020000U)

#define PREDIV_BIT ( 1U)
#define PREDIV_CLOCK_DIVIDED_TWO ( 1U)

#define PLL_INPUT_CLOCK_SOURCE_BIT ( 0x00010000U)
#define SELECT_PREDIV_OUTPUT_AS_PLL_INPUT ( 0x00010000U)

#define PLLMUL_BIT ( ( 12U-2U) << 18U)
#define SET_PLLMUL_TWELVE ( ( 12U-2U) << 18U) // frequency = 48 MHz

#define PLL_ON_BIT ( 0x01000000U)
#define ENABLE_PLL ( 0x01000000U)

#define PLLRDY_BIT ( 0x02000000U)
#define HCLK_PRESCALER_BITS ( 0b000U << 4U) // prescaler

#define SYSTEM_CLOCK_SWITCH_BIT ( 0b11U) // System clock switch
#define SELECT_PLL_AS_SYSCLK ( 0b10U)

#define PCLK_PRESCALER ( 0b001U << 8U)
#define SET_APB_FREQUENCY_TWTYFOUR ( 0b001U << 8U) // frequency

//===============================================

#define PC8_MODE_BIT ( 0b11U << (2*8U))
#define PC9_MODE_BIT ( 0b11U << (2*9U))

//----------------------------------
// General purpose output mode macro
//----------------------------------

#define PC8_GENERAL_OUTPUT_MODE ( 0b01U << (2*8U))
#define PC9_GENERAL_OUTPUT_MODE ( 0b01U << (2*9U))

#define PC8_TYPE_BIT ( 0b1U << 8U)
#define PC9_TYPE_BIT ( 0b1U << 9U)

#define PC8_OUTPUT_PUSH_PULL_MODE ( 0b0U << 8U)
#define PC9_OUTPUT_PUSH_PULL_MODE ( 0b0U << 9U)

#define GPIOC_CLOCKING_BIT    ( 0x80000U)
#define ENABLE_GPIOC_CLOCKING ( 0x80000U)

//====================LAB 2===========================

#define PIN4_5_MODE_BITS ( 0b1111U << 8U)
#define PIN4_5_INPUT_MODE ( 0b0000U << 8U)
#define PIN4_5_TYPE_BIT ( 0b11U << 4U)
#define PIN4_5_OUTPUT_PUSH_PULL_MODE ( 0b00U << 4U)
#define PIN4_5_PULL_UP_DOWN_BITS ( 0b1111U << (2U*4U))
#define PIN4_5_PULL_DOWN_MODE ( 0b1010U << (2U*4U))

//===============================================

#define PIN0_TYPE_BIT ( 1U)
#define PIN0_OUTPUT_PUSH_PULL_MODE ( 0U)

//---------------------------------------
// CONFIGURE THE IO PULL UP OR  PULL DOWN
//---------------------------------------

#define PIN0_PULL_UP_DOWN_BITS ( 0b11U << (2U*0U))
#define PIN0_PULL_DOWN_MODE ( 0b10U << (2U*0U))

//--------------------
// INPUT BITS
//--------------------

#define PIN0_INPUT_BIT ( 1U << 0U)
#define PIN4_INPUT_BIT ( 1U << 4U)
#define PIN5_INPUT_BIT ( 1U << 5U)

//--------------------
// GPIO configuration
//--------------------

#define PIN0_to_12_MODE_BITS ( 0x3FFFFFFU)
#define PIN0_INPUT_MODE ( 0x00U)
#define PIN1_to_12_GENERAL_OUTPUT_MODE ( 0x1555554U)

//---------------
// RCC Registers
//---------------

#define REG_RCC_CR     ( (volatile uint32_t*)(uintptr_t) 0x40021000U) // Clock Control Register
#define REG_RCC_CFGR   ( (volatile uint32_t*)(uintptr_t) 0x40021004U) // PLL Configuration Register
#define REG_RCC_AHBENR ( (volatile uint32_t*)(uintptr_t) 0x40021014U) // AHB1 Peripheral Clock Enable Register
#define REG_RCC_CFGR2  ( (volatile uint32_t*)(uintptr_t) 0x4002102CU) // Clock configuration register 2

//----------------
// GPIO Registers
//----------------

#define GPIOA_MODER ( (volatile uint32_t*)(uintptr_t) 0x48000000U) // GPIO port mode register
#define GPIOA_TYPER ( (volatile uint32_t*)(uintptr_t) 0x48000004U) // GPIO port output type register
#define GPIOA_PUPDR ( (volatile uint32_t*)(uintptr_t) 0x4800000CU) // GPIO port pull-up/pull-down register
#define GPIOA_IDR   ( (volatile uint32_t*)(uintptr_t) 0x48000010U) // GPIO port input  data register
#define GPIOA_ODR   ( (volatile uint32_t*)(uintptr_t) 0x48000014U) // GPIO port output data register

//--------------------
// GPIO configuration
//--------------------

#define GPIOA_CLOCKING_BIT    ( 1U << 17U)
#define ENABLE_GPIOA_CLOCKING ( 1U << 17U)

//----------------
// GPIO Registers


#define GPIOC_OUT   ( (volatile uint32_t*)(uintptr_t) 0x48000814U) // GPIO port output data register
#define GPIOC_MODER ( (volatile uint32_t*)(uintptr_t) 0x48000800U) // GPIO port mode register
#define GPIOC_TYPER ( (volatile uint32_t*)(uintptr_t) 0x48000804U) // GPIO port output type register
#define GPIOC_PUPDR ( (volatile uint32_t*)(uintptr_t) 0x4800080CU)
#define GPIOC_IDR   ( (volatile uint32_t*)(uintptr_t) 0x48000810U) // GPIO port input  data register

//------------------------------------------
// A FEW OF THE WORK 1 IMPROVED TO UNIVERSAL
//------------------------------------------

#define PIN8_MODE_BIT ( 0b11U << (2*8U))
#define PIN9_MODE_BIT ( 0b11U << (2*9U))

//----------------------------------
// General purpose output mode macro
//----------------------------------

#define PIN8_GENERAL_OUTPUT_MODE ( 0b01U << (2*8U))
#define PIN9_GENERAL_OUTPUT_MODE ( 0b01U << (2*9U))

#define PIN8_TYPE_BIT ( 0b1U << 8U)
#define PIN9_TYPE_BIT ( 0b1U << 9U)

#define PIN8_OUTPUT_PUSH_PULL_MODE ( 0b0U << 8U)
#define PIN9_OUTPUT_PUSH_PULL_MODE ( 0b0U << 9U)

//====================LAB 3===========================

#define ENABLE_AND_TICKINT_BITS ( 0b11U)
#define ENABLE_AND_TICK ( 0b11U)

#define SET_REG_VALUE(REG, VALUE)  ( *(REG)) = ( VALUE);

#define ZERO ( 0U)

#define RELOAD_BITS ( 0x00FFFFFFU)

//----------------
// GPIO Registers
//----------------

#define GPIOC_ODR   (volatile uint32_t*)(uintptr_t)0x48000814U // GPIO port output data register
#define GPIOC_BSRR  (volatile uint32_t*)(uintptr_t)0x48000818U

//-------------------
// SysTick registers
//-------------------

#define SYSTICK_CSR   (volatile uint32_t*)(uintptr_t)0xE000E010U // SysTick Control and Status Register
#define SYSTICK_RVR   (volatile uint32_t*)(uintptr_t)0xE000E014U // SysTick Reload Value Register
#define SYSTICK_CVR   (volatile uint32_t*)(uintptr_t)0xE000E018U // SysTick Current Value Register
#define SYSTICK_CALIB (volatile uint32_t*)(uintptr_t)0xE000E01CU // SysTick Calibration Value Register



#endif // STM_MACRO_