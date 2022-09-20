## TODO

### firmware
* process current fault on PB5
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
  
### sensor settings
  
* stgauge_scale_thrust	2kG -- 1054.501 1/G
* stgauge_scale_thrust	3kG -- 789.475 1/G

* stgauge_scale_torque	1kG -- 1740.526 1/G
* stgauge_scale_torque	2kG -- 1034.647 1/G


* adc_icoeff 	(ACS709)	+- 75A [0.028V/A 5V] 	-- 178.57 - 181.33 corrected (diff 1.5%)
* adc_icoeff	(ACHS-7125)	+- 50A [0.04V/A 5V] 	-- 125
* adc_icoeff	ACHS-7124)	+- 40A [0.05V/A 5V] 	-- 100
* adc_icoeff	(ACS711EX)	+- 15.5A [0.09V/A 3.3V] -- 36.7

