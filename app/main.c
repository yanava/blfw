
#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"
#include "fsm.h"

enum TestSignals
{
    TEST_SIGNAL_A = FSM_USER_SIGNAL,
    TEST_SIGNAL_B
};

typedef struct TestEventTag
{
    FSM_Event super;
    uint8_t   parameter;
} TestEvent;

typedef struct TestMachineTag
{
    FSM super;
    uint8_t unnecessary_property;
} TestMachine;

void TestInit(uint8_t unnecessary_property);
FSM_State FSM_TEST_INITIAL(TestMachine* me, FSM_Event* e);
FSM_State FSM_TEST_STATE_1(TestMachine* me, FSM_Event* e); 
FSM_State FSM_TEST_STATE_2(TestMachine* me, FSM_Event* e); 
void light_up_led(uint8_t value);

TestMachine testMachine;

void TEST_Init(uint8_t unnecessary_property)
{
    FSM_Ctor(&testMachine, &FSM_TEST_INITIAL);
    testMachine.unnecessary_property = unnecessary_property;
        
    FSM_Init((FSM*) &testMachine);
}

FSM_State FSM_TEST_INITIAL(TestMachine* me, FSM_Event* e)
{   
    return FSM_TRAN(me, &FSM_TEST_STATE_1);
}

FSM_State FSM_TEST_STATE_1(TestMachine* me, FSM_Event* e)
{
    switch(e->signal)
    {
      case FSM_ENTRY_SIGNAL:
        me->unnecessary_property = 8;
        return FSM_HANDLED();
      case TEST_SIGNAL_A:
        asm("nop;");
        return FSM_HANDLED();
      case TEST_SIGNAL_B:
        asm("nop;");
        return FSM_TRAN(me,&FSM_TEST_STATE_2);
      case FSM_EXIT_SIGNAL:
        me->unnecessary_property--;
        asm("nop;");
        return FSM_HANDLED();
      default:
        asm("nop;");
        return FSM_IGNORED(); 
    }
    
    
}

FSM_State FSM_TEST_STATE_2(TestMachine* me, FSM_Event* e)
{
    switch(e->signal)
    {
      case FSM_ENTRY_SIGNAL:
        me->unnecessary_property = 1;
        return FSM_HANDLED();
      case TEST_SIGNAL_A:
        asm("nop;");
        return FSM_TRAN(me,&FSM_TEST_STATE_1);
      case TEST_SIGNAL_B:
        asm("nop;");
        return FSM_HANDLED();
      case FSM_EXIT_SIGNAL:
        me->unnecessary_property++;
        asm("nop;");
        return FSM_HANDLED();
      default:
        asm("nop;");
        return FSM_IGNORED();
    }
}

void light_up_led(uint8_t value)
{
    uint16_t gpio_value = (value << 1) & (0x1E);
                                        
    GPIO_Write (GPIOE, gpio_value);
}

// Main function. Here's where the action happens!
void main(void)
{
    
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    // Init GPIOE Outputs
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // Init GPIOE Inputs
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);  
    
    SYSTICK_Init();    // System timer tick initialization
    DAC_HwInit();      // DAC Init
    USART2_Init();     // USART2 Init - Wifi
    
    TEST_Init(1);    
    
    TestEvent testEvent;
    
    // Main loop will execute forever
    for(;;)
    {
        if (!GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_5))
        {
            testEvent.super.signal = TEST_SIGNAL_A;
            FSM_Dispatch((FSM *)&testMachine,(FSM_Event *)&testEvent); 
        }
        else if (!GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_6))
        {
            testEvent.super.signal = TEST_SIGNAL_B;
            FSM_Dispatch((FSM *)&testMachine,(FSM_Event *)&testEvent); 
        }
        light_up_led(testMachine.unnecessary_property);
    }
    
}


