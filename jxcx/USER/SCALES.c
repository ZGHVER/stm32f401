#include "SCALES.h"


volatile uint16_t ADCval[100];

__STATIC_INLINE void SCALES_ADC_Init(){

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_CommonInitTypeDef adc1;
    adc1.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    adc1.ADC_Mode = ADC_Mode_Independent;
    adc1.ADC_Prescaler = ADC_Prescaler_Div4;
    adc1.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;
    ADC_CommonInit(&adc1);

    ADC_InitTypeDef adc1_init;
    ADC_StructInit(&adc1_init);
    adc1_init.ADC_Resolution = ADC_Resolution_12b;
    adc1_init.ADC_ContinuousConvMode = ENABLE;
    adc1_init.ADC_ScanConvMode = ENABLE;
    adc1_init.ADC_DataAlign = ADC_DataAlign_Right;
    adc1_init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    adc1_init.ADC_NbrOfConversion = 2;
    ADC_Init(ADC1, &adc1_init);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_56Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_56Cycles);
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    

    ADC_Cmd(ADC1, ENABLE);
}

__STATIC_INLINE void SCALES_DMA_Init(){
   DMA_InitTypeDef DMA2_CH0_STR0;  //DMA2通道0数据流4,外设ADC1的寄存器0x4c和数组ADCval之间传递100个数据,DMA初始化
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
   DMA_DeInit(DMA2_Stream0);
   while(DMA_GetCmdStatus(DMA2_Stream0) != DISABLE);

   DMA2_CH0_STR0.DMA_PeripheralBaseAddr = (uint32_t)ADC1+0x4c;             //外设基址为
   DMA2_CH0_STR0.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       //地址是否递增
   DMA2_CH0_STR0.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;      //突发模式
   DMA2_CH0_STR0.DMA_Memory0BaseAddr    = (uint32_t)(&ADCval[0]);           //存储器地址
   DMA2_CH0_STR0.DMA_MemoryInc          = DMA_MemoryInc_Enable;           //地址是否递增
   DMA2_CH0_STR0.DMA_MemoryBurst        = DMA_MemoryBurst_Single;          //突发模式
   DMA2_CH0_STR0.DMA_DIR                = DMA_DIR_PeripheralToMemory;      //数据传输方向
   DMA2_CH0_STR0.DMA_BufferSize         = 100;                               //缓冲区大小为，指一次传输的数据量
   DMA2_CH0_STR0.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //外设数据大小为 Byte:8 HalfWord:16 Word:32
   DMA2_CH0_STR0.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;     //存储器数据大小
   DMA2_CH0_STR0.DMA_Mode               = DMA_Mode_Circular;               //循环传输模式
   DMA2_CH0_STR0.DMA_Priority           = DMA_Priority_Medium;               //DMA传输通道优先级
   DMA2_CH0_STR0.DMA_FIFOMode           = DMA_FIFOMode_Disable;            //是否使用FIFO
   DMA2_CH0_STR0.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;      // FIFO大小
   DMA2_CH0_STR0.DMA_Channel            = DMA_Channel_0;                   //选择 DMA 通道
   DMA_Init(DMA2_Stream0, &DMA2_CH0_STR0);                                //初始化DMA流

   
   while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){};

   DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
   NVIC_InitTypeDef dma_NvicInit;
   dma_NvicInit.NVIC_IRQChannel = DMA2_Stream0_IRQn;       //中断频道
   dma_NvicInit.NVIC_IRQChannelCmd = ENABLE;
   dma_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
   dma_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
   NVIC_Init(&dma_NvicInit);

   DMA_Cmd(DMA2_Stream0, ENABLE);  
   
    
}

__STATIC_INLINE void SCALES_GPIO_Init(){
    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    ioA.GPIO_Mode = GPIO_Mode_AN;
    ioA.GPIO_PuPd = GPIO_PuPd_NOPULL;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);
}

void SCALES_Init(){
    SCALES_GPIO_Init();
    SCALES_DMA_Init();
    SCALES_ADC_Init();
}
