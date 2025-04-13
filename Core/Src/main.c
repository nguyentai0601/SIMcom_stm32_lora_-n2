/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"stdio.h"
#include"string.h"
#include"LoRa.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MQTT_BROKER "thingsboard.cloud"
#define MQTT_PORT 1883
#define client_ID "t6xqg0v96t1p9wtc9l2l"// Id của divices Thingsboard
#define User_name "13f8dni4utb650o7aduh"//User_name của divices Thingsboard
#define Pass "c0cu7lr7fbntrqkfok6k"// Pass của divices Thingsboard
char  Topic[50]= "v1/devices/me/telemetry";
uint16_t count=0;
uint8_t data[]="hello";
char dataID[100];
uint8_t dataRxloRa;
uint8_t dataSent[]="hello";
char data_json[50];
uint8_t temperature=35;
uint8_t humidity=65;
osStatus_t status1;
osStatus_t status2;
LoRa myLoRa;
uint8_t Lora_status;
uint8_t ID= '1';
typedef struct {
	uint16_t data;
	uint16_t ID;
}my_Queuedata_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* Definitions for DefaultTask */
osThreadId_t DefaultTaskHandle;
const osThreadAttr_t DefaultTask_attributes = {
  .name = "DefaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LowTask02 */
osThreadId_t LowTask02Handle;
const osThreadAttr_t LowTask02_attributes = {
  .name = "LowTask02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for HighTask03 */
osThreadId_t HighTask03Handle;
const osThreadAttr_t HighTask03_attributes = {
  .name = "HighTask03",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal3,
};
/* Definitions for interruptTask */
osThreadId_t interruptTaskHandle;
const osThreadAttr_t interruptTask_attributes = {
  .name = "interruptTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};
/* Definitions for myQueue02 */
osMessageQueueId_t myQueue02Handle;
const osMessageQueueAttr_t myQueue02_attributes = {
  .name = "myQueue02"
};
/* Definitions for myBinarySeminterrup */
osSemaphoreId_t myBinarySeminterrupHandle;
const osSemaphoreAttr_t myBinarySeminterrup_attributes = {
  .name = "myBinarySeminterrup"
};
/* Definitions for myCountingSem01 */
osSemaphoreId_t myCountingSem01Handle;
const osSemaphoreAttr_t myCountingSem01_attributes = {
  .name = "myCountingSem01"
};
/* Definitions for myCountingSem02 */
osSemaphoreId_t myCountingSem02Handle;
const osSemaphoreAttr_t myCountingSem02_attributes = {
  .name = "myCountingSem02"
};
/* USER CODE BEGIN PV */
int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/*
void Task_action(char message){
	ITM_sendchar(message);
	ITM_sendchar('\n');
}
*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);
void StartinterruptTask(void *argument);
void sendATCommand(const char *cmd, uint32_t delay_ms);
void connectToGPRS();
void connectToMQTT();

void disconnectMQTT();
/* USER CODE BEGIN PFP */
//void process(){
//	LoRa_receive(&myLoRa,&dataRxloRa,sizeof(dataRxloRa));
//	if (dataRxloRa==ID){
//		LoRa_transmit(&myLoRa,dataSent,sizeof(dataSent),200);
//		printf("Lora_transmited");
//	}printf("Lora not receive");
//}
/* Gửi lệnh AT qua UART */
void sendATCommand(const char *cmd, uint32_t delay_ms) {
    HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100);
    HAL_Delay(delay_ms);

}
/* 1 Kết nối GPRS */
void connectToGPRS() {
    sendATCommand("AT+CREG=1\r\n", 1000);
}
void connectToMQTT() {
    sendATCommand("AT+CMQTTSTART\r\n", 2000);
    sprintf(dataID,"AT+CMQTTACCQ=0,\"%s\",0\r\n",client_ID);
    sendATCommand(dataID, 1000);
    char mqtt_conn_cmd[128];
    sprintf(mqtt_conn_cmd, "AT+CMQTTCONNECT=0,\"tcp://%s:%d\",60,1,\"%s\",\"%s\"\r\n", MQTT_BROKER, MQTT_PORT,User_name,Pass );
    sendATCommand(mqtt_conn_cmd, 5000);
}
void publishToMQTT() {
	char Topic_t[100];
	char Payload[100];
	sprintf(Topic,"v1/devices/me/telemetry%s","\r\n");
	sprintf(Topic_t,"AT+CMQTTTOPIC=0,%d\r\n",strlen(Topic)-2);
	sendATCommand(Topic_t,500);
    sendATCommand(Topic, 500);
    sprintf(data_json,"{\"temperature\":%d,\"humidity\":%d}\r\n",temperature,humidity);
    sprintf(Payload,"AT+CMQTTPAYLOAD=0,%d\r\n",strlen(data_json)-2);
    sendATCommand(Payload,500);
    sendATCommand(data_json,1000);
    sendATCommand("AT+CMQTTPUB=0,1,60\r\n", 1000);
}
/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  myLoRa = newLoRa();

  	myLoRa.frequency             = 434;             // default = 433 MHz
  	myLoRa.spredingFactor        = SF_9;            // default = SF_7
  	myLoRa.bandWidth             = BW_250KHz;       // default = BW_125KHz
  	myLoRa.crcRate               = CR_4_8;          // default = CR_4_5
  	myLoRa.power                 = POWER_17db;      // default = 20db
  	myLoRa.overCurrentProtection = 130;             // default = 100 mA
  	myLoRa.preamble              = 10;              // default = 8;

  	myLoRa.CS_port         = NSS_GPIO_Port;
  	myLoRa.CS_pin          = NSS_Pin;
  	myLoRa.reset_port      = RST_GPIO_Port;
  	myLoRa.reset_pin       = RST_Pin;
  	myLoRa.DIO0_port       = DIO0_GPIO_Port;
  	myLoRa.DIO0_pin        = DIO0_Pin;
  	myLoRa.hSPIx           = &hspi1;

  	Lora_status = LoRa_init(&myLoRa);

  	LoRa_startReceiving(&myLoRa);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of myBinarySeminterrup */
  myBinarySeminterrupHandle = osSemaphoreNew(1, 0, &myBinarySeminterrup_attributes);

  /* creation of myCountingSem01 */
  myCountingSem01Handle = osSemaphoreNew(3, 0, &myCountingSem01_attributes);

  /* creation of myCountingSem02 */
  myCountingSem02Handle = osSemaphoreNew(3, 0, &myCountingSem02_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &myQueue01_attributes);

  /* creation of myQueue02 */
  myQueue02Handle = osMessageQueueNew (16, sizeof(my_Queuedata_t), &myQueue02_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DefaultTask */
  DefaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &DefaultTask_attributes);

  /* creation of LowTask02 */
  LowTask02Handle = osThreadNew(StartTask02, NULL, &LowTask02_attributes);

  /* creation of HighTask03 */
  HighTask03Handle = osThreadNew(StartTask03, NULL, &HighTask03_attributes);

  /* creation of interruptTask */
  interruptTaskHandle = osThreadNew(StartinterruptTask, NULL, &interruptTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, NSS_Pin|RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : NSS_Pin RST_Pin */
  GPIO_InitStruct.Pin = NSS_Pin|RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : button_Pin */
  GPIO_InitStruct.Pin = button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DIO0_Pin */
  GPIO_InitStruct.Pin = DIO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DIO0_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the DefaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */

  /* Infinite loop */
  for(;;)
  {
	  publishToMQTT();
	  osDelay(1000);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the LowTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
 /*my_Queuedata_t msg={
		 .ID=234
 };
*/
  for(;;)
  {
	  LoRa_transmit(&myLoRa,dataSent,sizeof(dataSent),100);
	 // osSemaphoreAcquire(myBinarySem01Handle,osWaitForever);

	/*	printf("@%lu: %s Enter!\n",osKernelGetTickCount(),LowTask02_attributes.name);
	  	count2--;
		printf("@%lu:Put ID:[%d],data:[%d] to %s\n",osKernelGetTickCount(),msg.ID,msg.data,myQueue02_attributes.name);
		osMessageQueuePut(myQueue02Handle,&msg,0,0);
	//	while(HAL_GPIO_ReadPin(button_GPIO_Port,button_Pin)==GPIO_PIN_SET);
		printf("@%lu:%s Exit\n\n",osKernelGetTickCount(),LowTask02_attributes.name);
	//	osSemaphoreRelease(myBinarySem01Handle);
	    osDelay(500);
	    */
	  osDelay(1000);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the HighTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
//my_Queuedata_t msg;
  /* Infinite loop */
  for(;;)
  {

//	  osSemaphoreAcquire(myCountingSem01Handle,osWaitForever);
//	//  count3++;
//	printf("@%lu:%sEnter!\n",osKernelGetTickCount(),HighTask03_attributes.name);
////	process();
// //   osMessageQueueGet(myQueue02Handle,&msg,0,osWaitForever);
// //   printf("@ %lu :%s Get ID:[%d],data:[%d] From %s\n",osKernelGetTickCount(),HighTask03_attributes.name,msg.ID,msg.data,myQueue02_attributes.name);
//    printf("@ %lu :%s Exit!\n\n",osKernelGetTickCount(),HighTask03_attributes.name);
//  //  osSemaphoreRelease(myBinarySem01Handle);
	 osDelay(1000);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartinterruptTask */
/**
* @brief Function implementing the interruptTask thread.
* @param argument: Not used
* @retval None
*/


/* USER CODE END Header_StartinterruptTask */
void StartinterruptTask(void *argument)
{
  /* USER CODE BEGIN StartinterruptTask */
  /* Infinite loop */
	//my_Queuedata_t msg;
  for(;;)
  {
	  /*
	 osSemaphoreAcquire(myBinarySeminterrupHandle,osWaitForever);
	 printf("@%lu:%s Enter!\n",osKernelGetTickCount(),interruptTask_attributes.name);
	 printf("@%lu:%s Exit!\n",osKernelGetTickCount(),interruptTask_attributes.name);

  	  osMessageQueueGet(myQueue02Handle,&msg,0,osWaitForever);
	  printf("@%lu: %s Enter\n",osKernelGetTickCount(),interruptTask_attributes.name);
	  printf("@%lu: %s Get ID:[%d],data:[%d] from %s",osKernelGetTickCount(),interruptTask_attributes.name,msg.ID,msg.data,myQueue02_attributes.name);
	  printf("@%lu: %s Exit!",osKernelGetTickCount(),interruptTask_attributes.name);
  	  osDelay(10000);
	 osSemaphoreRelease(myBinarySeminterrupHandle);
	 */
  }
  /* USER CODE END StartinterruptTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
}

   void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	   if(GPIO_Pin==DIO0_Pin){
		   printf("di vao ngat va giai phong semaphore\n");
		   osSemaphoreRelease(myCountingSem01Handle);

		  // HAL_Delay(500);
	   }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
