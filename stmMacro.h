
#ifndef STM_MACRO_
#define STM_MACRO_

//====================LAB 1===========================

// All unmarked bits of the MODIFYMASK REG register will remain unchanged

#define MODIFY_REG(REG, MODIFYMASK, VALUE)  ( *(REG)) = ( ( *(REG)) & ~ ( MODIFYMASK)) | ( ( VALUE) & ( MODIFYMASK) );

#define COMPARE_REG_AND_MASK(REG, COMPARISON_MASK) ( ( *(REG)) & ( COMPARISON_MASK)) ^ ( COMPARISON_MASK)

#define READ_REG(REG, MASK) ( ( *(REG)) & ( MASK) )

//===============================================

#define SET_GREEN_DIODE_BIT  0x100U
#define SET_BLUE_DIODE_BIT   0x200U

#define LIGHT_GREEN_DIODE  0x100U
#define LIGHT_BLUE_DIODE   0x200U
#define TURN_OFF_DIODE     0U

//===============================================

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

//===============================================

#define PC8_MODE_BIT 0b11U << (2*8U)
#define PC9_MODE_BIT 0b11U << (2*9U)

//----------------------------------
// General purpose output mode macro
//----------------------------------

#define PC8_GENERAL_OUTPUT_MODE 0b01U << (2*8U)
#define PC9_GENERAL_OUTPUT_MODE 0b01U << (2*9U)

#define PC8_TYPE_BIT 0b1U << 8U
#define PC9_TYPE_BIT 0b1U << 9U

#define PC8_OUTPUT_PUSH_PULL_MODE 0b0U << 8U
#define PC9_OUTPUT_PUSH_PULL_MODE 0b0U << 9U

#define GPIOC_CLOCKING_BIT    0x80000U
#define ENABLE_GPIOC_CLOCKING 0x80000U

//====================LAB 2===========================

#define PC4_5_MODE_BITS 0b1111U << 8U
#define PC4_5_INPUT_MODE 0b0000U << 8U
#define PC4_5_TYPE_BIT 0b11U << 4U
#define PC4_5_OUTPUT_PUSH_PULL_MODE 0b00U << 4U 
#define PC4_5_PULL_UP_DOWN_BITS 0b1111U << (2U*4U)
#define PC4_5_PULL_DOWN_MODE 0b1010U << (2U*4U)

//===============================================

#define PA0_TYPE_BIT 1U
#define PA0_OUTPUT_PUSH_PULL_MODE 0U

//---------------------------------------
// CONFIGURE THE IO PULL UP OR  PULL DOWN
//---------------------------------------

#define PA0_PULL_UP_DOWN_BITS 0b11U << (2U*0U)
#define PA0_PULL_DOWN_MODE 0b10U << (2U*0U)

//--------------------
// INPUT BITS
//--------------------

#define PA0_INPUT_BIT 1U << 0U
#define PC4_INPUT_BIT 1U << 4U
#define PC5_INPUT_BIT 1U << 5U

//--------------------
// GPIO configuration
//--------------------

#define PA0_to_12_MODE_BITS 0x3FFFFFFU
#define PA0_INPUT_MODE 0x00U
#define PA1_to_12_GENERAL_OUTPUT_MODE 0x1555554U

//====================LAB 3===========================


#endif // STM_MACRO_