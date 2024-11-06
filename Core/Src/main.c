/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "u8g2/u8g2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
static u8g2_t u8g2;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

const char *data[] = {
    "000", "001", "002", "003", "004", "005", "006", "007", "008", "009",
    "010", "011", "012", "013", "014", "015", "016", "017", "018", "019",
    "020", "021", "022", "023", "024", "025", "026", "027", "028", "029",
    "030", "031", "032", "033", "034", "035", "036", "037", "038", "039",
    "040", "041", "042", "043", "044", "045", "046", "047", "048", "049",
    "050", "051", "052", "053", "054", "055", "056", "057", "058", "059",
    "060", "061", "062", "063", "064", "065", "066", "067", "068", "069",
    "070", "071", "072", "073", "074", "075", "076", "077", "078", "079",
    "080", "081", "082", "083", "084", "085", "086", "087", "088", "089",
    "090", "091", "092", "093", "094", "095", "096", "097", "098", "099",
    "100"
};

const char *Rdata[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

int pos = 0;
int posValue[128] = {};

uint32_t Fan1_RValue = 0;
int Fan1_RData[5];
int Fan1_Speed = 0;
int Fan2_Speed = 0;
int Fan_number = 1;
int Before_Speed = 0;
int After_Speed = 0;

int long_SW = 320000;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void drawCurve(u8g2_t u8g2, int value) {
//  u8g2_DrawCircle(&u8g2,15,45,1,U8G2_DRAW_ALL);
  if (pos > 128) {
    pos = 15;
  } else if (pos < 15) {
    pos = 15;
  }
  pos = pos + 1;
  posValue[pos - 15] = value;
  u8g2_DrawVLine(&u8g2, pos, 0, 46);
  for (int i = 0; i < 115; i++) {
    int y1 = 46 - ((posValue[i] / 2));
    int y2 = 46 - ((posValue[i] / 2));
    if (y1 <= 0) {
      y1 = 0;
    } else if (y1 >= 46) {
      y1 = 46;
    }
    if (y2 <= 0) {
      y2 = 0;
    } else if (y2 >= 46) {
      y2 = 46;
    }
    u8g2_DrawLine(&u8g2, i + 15, y1, i + 15, y2);
//    u8g2_DrawPixel(&u8g2, i + 15, y1);
  }

}


void drawUI(u8g2_t u8g2) {
  u8g2_SetFontDirection(&u8g2, 3);
  u8g2_DrawStr(&u8g2, 14, 24, "Fan1");
  u8g2_DrawHLine(&u8g2, 3, 0, 13);

  u8g2_DrawStr(&u8g2, 14, 61, "Fan2");

  u8g2_DrawVLine(&u8g2, 15, 0, 64);

  if (Fan_number == 1) {
    u8g2_DrawLine(&u8g2, 14, 35, 3, 24);
    u8g2_DrawLine(&u8g2, 10, 32, 3, 39);
  } else if (Fan_number == 2) {
    u8g2_DrawLine(&u8g2, 10, 31, 3, 24);
    u8g2_DrawLine(&u8g2, 14, 28, 3, 39);
  }

  u8g2_DrawVLine(&u8g2, 3, 0, 24);
  // u8g2_DrawLine(&u8g2,14,35,3,24);
//  u8g2_DrawLine(&u8g2, 10, 31, 3, 24);

  u8g2_DrawVLine(&u8g2, 3, 39, 24);
  // u8g2_DrawLine(&u8g2, 14, 28, 3, 39);
//  u8g2_DrawLine(&u8g2,10,32,3,39);


  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 22; j++) {
      u8g2_DrawPixel(&u8g2, 20 + (5 * j), (5 * i) + 1);
    }
  }

  u8g2_DrawHLine(&u8g2, 15, 46, 113);
  u8g2_SetFontDirection(&u8g2, 0);
  if (Fan1_Speed == 0 && Fan_number == 1){
    u8g2_DrawStr(&u8g2, 19, 59, "OFF");
  } else if (Fan1_Speed != 0 && Fan_number == 1){
    u8g2_DrawStr(&u8g2, 22, 59, "ON");
  }

  if (Fan2_Speed == 0 && Fan_number == 2){
    u8g2_DrawStr(&u8g2, 19, 59, "OFF");
  } else if (Fan2_Speed != 0 && Fan_number == 2){
    u8g2_DrawStr(&u8g2, 22, 59, "ON");
  }

