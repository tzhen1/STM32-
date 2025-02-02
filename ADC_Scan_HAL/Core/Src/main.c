#include "stm32f4xx_hal.h"

//config functions using the driver's documentation
void GPIO_Config(void); //for the 3 potentiometer pins
void ADC_Config(void);
void DMA_Config(ADC_HandleTypeDef* hadc); //globally defined this way inside the func  (handle just below)

ADC_HandleTypeDef myADChandle; //global need for main/ADC config
DMA_HandleTypeDef myDMA2handle;

uint8_t adcValArray[3];

int main(void)
{
	//CAll funcs
	HAL_Init();
	GPIO_Config();
	ADC_Config();
	DMA_Config(&myADChandle);

	HAL_ADC_Start_DMA(&myADChandle,(uint32_t*) adcValArray,3); //start ADC as DMA , func expects 32bit, so typecast

	while(1)
	{


	}
}



void GPIO_Config(void)
{
	//enable port A clk,
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//config struct gpio type def
	GPIO_InitTypeDef myPinsInit;
	myPinsInit.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2; //PA0,PA1,PA2
	myPinsInit.Mode = GPIO_MODE_ANALOG; //3 analog i/p
	myPinsInit.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &myPinsInit);

}

void ADC_Config(void)
{
	//Enable ADC1 clock
	__HAL_RCC_ADC1_CLK_ENABLE();

	// config ADC, like cubemx
	myADChandle.Instance = ADC1;
	myADChandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	myADChandle.Init.Resolution = ADC_RESOLUTION_8B;
	myADChandle.Init.ScanConvMode = ENABLE;
	myADChandle.Init.ContinuousConvMode = ENABLE;
	myADChandle.Init.DiscontinuousConvMode = DISABLE;
	myADChandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	myADChandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	myADChandle.Init.NbrOfConversion = 3;
	myADChandle.Init.DMAContinuousRequests = ENABLE;
	myADChandle.Init.EOCSelection = ADC_EOC_SEQ_CONV; //end of every seq conversion
	//INIT
	HAL_ADC_Init(&myADChandle);

	//ADC config the channels
	ADC_ChannelConfTypeDef myADC_Channel;

	//settings channel 0
	myADC_Channel.Channel = ADC_CHANNEL_0;
	myADC_Channel.Rank = 1;
	myADC_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&myADChandle, &myADC_Channel);

	//settings channel 2
	myADC_Channel.Channel = ADC_CHANNEL_1;
	myADC_Channel.Rank = 2;
	myADC_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&myADChandle, &myADC_Channel);

	//settings channel 3
	myADC_Channel.Channel = ADC_CHANNEL_2;
	myADC_Channel.Rank = 3;
	myADC_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&myADChandle, &myADC_Channel);
}

void DMA_Config(ADC_HandleTypeDef* hadc) //use driver to config
{

	//Enable DMA clock, find DMA hardware connected to ADC from ref manual
	__HAL_RCC_DMA2_CLK_ENABLE(); //DMA2 connected to ADC

	//config, handle defined globally
	myDMA2handle.Instance = DMA2_Stream0; // on data sheet
	myDMA2handle.Init.Channel = DMA_CHANNEL_0;
	myDMA2handle.Init.Direction = DMA_PERIPH_TO_MEMORY; // ADC to mem
	myDMA2handle.Init.PeriphInc = DMA_PINC_DISABLE; //disable , dont want to incr the address of the periph
	myDMA2handle.Init.MemInc = DMA_MINC_ENABLE; //need incrememnt to store them in different locations
	myDMA2handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; //to byte
	myDMA2handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; //to byte since ADC set to 8bit (1 byte)
	myDMA2handle.Init.Mode = DMA_CIRCULAR; // auto go back 1st position of array
	myDMA2handle.Init.Priority = DMA_PRIORITY_LOW;
	myDMA2handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	//INIT
	HAL_DMA_Init(&myDMA2handle);

	//Link DMA to ADC1 (link the handletypedef together), when ADC starts it can refer to DMA typedef to get its settings
	__HAL_LINKDMA(hadc,DMA_Handle,myDMA2handle);

	//enable DMA interrupt
//	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0); //set priority
//	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn); //enables intterupt, at end of every DMA transaction can make it do stuff
}
