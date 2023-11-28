/**
******************************************************************************
* @file    USB_Device/CDC_Standalone/Src/usbd_cdc_interface.c
* @author  MCD Application Team
* @version V1.1.0
* @date    26-June-2014
* @brief   Source file for USBD CDC interface
* @Modified by Bluesky
* @date    2014年10月4日
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_CDC_LineCodingTypeDef LineCoding =
{
	115200, /* baud rate*/
	0x00,   /* stop bits-1*/
	0x00,   /* parity - none*/
	0x08    /* nb. of bits 8*/
};

uint8_t UserRxBuffer[2][APP_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */	//// 用双缓存保存PC发来的数据
uint32_t nRxLength;							//// 接收到的数据长度
uint8_t uRxBufIndex = 0;					//// 当前使用的缓冲区索引号
uint8_t  uLastRxBufIndex = 0;				//// 上次使用的接收缓冲区索引号
/* USB handler declaration */
extern USBD_HandleTypeDef  USBD_Device;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init     (void);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (uint8_t* pbuf, uint32_t *Len);

//// static void Error_Handler(void);

USBD_CDC_ItfTypeDef USBD_CDC_fops = 
{
	CDC_Itf_Init,
	CDC_Itf_DeInit,
	CDC_Itf_Control,
	CDC_Itf_Receive
};

/* Private functions ---------------------------------------------------------*/

/**
* @brief  CDC_Itf_Init
*         Initializes the CDC media low layer
* @param  None
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_Itf_Init(void)
{
	USBD_CDC_SetRxBuffer(&USBD_Device, &UserRxBuffer[uRxBufIndex][0]);

	return (USBD_OK);
}

/**
* @brief  CDC_Itf_DeInit
*         DeInitializes the CDC media low layer
* @param  None
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_Itf_DeInit(void)
{
	return (USBD_OK);
}

/**
* @brief  CDC_Itf_Control
*         Manage the CDC class requests
* @param  Cmd: Command code            
* @param  Buf: Buffer containing command data (request parameters)
* @param  Len: Number of data to be sent (in bytes)
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
	switch (cmd)
	{
	case CDC_SEND_ENCAPSULATED_COMMAND:
		/* Add your code here */
		break;

	case CDC_GET_ENCAPSULATED_RESPONSE:
		/* Add your code here */
		break;

	case CDC_SET_COMM_FEATURE:
		/* Add your code here */
		break;

	case CDC_GET_COMM_FEATURE:
		/* Add your code here */
		break;

	case CDC_CLEAR_COMM_FEATURE:
		/* Add your code here */
		break;

	case CDC_SET_LINE_CODING:
		LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) |\
			(pbuf[2] << 16) | (pbuf[3] << 24));
		LineCoding.format     = pbuf[4];
		LineCoding.paritytype = pbuf[5];
		LineCoding.datatype   = pbuf[6];

		/* Set the new configuration */
		//// ComPort_Config();
		break;

	case CDC_GET_LINE_CODING:
		pbuf[0] = (uint8_t)(LineCoding.bitrate);
		pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
		pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
		pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
		pbuf[4] = LineCoding.format;
		pbuf[5] = LineCoding.paritytype;
		pbuf[6] = LineCoding.datatype;     
		break;

	case CDC_SET_CONTROL_LINE_STATE:
		/* Add your code here */
		break;

	case CDC_SEND_BREAK:
		/* Add your code here */
		break;    

	default:
		break;
	}

	return (USBD_OK);
}

/**
* @brief  CDC_Itf_DataRx
*         Data received over USB OUT endpoint are sent over CDC interface 
*         through this function.
* @param  Buf: Buffer of data to be transmitted
* @param  Len: Number of data received (in bytes)
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
extern int bSendMark;
static int8_t CDC_Itf_Receive(uint8_t* Buf, uint32_t *Len)
{
	//// HAL_UART_Transmit_DMA(&UartHandle, Buf, *Len);

	nRxLength = *Len;
	//// 接收到的数据在main()函数中处理
	//// 这里只是简单地切换缓存
	uRxBufIndex++;
	uRxBufIndex &= 0x01;
	USBD_CDC_SetRxBuffer(&USBD_Device, &UserRxBuffer[uRxBufIndex][0]);

	//// 复位OUT端点接收缓冲区
	USBD_CDC_ReceivePacket(&USBD_Device);
	return (USBD_OK);
}

/**
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
/*
static void Error_Handler(void)
{
	/ * Add your own code here * /
}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
