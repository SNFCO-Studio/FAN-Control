#include "u8g2.h"
#include "stm32f0xx_hal.h"

//extern SPI_HandleTypeDef hspi1;

extern I2C_HandleTypeDef hi2c1;


//uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
//                               void *arg_ptr)
//{
//  switch (msg)
//  {
//    case U8X8_MSG_BYTE_SEND:
//      HAL_SPI_Transmit(&hspi1, (uint8_t *) arg_ptr, arg_int, 10000);
//      break;
//    case U8X8_MSG_BYTE_INIT:
//      break;
//    case U8X8_MSG_BYTE_SET_DC:
//      HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, arg_int);
//      break;
//    case U8X8_MSG_BYTE_START_TRANSFER:
//      break;
//    case U8X8_MSG_BYTE_END_TRANSFER:
//      break;
//    default:
//      return 0;
//  }
//  return 1;
//}

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32];		/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
  static uint8_t buf_idx;
  uint8_t *data;

  switch(msg){

    case U8X8_MSG_BYTE_SEND:
      data = (uint8_t *)arg_ptr;
      while( arg_int > 0 ){
        buffer[buf_idx++] = *data;
        data++;
        arg_int--;
      }
      break;

    case U8X8_MSG_BYTE_INIT:
      /* add your custom code to init i2c subsystem */
      break;

    case U8X8_MSG_BYTE_START_TRANSFER:
      buf_idx = 0;
      break;

    case U8X8_MSG_BYTE_END_TRANSFER:
      HAL_I2C_Master_Transmit(&hi2c1 ,u8x8_GetI2CAddress(u8x8), buffer, buf_idx,1000);
      break;

    default:
      return 0;
  }
  return 1;
}


uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
                                  U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
                                  U8X8_UNUSED void *arg_ptr)
{
  switch(msg){

    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      break;

    case U8X8_MSG_DELAY_MILLI:
      HAL_Delay(arg_int);
      break;

    case U8X8_MSG_GPIO_I2C_CLOCK:
      break;

    case U8X8_MSG_GPIO_I2C_DATA:
      break;

    default:
      return 0;
  }
  return 1; // command processed successfully.

}

