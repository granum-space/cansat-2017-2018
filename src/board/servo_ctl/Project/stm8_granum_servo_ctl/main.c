/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"    // SDCC requires ISR declaration to be included here

#include "stm8s_gpio.h"
#include "stm8s_tim2.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void blink(void) {

	uint8_t port = GPIO_ReadOutputData(GPIOC);
	port ^= (1 << 3);
	GPIO_Write(GPIOC, port);
}

void delay(long int cycles) {

	for (long int i = 0; i < cycles; i++) {

		volatile int x = 13;
	}
}

void rotate(int angle) {

	uint16_t time = (angle / 180.0) * 2000 + 2000;
	TIM2_SetCompare1(time);
}

void main(void) {
	TIM2_TimeBaseInit(TIM2_PRESCALER_1, 40000);
	TIM2_OC1Init(TIM2_OCMODE_PWM2, TIM2_OUTPUTSTATE_ENABLE, 4000, TIM2_OCPOLARITY_LOW);
	TIM2_OC1PreloadConfig(ENABLE);


	TIM2_Cmd(ENABLE);

	GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_OD_LOW_FAST);
	//GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_SLOW);

	while(1) {

		for(int i = 0; i < 3; i++) {

			rotate(180);
			delay(30000);
			rotate(0);
			delay(30000);

		}

		for(int i = 0; i < 3; i++) {

					rotate(180);
					delay(30000);
					rotate(0);
					delay(90000);

		}

	}
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
	(void)file;
	(void)line;
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
