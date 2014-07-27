#include "dac.h"

void DAC_HwInit()
{
    
    // GPIO and DAC Init Structures
    GPIO_InitTypeDef  GPIO_InitStructure;
    DAC_InitTypeDef   DAC_InitStructure;

    // Clock enable to DAC and the associated GPIOA
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // Connects PA4 and PA5, which are respectively DAC1 and DAC2, to Analog
    // Mode with no PullUp
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Initializing both DACs with no Trigger and no Wave Generation. Output
    // buffer is enabled to allow driving loads directly, even though there is
    // an LMC6484 for that purpose on the board. 
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    // Enables both channels
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);
    
    DAC_SetDAC1ValInMilivolts(10);
    
}


// Set DAC1 Value in Milivolts
int DAC_SetDAC1ValInMilivolts(uint16_t val)
{
    // If value is above reference voltage, return an error
    if (val > DAC_VREF_PLUS) return DAC_VALUE_OUTSIDE_BOUNDARIES;
    
    // Set the voltage using the formula on Page 152 of UM1061
    DAC_SetChannel1Data(DAC_Align_12b_R, val * DAC_MAX_VALUE / DAC_VREF_PLUS);
           
    // If everything went right, no error is returned
    return DAC_NOERROR;
}

// Set DAC2 Value in Milivolts
int DAC_SetDAC2ValInMilivolts(uint16_t val)
{
    // If value is above reference voltage, return an error
    if (val > DAC_VREF_PLUS) return DAC_VALUE_OUTSIDE_BOUNDARIES;
    
    // Set the voltage using the formula on Page 152 of UM1061
    DAC_SetChannel2Data(DAC_Align_12b_R, val * DAC_MAX_VALUE / DAC_VREF_PLUS);
    
    // If everything went right, no error is returned
    return DAC_NOERROR;
}

// Set both DACs in Milivolts, in case you need them synchronized
int DAC_SetBothDacsInMilivolts(uint16_t dac1_val, uint16_t dac2_val)
{
    // If either value is above reference voltage, return an error
    if (dac1_val > DAC_VREF_PLUS || dac2_val > DAC_VREF_PLUS) 
        return DAC_VALUE_OUTSIDE_BOUNDARIES;
    
    // Set both voltages using the formula on Page 152 of UM1061
    DAC_SetDualChannelData(DAC_Align_12b_R, 
                           dac2_val * DAC_MAX_VALUE / DAC_VREF_PLUS,
                           dac1_val * DAC_MAX_VALUE / DAC_VREF_PLUS);

    // If everything went right, no error is returned
    return DAC_NOERROR;

}
