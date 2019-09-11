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
   DMA_InitTypeDef DMA2_CH0_STR0;  //DMA2ͨ��0������4,����ADC1�ļĴ���0x4c������ADCval֮�䴫��100������,DMA��ʼ��
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
   DMA_DeInit(DMA2_Stream0);
   while(DMA_GetCmdStatus(DMA2_Stream0) != DISABLE);

   DMA2_CH0_STR0.DMA_PeripheralBaseAddr = (uint32_t)ADC1+0x4c;             //�����ַΪ
   DMA2_CH0_STR0.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       //��ַ�Ƿ����
   DMA2_CH0_STR0.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;      //ͻ��ģʽ
   DMA2_CH0_STR0.DMA_Memory0BaseAddr    = (uint32_t)(&ADCval[0]);           //�洢����ַ
   DMA2_CH0_STR0.DMA_MemoryInc          = DMA_MemoryInc_Enable;           //��ַ�Ƿ����
   DMA2_CH0_STR0.DMA_MemoryBurst        = DMA_MemoryBurst_Single;          //ͻ��ģʽ
   DMA2_CH0_STR0.DMA_DIR                = DMA_DIR_PeripheralToMemory;      //���ݴ��䷽��
   DMA2_CH0_STR0.DMA_BufferSize         = 100;                               //��������СΪ��ָһ�δ����������
   DMA2_CH0_STR0.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //�������ݴ�СΪ Byte:8 HalfWord:16 Word:32
   DMA2_CH0_STR0.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;     //�洢�����ݴ�С
   DMA2_CH0_STR0.DMA_Mode               = DMA_Mode_Circular;               //ѭ������ģʽ
   DMA2_CH0_STR0.DMA_Priority           = DMA_Priority_Medium;               //DMA����ͨ�����ȼ�
   DMA2_CH0_STR0.DMA_FIFOMode           = DMA_FIFOMode_Disable;            //�Ƿ�ʹ��FIFO
   DMA2_CH0_STR0.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;      // FIFO��С
   DMA2_CH0_STR0.DMA_Channel            = DMA_Channel_0;                   //ѡ�� DMA ͨ��
   DMA_Init(DMA2_Stream0, &DMA2_CH0_STR0);                                //��ʼ��DMA��

   
   while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){};

   DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
   NVIC_InitTypeDef dma_NvicInit;
   dma_NvicInit.NVIC_IRQChannel = DMA2_Stream0_IRQn;       //�ж�Ƶ��
   dma_NvicInit.NVIC_IRQChannelCmd = ENABLE;
   dma_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //��ռ���ȼ��������ȼ�
   dma_NvicInit.NVIC_IRQChannelSubPriority = 0;            //�ύ���ȼ��������ȼ�
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