//  u8g2_DrawStr(&u8g2, 22, 59, "ON");
//  u8g2_DrawStr(&u8g2,19,60,"OFF");

  u8g2_DrawVLine(&u8g2, 41, 46, 18);

  u8g2_DrawStr(&u8g2, 46, 59, "S.");
  u8g2_DrawStr(&u8g2, 75, 59, "%");
  //  u8g2_DrawStr(&u8g2, 57, 60, data[0]);
  //  u8g2_DrawStr(&u8g2, 57, 60, "10");
  //  u8g2_DrawStr(&u8g2, 57, 60, "AUTO");

  u8g2_DrawStr(&u8g2, 85, 59, "R.");
  u8g2_DrawStr(&u8g2, 96, 59, Rdata[Fan1_RData[4]]);
  u8g2_DrawStr(&u8g2, 102, 59, Rdata[Fan1_RData[3]]);
  u8g2_DrawStr(&u8g2, 108, 59, Rdata[Fan1_RData[2]]);
  u8g2_DrawStr(&u8g2, 114, 59, Rdata[Fan1_RData[1]]);
  u8g2_DrawStr(&u8g2, 120, 59, Rdata[Fan1_RData[0]]);
  //  u8g2_DrawStr(&u8g2, 96, 60, "-----");
  //  u8g2_DrawStr(&u8g2,97,60,"00000"); // 57
  u8g2_DrawVLine(&u8g2, 127, 46, 18);
  u8g2_DrawHLine(&u8g2, 3, 63, 125);

}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
    Fan1_RValue = Fan1_RValue + HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
    if (Fan1_RValue >= 100000) {
      Fan1_RValue = 99999;
    }
    Fan1_RData[0] = Fan1_RValue % 10;        // 个位
    Fan1_RData[1] = (Fan1_RValue / 10) % 10;  // 十位
    Fan1_RData[2] = (Fan1_RValue / 100) % 10; // 百位
    Fan1_RData[3] = (Fan1_RValue / 1000) % 10; // 千位
    Fan1_RData[4] = (Fan1_RValue / 10000) % 10; // 万位
  }


}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim16) {
    Fan1_RValue = 0;
  }
  if (htim == &htim17) {
    if (Fan1_Speed > 201) {
      Fan1_Speed = 200;
    } else if (Fan1_Speed < 0) {
      Fan1_Speed = 0;
    }
    if (Fan2_Speed > 201) {
      Fan2_Speed = 200;
    } else if (Fan2_Speed < 0) {
      Fan2_Speed = 0;
    }
    u8g2_ClearBuffer(&u8g2);
    // u8g2_DrawLine(&u8g2, 10, 10, 10, long_SW / 100);
    drawUI(u8g2);
    if (Fan_number == 1) {
      drawCurve(u8g2, Fan1_Speed / 2);
      u8g2_DrawStr(&u8g2, 57, 59, data[Fan1_Speed / 2]);
    } else {
      drawCurve(u8g2, Fan2_Speed / 2);
      u8g2_DrawStr(&u8g2, 57, 59, data[Fan2_Speed / 2]);
    }

    u8g2_SendBuffer(&u8g2);
  }
}


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
  MX_ADC_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */

  u8g2Init(&u8g2);

  // HAL_TIM_Base_Start_IT(&htim1);

  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2);

  HAL_TIM_Base_Start(&htim1);

  HAL_TIM_Base_Start_IT(&htim16);
  HAL_TIM_Base_Start_IT(&htim17);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4); // fan2 pwm output
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // fan1 pwm output
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);

//  HAL_TIM_PWM_Start_IT(&htim16, TIM_CHANNEL_1);

  //  fan1 pwm input
  //  HAL_TIM_IC_Start(&htim1, TIM_CHANNEL_3);
  //  HAL_TIM_IC_Start(&htim1, TIM_CHANNEL_4);
  //
  //  fan2 pwm input
  //  HAL_TIM_IC_Start(&htim14, TIM_CHANNEL_1);



  //  u8g2_SetDisplayRotation(&u8g2, U8G2_R1);
  u8g2_SetFont(&u8g2, u8g2_font_t0_11_tr);
  //  u8g2_DrawBox(&u8g2, 0, 0, 10, 10);

  drawUI(u8g2);

  u8g2_SendBuffer(&u8g2);

  //  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 10);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {

    if (Before_Speed - __HAL_TIM_GET_COUNTER(&htim3) > 0) {
      After_Speed = Before_Speed - __HAL_TIM_GET_COUNTER(&htim3);
    } else if (Before_Speed - __HAL_TIM_GET_COUNTER(&htim3) < 0) {
      After_Speed = Before_Speed - __HAL_TIM_GET_COUNTER(&htim3);
    } else {
      After_Speed = 0;
    }

    if (Fan_number == 1) {
      if (Fan1_Speed > 202) {
        Fan1_Speed = 202;
      } else if (Fan1_Speed < 0) {
        Fan1_Speed = 0;
      }
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, Fan1_Speed / 2);
      Fan1_Speed = Fan1_Speed + After_Speed;
    } else {
      if (Fan2_Speed > 202) {
        Fan2_Speed = 202;
      } else if (Fan2_Speed < 0) {
        Fan2_Speed = 0;
      }
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, Fan2_Speed / 2);
      Fan2_Speed = Fan2_Speed + After_Speed;
    }

    Before_Speed = __HAL_TIM_GET_COUNTER(&htim3);

    if (HAL_GPIO_ReadPin(DECODE_SW_GPIO_Port, DECODE_SW_Pin) == GPIO_PIN_RESET) {
      if (Fan_number == 1) {
        Fan_number = 2;
      } else {
        Fan_number = 1;
      }
    }
    while (HAL_GPIO_ReadPin(DECODE_SW_GPIO_Port, DECODE_SW_Pin) == GPIO_PIN_RESET);

    HAL_Delay(100);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
