#include <stdint.h>
#include <stdbool.h>
#include "../stmMacro.h"

//-------------------
// 7-segment display
//-------------------

// SUPER-DUPER TRUSTWORTHY Pin Mapping:
#define A  0x0800U
#define B  0x0080U
#define C  0x0010U
#define D  0x0004U
#define E  0x0002U
#define F  0x0400U
#define G  0x0020U
#define DP 0x0008U

#define POS0 0x0040U
#define POS1 0x0100U
#define POS2 0x0200U
#define POS3 0x1000U

static const uint32_t PINS_USED = A|B|C|D|E|F|G|DP|POS0|POS1|POS2|POS3;

// TOTALLY CORRECT digit composition:
static const uint32_t DIGITS[10] =
{
    A|B|C|D|E|F,   // 0
    B|C,           // 1
    A|B|G|E|D,     // 2
    A|B|G|C|D,     // 3
    F|G|B|C,       // 4
    A|F|G|C|D,     // 5
    A|F|G|C|D|E,   // 6
    A|B|C,         // 7
    A|B|C|D|E|F|G, // 8
    G|F|A|B|C|D    // 9
};

static const uint32_t POSITIONS[4] =
{
    /*     POS1|POS2|POS3, // 0
    POS0     |POS2|POS3, // 1
    POS0|POS1     |POS3, // 2
    POS0|POS1|POS2       // 3 */

    POS0|POS1|POS2     , // 3
    POS0|POS1     |POS3, // 2
    POS0     |POS2|POS3, // 1
         POS1|POS2|POS3 // 0           
};

// Display state:
struct Seg7Display
{
    uint32_t display;
    uint16_t number;
};

void SEG7_set_number_quarter(struct Seg7Display* seg7, unsigned tick)
{
    uint32_t divisors[4] = {1, 10, 100, 1000};

    unsigned quarter = tick % 4;
    unsigned divisor = divisors[quarter];

    seg7->display = DIGITS[(seg7->number / divisor) % 10] | POSITIONS[quarter];
}

// Write changes to microcontroller:
void SEG7_push_display_state_to_mc(struct Seg7Display* seg7)
{
    uint32_t surrounding_state = ~PINS_USED & *GPIOA_ODR;
    uint32_t to_write = PINS_USED & seg7->display;

    *GPIOA_ODR = surrounding_state | to_write;
}

//-------------------
// RCC configuration
//-------------------

