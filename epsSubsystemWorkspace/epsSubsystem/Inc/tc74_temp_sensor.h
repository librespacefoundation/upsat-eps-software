#ifndef __TC74_TEMP_SENSOR_H
#define __TC74_TEMP_SENSOR_H

#include "stm32l1xx_hal.h"


#define VDDA_APPLI                     ((uint32_t) 3300)    /* Value of analog voltage supply Vdda (unit: mV) */
#define RANGE_12BITS                   ((uint32_t) 4095)    /* Max digital value with a full range of 12 bits */

/* ADC parameters */
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint32_t)    3)    /* Size of array containing ADC converted values: set to ADC sequencer number of ranks converted, to have a rank in each address */

/* Internal temperature sensor: constants data used for indicative values in  */
/* this example. Refer to device datasheet for min/typ/max values.            */
/* For more accurate values, device should be calibrated on offset and slope  */
/* for application temperature range.                                         */
#define INTERNAL_TEMPSENSOR_V110       ((int32_t)  627)         /* Internal temperature sensor, parameter V110 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_AVGSLOPE   ((int32_t) 1610)         /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */
#define TEMP30_CAL_ADDR   ((uint16_t*) ((uint32_t) 0x1FF800FA)) /* Internal temperature sensor, parameter TS_CAL1: TS ADC raw data acquired at a temperature of 110 DegC (+-5 DegC), VDDA = 3.3 V (+-10 mV). */
#define TEMP110_CAL_ADDR  ((uint16_t*) ((uint32_t) 0x1FF800FE)) /* Internal temperature sensor, parameter TS_CAL2: TS ADC raw data acquired at a temperature of  30 DegC (+-5 DegC), VDDA = 3.3 V (+-10 mV). */
#define VDDA_TEMP_CAL                  ((uint32_t) 3000)        /* Vdda value with which temperature sensor has been calibrated in production (+-10 mV). */

/* Internal voltage reference */
#define VREFINT_CAL       ((uint16_t*) ((uint32_t) 0x1FF800F8)) /* Internal voltage reference, parameter VREFINT_CAL: Raw data acquired at a temperature of 30 DegC (+-5 DegC), VDDA = 3.3 V (+-10 mV). */
                                                                /* This calibration parameter is intended to calculate the actual VDDA from Vrefint ADC measurement. */

/* Private macro -------------------------------------------------------------*/
/**
  * @brief  Computation of temperature (unit: degree Celsius) from the internal
  *         temperature sensor measurement by ADC.
  *         Computation is using temperature sensor calibration values done
  *         in production.
  *         Computation formula:
  *         Temperature = (TS_ADC_DATA - TS_CAL1) * (110degC - 30degC)
  *                       / (TS_CAL2 - TS_CAL1) + 30degC
  *         with TS_ADC_DATA = temperature sensor raw data measured by ADC
  *              Avg_Slope = (TS_CAL2 - TS_CAL1) / (110 - 30)
  *              TS_CAL1 = TS_ADC_DATA @30degC (calibrated in factory)
  *              TS_CAL2 = TS_ADC_DATA @110degC (calibrated in factory)
  *         Calculation validity conditioned to settings:
  *          - ADC resolution 12 bits (need to scale conversion value
  *            if using a different resolution).
  *          - Power supply of analog voltage set to literal VDDA_APPLI
  *            (need to scale value if using a different value of analog
  *            voltage supply).
  * @param TS_ADC_DATA: Temperature sensor digital value measured by ADC
  * @retval None
  */
#define COMPUTATION_TEMPERATURE_TEMP30_TEMP110(TS_ADC_DATA)                    \
  (((( ((int32_t)((TS_ADC_DATA * VDDA_APPLI) / VDDA_TEMP_CAL)                  \
        - (int32_t) *TEMP30_CAL_ADDR)                                          \
     ) * (int32_t)(110 - 30)                                                   \
    ) / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR)                        \
   ) + 30                                                                      \
  )



// tc74 register addresses
#define CONFIGURATION_REGISTER	0x01
#define TEMPERATURE_REGISTER	0x00

// configuration register commands
#define STANDBY 	0b10000000
#define NORMAL 		0b00000000

// tc74 i2c address constants
#define TC74_A0	0b10010000
#define TC74_A1	0b10010010
#define TC74_A2	0b10010100
#define TC74_A3	0b10010110
#define TC74_A4	0b10011000
#define TC74_A5	0b10011010
#define TC74_A6	0b10011100
#define TC74_A7	0b10011110


typedef enum TC_74_STATUS{ DEVICE_ERROR, DEVICE_STANDBY, DEVICE_NORMAL } TC_74_STATUS;

/*get temperature reading from the tc74 sensor with the specified device address - returns status*/
TC_74_STATUS read_device_temperature(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address, uint8_t *receive_word );
/*wake up sensor*/
TC_74_STATUS device_wake_up(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address);
/*put sensor to sleep*/
TC_74_STATUS device_sleep(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address);
/*get sensor status*/
TC_74_STATUS read_device_status(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address);


#endif
