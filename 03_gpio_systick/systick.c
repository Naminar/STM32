
#include <stdint.h>
#include <stdbool.h>
#include "../stmMacro.h"

bool shine_on_flag;
int user_flag;
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

void systick_init(uint32_t period_us);
void systick_handler(void);

int main()
{
    shine_on_flag = false;
    user_flag     = 0;

    board_clocking_init();

    systick_init(100U);

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
        }

        if ( saved_right_pressed && !saved_left_pressed && left_button.pressed )
        {
            seg7.number += 100U;

            left_button.pressed  = 0U;
            right_button.pressed = 0U;

            shine_box.light_index   = 100U; 
            shine_box.user          = 0; 
            shine_box.mode          = 1;
        }

        if ( shine_box.mode == 1 )
        {
            shine_on_flag = true;
            
            if ( shine_box.user == 0 ) 
            {        
                user_flag = 0;
                
                if ( shine_box.light_index % 20 >= 1 )
                {                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE)
                    
                    //MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
                }
                else
                {
                    //MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)
                
                    MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
                }
                
            }
            else
            {
                user_flag = 1;
                
                if ( shine_box.light_index % 20 < 1 )
                {                
                    //MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE)

                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)
                }
                else
                {
                    MODIFY_REG(GPIOC_OUT, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)
                
                    //MODIFY_REG(GPIOC_OUT, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
                }
            }

            --shine_box.light_index;

            if ( shine_box.light_index == 0)
            {
                shine_box.mode = 0;
                shine_on_flag  = false;

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

        if ( this_button->saturation == 5U)
            this_button->pressed = 1U;
    }
    else
    {
        if ( this_button->saturation > 0U )
        {
            this_button->saturation -= 1U;
        }

        if ( this_button->saturation == 1U)
            this_button->pressed = 0U; 
    }
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

    
    if (shine_on_flag)
    {
        if (handler_ticks % 80 < 4)//(handler_ticks < 10000)
        {
            //** handler_ticks = 0U;

            //uint32_t reg_gpio_odr = *GPIOC_ODR;
            //*GPIOC_ODR = (reg_gpio_odr & ~0x0100U) | (~reg_gpio_odr & 0x0100U);
            
            // MISTAKE HERE ^^^
            // this laps didn't light on and off
            
            //GPIOC_BSRR = 0x48000818U; // ADDED
            //0x18 - BSRR
            if (user_flag == 1)
                MODIFY_REG(GPIOC_BSRR, SET_GREEN_DIODE_BIT, LIGHT_GREEN_DIODE) // ADDED
            else
                MODIFY_REG(GPIOC_BSRR, SET_BLUE_DIODE_BIT, LIGHT_BLUE_DIODE)

            //MODIFY_REG(GPIOC_ODR, 0x0100U, 0x0100U)

            //** more_precise_delay_forbidden_by_quantum_mechanics_1000ms();

            //MODIFY_REG(GPIOC_ODR, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
            //** MODIFY_REG(GPIOC_ODR, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
        }
        else if (handler_ticks < 20000)
        {
            if (user_flag == 1)
                MODIFY_REG(GPIOC_ODR, SET_GREEN_DIODE_BIT, TURN_OFF_DIODE)
            else
                MODIFY_REG(GPIOC_ODR, SET_BLUE_DIODE_BIT, TURN_OFF_DIODE)

        }
        else
            handler_ticks = 0U;
    }
    else {}
}


