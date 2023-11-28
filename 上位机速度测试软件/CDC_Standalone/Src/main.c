/**
******************************************************************************
* @file    USB_Device/CDC_Standalone/Src/main.c
* @author  MCD Application Team
* @version V1.1.0
* @date    26-June-2014
* @brief   USB device CDC application main file
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#include "main.h"
#include <stdarg.h>			/* 因为用到了va_start等va_宏，所以必须包含这个文件 */
#include <stdio.h>			/* 因为用到了printf函数，所以必须包含这个文件 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef  USBD_Device;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Toggle_Leds(void);
void	UsbPrintf(const char* lpszFormat, ...);
uint8_t	UsbSendData(uint8_t* pBuf, uint16_t nLen);

#define TxBufSize 20480
int bSendMark = 0;				//// 发送数据的标志
uint8_t SendData[TxBufSize];

extern uint8_t UserRxBuffer[2][APP_RX_DATA_SIZE];//// 用双缓存保存数据
extern uint32_t nRxLength;		//// 接收到的数据长度
extern uint8_t uRxBufIndex;		//// 当前使用的缓冲区索引号
extern uint8_t  uLastRxBufIndex;//// 上次使用的接收缓冲区索引号
/* Private functions ---------------------------------------------------------*/ 

/**
* @brief  Main program
* @param  None
* @retval None
*/
int main(void)
{
	int i;
	/* STM32F4xx HAL library initialization:
	- Configure the Flash prefetch, instruction and Data caches
	- Configure the Systick to generate an interrupt each 1 msec
	- Set NVIC Group Priority to 4
	- Global MSP (MCU Support Package) initialization
	*/
	HAL_Init();

	/* Configure the system clock to 168 Mhz */
	SystemClock_Config();

#ifdef USE_USB_FS
	/* Configure LED3, LED4, LED5 and LED6 */
	BSP_LED_Init(LED3);			//// HS模式下LED3与USB_RESET引脚冲突，因此LED代码仅在FS模式下使用
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
#endif

	/* Init Device Library */
	USBD_Init(&USBD_Device, &VCP_Desc, 0);

	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

	/* Add CDC Interface Class */
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

	/* Start Device Process */
	USBD_Start(&USBD_Device);

	/* Run Application (Interrupt mode) */
	while (1)
	{
		Toggle_Leds();

		// 处理接收到的数据：解码数据流中的指令。可以改成你自己的数据处理过程。
		// 每次CDC_Itf_Receive()接收到新数据都会更换缓存，所以发现缓存切换过就是有新的数据。
		// 这里必须保证数据处理的速度足够快，否则缓存切换了两次才处理的话，就没法识别有新数据到来了。
		if (uLastRxBufIndex != uRxBufIndex)
		{
			// --> 指令译码开始。
			for (i=0; i<nRxLength; i++)
			{
				if (UserRxBuffer[uLastRxBufIndex][i] == 0x55)	// 0x55, 开始发送数据指令
					bSendMark = 1;
				if (UserRxBuffer[uLastRxBufIndex][i] == 0xAA)	// 0xAA, 停止发送数据指令
					bSendMark = 0;
			}
			// <-- 指令译码结束。
			uLastRxBufIndex = (uLastRxBufIndex + 1) & 1;
		}

		if (bSendMark)
		{
			int32_t k = 0;
			while (UsbSendData(SendData, TxBufSize) != USBD_OK)
				k++;
		}
	}
}

/**
* @brief  System Clock Configuration
*         The system Clock is configured as follow : 
*            System Clock source            = PLL (HSE)
*            SYSCLK(Hz)                     = 168000000
*            HCLK(Hz)                       = 168000000
*            AHB Prescaler                  = 1
*            APB1 Prescaler                 = 4
*            APB2 Prescaler                 = 2
*            HSE Frequency(Hz)              = 8000000 / 24000000
*            PLL_M                          = 8 / 24
*            PLL_N                          = 336
*            PLL_P                          = 2
*            PLL_Q                          = 7
*            VDD(V)                         = 3.3
*            Main regulator output voltage  = Scale1 mode
*            Flash Latency(WS)              = 5
* @param  None
* @retval None
*/
static void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInit;
	RCC_ClkInitTypeDef RCC_ClkInit;

	/* Enable Power Control clock */
	__PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is 
	clocked below the maximum system frequency, to update the voltage scaling value 
	regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Configure RCC Oscillators: All parameters can be changed according to user抯 needs */
	RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInit.HSEState = RCC_HSE_ON;
	RCC_OscInit.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInit.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	#ifdef USE_USB_HS
		RCC_OscInit.PLL.PLLM = 24;	//// Microendoscope
	#else
		RCC_OscInit.PLL.PLLM = 8;	//// STM32F4Discovery
	#endif
	RCC_OscInit.PLL.PLLN = 336;
	RCC_OscInit.PLL.PLLP = 2;
	RCC_OscInit.PLL.PLLQ = 7;
	HAL_RCC_OscConfig (&RCC_OscInit);

	/* RCC Clocks: All parameters can be changed according to user抯 needs */
	RCC_ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_HCLK |RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInit.APB1CLKDivider = RCC_HCLK_DIV4;  
	RCC_ClkInit.APB2CLKDivider = RCC_HCLK_DIV2;  
	HAL_RCC_ClockConfig(&RCC_ClkInit, FLASH_LATENCY_5);
}

/**
* @brief  Toggles LEDs.
* @param  None
* @retval None
*/
static void Toggle_Leds(void)
{
	static uint32_t ticks;

	if(ticks++ == 0xfffff)
	{
#ifdef USE_USB_FS
		BSP_LED_Toggle(LED3);
		BSP_LED_Toggle(LED4);
		BSP_LED_Toggle(LED5);
		BSP_LED_Toggle(LED6);
#endif
		ticks = 0;

		// UsbPrintf("I'm Happy!\r\n");
	}
}

/*
*********************************************************************************************************
*	函 数 名: UsbSendData
*	功能说明: 向虚拟串口发送数据
*	形    参：pBuf, 数据缓冲地址
*			  nLen, 数据长度，以字节为单位
*	返 回 值:	=USBD_OK，发送成功
*				=USBD_BUSY，Tx忙，需要重发
*				=USBD_FAIL，发送失败
*	注    释：
*	作    者：碧云天书
*********************************************************************************************************
*/
uint8_t UsbSendData(uint8_t* pBuf, uint16_t nLen)
{
	USBD_CDC_SetTxBuffer(&USBD_Device, (uint8_t*)pBuf, nLen);
	return USBD_CDC_TransmitPacket(&USBD_Device);
}

/*
*********************************************************************************************************
*	函 数 名: UsbPrintf
*	功能说明: 向虚拟串口打印字符串, 接上串口线后，打开PC机的超级终端软件可以观察结果。语法与printf相同。
*	形    参：lpszFormat, 格式描述字串
*			  ..., 不定参数
*	返 回 值: 无
*	注    释：
*	作    者：碧云天书
*********************************************************************************************************
*/
#define CMD_BUFFER_LEN 120
void UsbPrintf(const char* lpszFormat, ...)
{
	int nLen;
	char szBuffer[CMD_BUFFER_LEN+1];
	va_list args;
	va_start(args, lpszFormat);
	nLen = vsnprintf(szBuffer, CMD_BUFFER_LEN+1, lpszFormat, args);
	UsbSendData((uint8_t*)szBuffer, nLen);
	va_end(args);
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