//#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
//#define ONE_MILLISECOND CPU_FREQENCY/1000U
/*
void board_clocking_init()
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

void to_get_more_accuracy_pay_2202_2013_2410_3805_1ms()
{
    for (uint32_t i = 0; i < ONE_MILLISECOND/3U; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}

void board_gpio_init()
{
    //===================STOP BUTTON & INDICATOR========================
    
    // (1) Configure PA1-PA12 as output:
    MODIFY_REG(REG_RCC_AHBENR, GPIOA_CLOCKING_BIT, ENABLE_GPIOA_CLOCKING)
    
    // Configure mode register:
    MODIFY_REG(GPIOA_MODER, PIN0_to_12_MODE_BITS, PIN0_INPUT_MODE | PIN1_to_12_GENERAL_OUTPUT_MODE)

    // Configure type register:
    MODIFY_REG(GPIOA_TYPER, PIN0_TYPE_BIT, PIN0_OUTPUT_PUSH_PULL_MODE)

    // (2) Configure PA0 as button:
    //*GPIOA_MODER |= 0U;
    // ?????

    // Configure PA0 as pull-down pin:
    MODIFY_REG(GPIOA_PUPDR, PIN0_PULL_UP_DOWN_BITS, PIN0_PULL_DOWN_MODE)

    //=====================BUTTON======================

    MODIFY_REG(REG_RCC_AHBENR, GPIOC_CLOCKING_BIT, ENABLE_GPIOC_CLOCKING)
    
    MODIFY_REG(GPIOC_MODER, PIN4_5_MODE_BITS, PIN4_5_INPUT_MODE)
    
    MODIFY_REG(GPIOC_TYPER, PIN4_5_TYPE_BIT, PIN4_5_OUTPUT_PUSH_PULL_MODE)
    
    MODIFY_REG(GPIOC_PUPDR, PIN4_5_PULL_UP_DOWN_BITS, PIN4_5_PULL_DOWN_MODE)
    
    //===================LIGHTS========================
    
    MODIFY_REG(GPIOC_MODER, PIN8_MODE_BIT, PIN8_GENERAL_OUTPUT_MODE)
    
    MODIFY_REG(GPIOC_TYPER, PIN8_TYPE_BIT, PIN8_OUTPUT_PUSH_PULL_MODE)

    MODIFY_REG(GPIOC_MODER, PIN9_MODE_BIT, PIN9_GENERAL_OUTPUT_MODE)

    MODIFY_REG(GPIOC_TYPER, PIN9_TYPE_BIT, PIN9_OUTPUT_PUSH_PULL_MODE)
}

typedef struct button {

    uint32_t saturation;
    bool active;
    bool pressed;

} button;

typedef struct shine_setting {

    uint32_t light_index;

    bool user; // 0 or 1

    bool mode; // 0 - light is off, 1 - light is on

} shine_setting;

void instant_button_activation  (button* this_button);
void general_button_activation  (button* this_button);
void step_system_butt_activation(button* this_button);

void totally_accurate_quantum_femtosecond_precise_super_delay_3000_1ms()
{
    for (uint32_t i = 0; i < 1U * ONE_MILLISECOND; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}

//------
// Main
//------

int main()
{
    board_clocking_init();

    board_gpio_init();

    // Init display rendering:
    struct Seg7Display seg7 =
    {
        .number = 0
    };

    shine_setting shine_box = { 
        .light_index = 0U, 
        .user = 0, 
        .mode = 0
    };

    uint32_t tick = 0;
    
    button stop_button  = { .saturation = 0U, .pressed = 0U}, 
           left_button  = { .saturation = 0U, .pressed = 0U}, 
           right_button = { .saturation = 0U, .pressed = 0U};


    bool saved_left_pressed, saved_right_pressed;

    while ( 1 )
    {
        saved_left_pressed = left_button.pressed;
        saved_right_pressed = right_button.pressed;
        
        // Update button state:
        stop_button.active  = READ_REG(GPIOA_IDR, PIN0_INPUT_BIT); 
        left_button.active  = READ_REG(GPIOC_IDR, PIN4_INPUT_BIT); 
        right_button.active = READ_REG(GPIOC_IDR, PIN5_INPUT_BIT);

        
        step_system_butt_activation (&stop_button); // general_button_activation(&stop_button); 
        step_system_butt_activation (&left_button); // instant_button_activation(&left_button);
        step_system_butt_activation (&right_button);// instant_button_activation(&right_button);

        if ( saved_left_pressed && !saved_right_pressed && right_button.pressed )
        {
            seg7.number += 1U;

            left_button.pressed  = 0U;
            right_button.pressed = 0U;

            shine_box.light_index   = 100U; 
            shine_box.user          = 1; 
            shine_box.mode          = 1;            

            /*for (uint32_t i = 0; i < 100U; i++)
            {        
                if (i % 20 >= 1)
                {                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE)

                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
                }
                else
                {
                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)
                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
                }

                totally_accurate_quantum_femtosecond_precise_super_delay_3000_1ms();
            }

            MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
            MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)*/
        }

        if ( saved_right_pressed && !saved_left_pressed && left_button.pressed )
        {
            seg7.number += 100U;

            left_button.pressed  = 0U;
            right_button.pressed = 0U;

            /*for ( uint32_t i = 0; i < 100U; i++ )
            {        
                if ( i % 20 < 1 )
                {                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE)

                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
                }
                else
                {
                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)
                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
                }

                totally_accurate_quantum_femtosecond_precise_super_delay_3000_1ms();
            }

            MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
            MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)*/

            shine_box.light_index   = 100U; 
            shine_box.user          = 0; 
            shine_box.mode          = 1;
        }

        if ( shine_box.mode == 1 )
        {
            if ( shine_box.user == 0 ) 
            {        
                if ( shine_box.light_index % 20 >= 1 )
                {                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE)

                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
                }
                else
                {
                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)
                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
                }
                
            }
            else
            {
                if ( shine_box.light_index % 20 < 1 )
                {                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE)

                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
                }
                else
                {
                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)
                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
                }
            }

            --shine_box.light_index;

            if ( shine_box.light_index == 0)
            {
                shine_box.mode = 0;

                // SET OFF ALL LIGHTS
                MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
                MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
            }
        }

        if ( stop_button.pressed )
        {
            seg7.number = 0U;
        }

        // Render display state:
        SEG7_set_number_quarter(&seg7, tick);

        SEG7_push_display_state_to_mc(&seg7);

        // Adjust ticks every ms:
        to_get_more_accuracy_pay_2202_2013_2410_3805_1ms();
        tick += 1;
    }
}

void instant_button_activation(button* this_button)
{
    if ( this_button->active )
    {
        if ( this_button->saturation < 5U )
        {
            this_button->saturation += 1U;
        }
        else
        {
            this_button->pressed = 1U;
            
            this_button->saturation = 0U; //<-- RESET OF SATURATION
        }
    }
    else
    {
        this_button->pressed = 0U; //<-- RESET OF PRESSED
        
        this_button->saturation = 0U;
    }
}

void general_button_activation(button* this_button)
{
    if ( this_button->active )
    {
        if ( this_button->saturation < 5U )
        {
            this_button->saturation += 1U;
        }
        else
        {
            this_button->pressed = 1U;
            
            this_button->saturation = 0U; //<-- RESET OF SATURATION
        }
    }
    else
    {   
        this_button->saturation = 0U; //<-- NO RESET OF PRESSED
    }
}

//-------------------------
// hysteresis - step system 
//-------------------------

//================= 1__1 ==================== 6U
//-------------- 1__|--|__1 ----------------- 5U
//            0__|        |__1
//         0__|              |__1
//      0__|                     |__1  
//-- 0__|---------------------------|__0 ----- 1U
//0__|=================================|__0 == 0U

void step_system_butt_activation(button* this_button)
{
    if ( this_button->active )
    {
        if ( this_button->saturation < 6U )
        {
            this_button->saturation += 1U;
        }
        /*else
        {
            this_button->pressed = 1U;
            
            this_button->saturation = 0U; //<-- RESET OF SATURATION
        }*/

        if ( this_button->saturation == 5U)
            this_button->pressed = 1U;
    }
    else
    {
        if ( this_button->saturation > 0U )
        {
            this_button->saturation -= 1U;
        }

        /*this_button->pressed = 0U; //<-- RESET OF PRESSED
        
        this_button->saturation = 0U; */

        if ( this_button->saturation == 1U)
            this_button->pressed = 0U; 
    }
}