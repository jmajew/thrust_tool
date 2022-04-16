## TODO

### firmware
* RPM measurement fixing and testing
* Add msg_id to DATA_STREAM messages
* ChibiOS EFL driver for internal flash?
* check RTC for time stamp
* DSHOT output, DMA and PWM cycle
* ChibiOS equivalent of:
  - `HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_1);`
  - `HAL_DMA_Start_IT(&hdma_tim4_ch1, (uint32_t)Wave_LUT, DestAddress, NS);`
  - `__HAL_TIM_ENABLE_DMA(&htim4, TIM_DMA_CC1);`

### hardware v2
* low-pass filter for ADC
  
  

