
/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware Includes */
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysexc.h" //System Exception
#include "driverlib/systick.h" //SysTick
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h" //NIVC
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "driverlib/i2c.h"

//-----------------
#include "drivers/rtos_hw_drivers.h"
#include "drivers/i2c_master.h"

#include "APDS9960.h"

/* Members */
gesture_data_type gesture_data_;
int gesture_ud_delta_;
int gesture_lr_delta_;
int gesture_ud_count_;
int gesture_lr_count_;
int gesture_near_count_;
int gesture_far_count_;
int gesture_state_;
int gesture_motion_;

volatile uint8_t fifo_data[128];

//Static
/* Gesture processing */
void resetGestureParameters(void);
bool processGestureData(void);
bool decodeGesture(void);

/* Proximity Interrupt Threshold */
#define getProxIntLowThresh() (I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PILT))
bool setProxIntLowThresh(uint8_t threshold);
#define getProxIntHighThresh() (I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PIHT))
bool setProxIntHighThresh(uint8_t threshold);

/* LED Boost Control */
uint8_t getLEDBoost(void);
bool setLEDBoost(uint8_t boost);

/* Proximity photodiode select */
uint8_t getProxGainCompEnable(void);
bool setProxGainCompEnable(uint8_t enable);
uint8_t getProxPhotoMask(void);
bool setProxPhotoMask(uint8_t mask);

/* Gesture threshold control */
uint8_t getGestureEnterThresh(void);
bool setGestureEnterThresh(uint8_t threshold);
#define getGestureExitThresh() (I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GEXTH))
bool setGestureExitThresh(uint8_t threshold);

/* Gesture LED, gain, and time control */
uint8_t getGestureWaitTime(void);
bool setGestureWaitTime(uint8_t time);

/* Gesture mode */
#define getGestureMode() (I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF4) & 0b00000001)
bool setGestureMode(uint8_t mode);

static uint32_t i2cBase;

bool APDS9960Init(uint32_t ui32Base)
{
    uint8_t id;

    /* Initialize I2C */

    i2cBase = ui32Base;

    /* Read ID register and check against known values for APDS-9960 */
    id = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR, APDS9960_ID);

    if( !(id == APDS9960_ID_1 || id == APDS9960_ID_2) ) {
        return false;
    }

    /* Set ENABLE register to 0 (disable all features) */
    if( !setMode(ALL, OFF) ) {
        return false;
    }

    /* Set default values for ambient light and proximity registers */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ATIME,DEFAULT_ATIME);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_WTIME, DEFAULT_WTIME);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PPULSE, DEFAULT_PROX_PPULSE);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_POFFSET_UR, DEFAULT_POFFSET_UR);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_POFFSET_DL, DEFAULT_POFFSET_DL);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG1, DEFAULT_CONFIG1);

    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return false;
    }
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return false;
    }
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return false;
    }
    if( !setProxIntLowThresh(DEFAULT_PILT) ) {
        return false;
    }
    if( !setProxIntHighThresh(DEFAULT_PIHT) ) {
        return false;
    }
    if( !setLightIntLowThreshold(DEFAULT_AILT) ) {
        return false;
    }
    if( !setLightIntHighThreshold(DEFAULT_AIHT) ) {
        return false;
    }

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PERS, DEFAULT_PERS);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG2, DEFAULT_CONFIG2);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG3, DEFAULT_CONFIG3);

    /* Set default values for gesture sense registers */
    if( !setGestureEnterThresh(DEFAULT_GPENTH) ) {
        return false;
    }
    if( !setGestureExitThresh(DEFAULT_GEXTH) ) {
        return false;
    }

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF1, DEFAULT_GCONF1);

    if( !setGestureGain(DEFAULT_GGAIN) ) {
        return false;
    }
    if( !setGestureLEDDrive(DEFAULT_GLDRIVE) ) {
        return false;
    }
    if( !setGestureWaitTime(DEFAULT_GWTIME) ) {
        return false;
    }

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GOFFSET_U, DEFAULT_GOFFSET);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GOFFSET_D, DEFAULT_GOFFSET);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GOFFSET_L, DEFAULT_GOFFSET);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GOFFSET_R, DEFAULT_GOFFSET);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GPULSE, DEFAULT_GPULSE);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF3, DEFAULT_GCONF3);

    if( !setGestureIntEnable(DEFAULT_GIEN) ) {
        return false;
    }

    return true;
}

/**
 * @brief Reads and returns the contents of the STATUS register
 *
 * @return Contents of the STATUS register. 0xFF if error.
 */
uint8_t getStatusRegister(void)
{
    uint8_t status_value;

    /* Read current STATUS register */
    status_value = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_STATUS);

    return status_value;
}

/**
 * @brief Reads and returns the contents of the ENABLE register
 *
 * @return Contents of the ENABLE register. 0xFF if error.
 */
//uint8_t getMode()
//{
//    uint8_t enable_value;
//
//    /* Read current ENABLE register */
//    enable_value = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE);
//
//    return enable_value;
//}

/**
 * @brief Enables or disables a feature in the APDS-9960
 *
 * @param[in] mode which feature to enable
 * @param[in] enable ON (1) or OFF (0)
 * @return True if operation success. False otherwise.
 */
bool setMode(uint8_t mode, uint8_t enable)
{
    uint8_t reg_val;

    /* Read current ENABLE register */
    reg_val = getMode();
    if( reg_val == ERROR ) {
        return false;
    }

    /* Change bit(s) in ENABLE register */
    enable = enable & 0x01;
    if( mode >= 0 && mode <= 6 ) {
        if (enable) {
            reg_val |= (1 << mode);
        } else {
            reg_val &= ~(1 << mode);
        }
    } else if( mode == ALL ) {
        if (enable) {
            reg_val = 0x7F;
        } else {
            reg_val = 0x00;
        }
    }

    /* Write value back to ENABLE register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE, reg_val);

    return true;
}

/**
 * @brief Starts the light (R/G/B/Ambient) sensor on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware interrupt on high or low light
 * @return True if sensor enabled correctly. False on error.
 */
bool enableLightSensor(bool interrupts)
{

    /* Set default gain, interrupts, enable power, and enable sensor */
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return false;
    }
    if( interrupts ) {
        if( !setAmbientLightIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setAmbientLightIntEnable(0) ) {
            return false;
        }
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(AMBIENT_LIGHT, 1) ) {
        return false;
    }

    return true;

}

/**
 * @brief Ends the light sensor on the APDS-9960
 *
 * @return True if sensor disabled correctly. False on error.
 */
bool disableLightSensor(void)
{
    if( !setAmbientLightIntEnable(0) ) {
        return false;
    }
    if( !setMode(AMBIENT_LIGHT, 0) ) {
        return false;
    }

    return true;
}

/**
 * @brief Starts the proximity sensor on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware external interrupt on proximity
 * @return True if sensor enabled correctly. False on error.
 */
bool enableProximitySensor(bool interrupts)
{
    /* Set default gain, LED, interrupts, enable power, and enable sensor */
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return false;
    }
    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return false;
    }
    if( interrupts ) {
        if( !setProximityIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setProximityIntEnable(0) ) {
            return false;
        }
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return false;
    }

    return true;
}

/**
 * @brief Ends the proximity sensor on the APDS-9960
 *
 * @return True if sensor disabled correctly. False on error.
 */
bool disableProximitySensor(void)
{
    if( !setProximityIntEnable(0) ) {
        return false;
    }
    if( !setMode(PROXIMITY, 0) ) {
        return false;
    }

    return true;
}

/**
 * @brief Starts the gesture recognition engine on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware external interrupt on gesture
 * @return True if engine enabled correctly. False on error.
 */
bool enableGestureSensor(bool interrupts)
{

    /* Enable gesture mode
       Set ENABLE to 0 (power off)
       Set WTIME to 0xFF
       Set AUX to LED_BOOST_300
       Enable PON, WEN, PEN, GEN in ENABLE
    */
    resetGestureParameters();

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_WTIME, 0xFF);

    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PPULSE, DEFAULT_GESTURE_PPULSE);

    if( !setLEDBoost(LED_BOOST_100) ) { //LED_BOOST_300
        return false;
    }
    if( interrupts ) {
        if( !setGestureIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setGestureIntEnable(0) ) {
            return false;
        }
    }
    if( !setGestureMode(1) ) {
        return false;
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(WAIT, 1) ) {
        return false;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return false;
    }
    if( !setMode(GESTURE, 1) ) {
        return false;
    }

    return true;
}

/**
 * @brief Ends the gesture recognition engine on the APDS-9960
 *
 * @return True if engine disabled correctly. False on error.
 */
bool disableGestureSensor(void)
{
    resetGestureParameters();
    if( !setGestureIntEnable(0) ) {
        return false;
    }
    if( !setGestureMode(0) ) {
        return false;
    }
    if( !setMode(GESTURE, 0) ) {
        return false;
    }

    return true;
}


/**
 * @brief Processes a gesture event and returns best guessed gesture
 *
 * @return Number corresponding to gesture. -1 on error.
 */

int readGesture(void)
{
    uint8_t fifo_level = 0;
    uint8_t bytes_read = 0;
    //uint8_t fifo_data[128];
    uint8_t gstatus;
    int motion;
    int i;

    /* Make sure that power and gesture is on and data is valid */
    if( !isGestureAvailable() || !(getMode() & 0b01000001) ) {
        return DIR_NONE;
    }

    /* Keep looping as long as gesture data is valid */
    while(1) {
        //CLRWDT(); //TODO: it should not be here
        /* Wait some time to collect next batch of FIFO data */
        //__delay_ms(FIFO_PAUSE_TIME); //TODO:

        /* Get the contents of the STATUS register. Is data still valid? */
        gstatus = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GSTATUS);

        /* If we have valid data, read in FIFO */
        if( (gstatus & APDS9960_GVALID) == APDS9960_GVALID ) {

            /* Read the current FIFO level */
            fifo_level = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GFLVL);

#if DEBUG
            Serial.print("FIFO Level: ");
            Serial.println(fifo_level);
#endif

            /* If there's stuff in the FIFO, read it into our data block */
            if( fifo_level > 0) {

//                i2c_readDataBlock(APDS9960_I2C_ADDR,APDS9960_GFIFO_U,
//                                                (uint8_t*)fifo_data,
//                                                (fifo_level * 4));

                I2C_Master_ReadBytes(i2cBase,APDS9960_I2C_ADDR,APDS9960_GFIFO_U,
                                                                (uint8_t*)fifo_data,
                                                                (fifo_level * 4));

                bytes_read = (fifo_level * 4);// NOTE: must be >= 4; we assume all bytes are read successfully

//                bytes_read = wireReadDataBlock(  APDS9960_GFIFO_U,
//                                                (uint8_t*)fifo_data,
//                                                (fifo_level * 4) );
//                if( bytes_read == -1 ) {
//                    return ERROR;
//                }
#if DEBUG
                Serial.print("FIFO Dump: ");
                for ( i = 0; i < bytes_read; i++ ) {
                    Serial.print(fifo_data[i]);
                    Serial.print(" ");
                }
                Serial.println();
#endif

                /* If at least 1 set of data, sort the data into U/D/L/R */
                if( bytes_read >= 4 ) {
                    for( i = 0; i < bytes_read; i += 4 ) {
                        gesture_data_.u_data[gesture_data_.index] = \
                                                            fifo_data[i + 0];
                        gesture_data_.d_data[gesture_data_.index] = \
                                                            fifo_data[i + 1];
                        gesture_data_.l_data[gesture_data_.index] = \
                                                            fifo_data[i + 2];
                        gesture_data_.r_data[gesture_data_.index] = \
                                                            fifo_data[i + 3];
                        gesture_data_.index++;
                        gesture_data_.total_gestures++;
                    }

#if DEBUG
                Serial.print("Up Data: ");
                for ( i = 0; i < gesture_data_.total_gestures; i++ ) {
                    Serial.print(gesture_data_.u_data[i]);
                    Serial.print(" ");
                }
                Serial.println();
#endif

                    /* Filter and process gesture data. Decode near/far state */
                    if( processGestureData() ) {
                        if( decodeGesture() ) {
                            //***TODO: U-Turn Gestures
#if DEBUG
                            //Serial.println(gesture_motion_);
#endif
                        }
                    }

                    /* Reset data */
                    gesture_data_.index = 0;
                    gesture_data_.total_gestures = 0;
                }
            }
        } else {

            /* Determine best guessed gesture and clean up */
            //__delay_ms(FIFO_PAUSE_TIME); //TODO:
            decodeGesture();
            motion = gesture_motion_;
#if DEBUG
            Serial.print("END: ");
            Serial.println(gesture_motion_);
#endif
            resetGestureParameters();
            return motion;
        }
    }
}

/*******************************************************************************
 * Ambient light and color sensor controls
 ******************************************************************************/

/**
 * @brief Reads the ambient (clear) light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
uint16_t readAmbientLight(void)
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CDATAL);
    val = val_byte;

    /* Read value from clear channel, high byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CDATAH);
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
uint16_t readRedLight(void)
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_RDATAL);
    val = val_byte;

    /* Read value from clear channel, high byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_RDATAH);
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

/**
 * @brief Reads the green light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
uint16_t readGreenLight(void)
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GDATAL);
    val = val_byte;

    /* Read value from clear channel, high byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GDATAH);
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
uint16_t readBlueLight(void)
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_BDATAL);
    val = val_byte;

    /* Read value from clear channel, high byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_BDATAH);
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

/*******************************************************************************
 * High-level gesture controls
 ******************************************************************************/

/**
 * @brief Resets all the parameters in the gesture data member
 */
void resetGestureParameters(void)
{
    gesture_data_.index = 0;
    gesture_data_.total_gestures = 0;

    gesture_ud_delta_ = 0;
    gesture_lr_delta_ = 0;

    gesture_ud_count_ = 0;
    gesture_lr_count_ = 0;

    gesture_near_count_ = 0;
    gesture_far_count_ = 0;

    gesture_state_ = 0;
    gesture_motion_ = DIR_NONE;
}

/**
 * @brief Processes the raw gesture data to determine swipe direction
 *
 * @return True if near or far state seen. False otherwise.
 */
bool processGestureData(void)
{
    uint8_t u_first = 0;
    uint8_t d_first = 0;
    uint8_t l_first = 0;
    uint8_t r_first = 0;
    uint8_t u_last = 0;
    uint8_t d_last = 0;
    uint8_t l_last = 0;
    uint8_t r_last = 0;
    int ud_ratio_first;
    int lr_ratio_first;
    int ud_ratio_last;
    int lr_ratio_last;
    int ud_delta;
    int lr_delta;
    int i;

    /* If we have less than 4 total gestures, that's not enough */
    if( gesture_data_.total_gestures <= 4 ) {
        return false;
    }

    /* Check to make sure our data isn't out of bounds */
    if( (gesture_data_.total_gestures <= 32) && \
        (gesture_data_.total_gestures > 0) ) {

        /* Find the first value in U/D/L/R above the threshold */
        for( i = 0; i < gesture_data_.total_gestures; i++ ) {
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {

                u_first = gesture_data_.u_data[i];
                d_first = gesture_data_.d_data[i];
                l_first = gesture_data_.l_data[i];
                r_first = gesture_data_.r_data[i];
                break;
            }
        }

        /* If one of the _first values is 0, then there is no good data */
        if( (u_first == 0) || (d_first == 0) || \
            (l_first == 0) || (r_first == 0) ) {

            return false;
        }
        /* Find the last value in U/D/L/R above the threshold */
        for( i = gesture_data_.total_gestures - 1; i >= 0; i-- ) {
#if DEBUG
            Serial.print(F("Finding last: "));
            Serial.print(F("U:"));
            Serial.print(gesture_data_.u_data[i]);
            Serial.print(F(" D:"));
            Serial.print(gesture_data_.d_data[i]);
            Serial.print(F(" L:"));
            Serial.print(gesture_data_.l_data[i]);
            Serial.print(F(" R:"));
            Serial.println(gesture_data_.r_data[i]);
#endif
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {

                u_last = gesture_data_.u_data[i];
                d_last = gesture_data_.d_data[i];
                l_last = gesture_data_.l_data[i];
                r_last = gesture_data_.r_data[i];
                break;
            }
        }
    }

    /* Calculate the first vs. last ratio of up/down and left/right */
    ud_ratio_first = ((u_first - d_first) * 100) / (u_first + d_first);
    lr_ratio_first = ((l_first - r_first) * 100) / (l_first + r_first);
    ud_ratio_last = ((u_last - d_last) * 100) / (u_last + d_last);
    lr_ratio_last = ((l_last - r_last) * 100) / (l_last + r_last);

#if DEBUG
    Serial.print(F("Last Values: "));
    Serial.print(F("U:"));
    Serial.print(u_last);
    Serial.print(F(" D:"));
    Serial.print(d_last);
    Serial.print(F(" L:"));
    Serial.print(l_last);
    Serial.print(F(" R:"));
    Serial.println(r_last);

    Serial.print(F("Ratios: "));
    Serial.print(F("UD Fi: "));
    Serial.print(ud_ratio_first);
    Serial.print(F(" UD La: "));
    Serial.print(ud_ratio_last);
    Serial.print(F(" LR Fi: "));
    Serial.print(lr_ratio_first);
    Serial.print(F(" LR La: "));
    Serial.println(lr_ratio_last);
#endif

    /* Determine the difference between the first and last ratios */
    ud_delta = ud_ratio_last - ud_ratio_first;
    lr_delta = lr_ratio_last - lr_ratio_first;

#if DEBUG
    Serial.print("Deltas: ");
    Serial.print("UD: ");
    Serial.print(ud_delta);
    Serial.print(" LR: ");
    Serial.println(lr_delta);
#endif

    /* Accumulate the UD and LR delta values */
    gesture_ud_delta_ += ud_delta;
    gesture_lr_delta_ += lr_delta;

#if DEBUG
    Serial.print("Accumulations: ");
    Serial.print("UD: ");
    Serial.print(gesture_ud_delta_);
    Serial.print(" LR: ");
    Serial.println(gesture_lr_delta_);
#endif

    /* Determine U/D gesture */
    if( gesture_ud_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = 1;
    } else if( gesture_ud_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = -1;
    } else {
        gesture_ud_count_ = 0;
    }

    /* Determine L/R gesture */
    if( gesture_lr_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = 1;
    } else if( gesture_lr_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = -1;
    } else {
        gesture_lr_count_ = 0;
    }

    /* Determine Near/Far gesture */
    if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 0) ) {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {

            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            } else if( (ud_delta != 0) || (lr_delta != 0) ) {
                gesture_far_count_++;
            }

            if( (gesture_near_count_ >= 10) && (gesture_far_count_ >= 2) ) {
                if( (ud_delta == 0) && (lr_delta == 0) ) {
                    gesture_state_ = NEAR_STATE;
                } else if( (ud_delta != 0) && (lr_delta != 0) ) {
                    gesture_state_ = FAR_STATE;
                }
                return true;
            }
        }
    } else {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {

            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            }

            if( gesture_near_count_ >= 10 ) {
                gesture_ud_count_ = 0;
                gesture_lr_count_ = 0;
                gesture_ud_delta_ = 0;
                gesture_lr_delta_ = 0;
            }
        }
    }

#if DEBUG
    Serial.print("UD_CT: ");
    Serial.print(gesture_ud_count_);
    Serial.print(" LR_CT: ");
    Serial.print(gesture_lr_count_);
    Serial.print(" NEAR_CT: ");
    Serial.print(gesture_near_count_);
    Serial.print(" FAR_CT: ");
    Serial.println(gesture_far_count_);
    Serial.println("----------");
#endif

    return false;
}

/**
 * @brief Determines swipe direction or near/far state
 *
 * @return True if near/far event. False otherwise.
 */
bool decodeGesture(void)
{
    /* Return if near or far event is detected */
    if( gesture_state_ == NEAR_STATE ) {
        gesture_motion_ = DIR_NEAR;
        return true;
    } else if ( gesture_state_ == FAR_STATE ) {
        gesture_motion_ = DIR_FAR;
        return true;
    }

    /* Determine swipe direction */
    if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_UP;
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_DOWN;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 1) ) {
        gesture_motion_ = DIR_RIGHT;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == -1) ) {
        gesture_motion_ = DIR_LEFT;
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else {
        return false;
    }

    return true;
}

/*******************************************************************************
 * Getters and setters for register values
 ******************************************************************************/

/**
 * @brief Sets the lower threshold for proximity detection
 *
 * @param[in] threshold the lower proximity threshold
 * @return True if operation successful. False otherwise.
 */
bool setProxIntLowThresh(uint8_t threshold)
{
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PILT, threshold);

    return true;
}

/**
 * @brief Sets the high threshold for proximity detection
 *
 * @param[in] threshold the high proximity threshold
 * @return True if operation successful. False otherwise.
 */
bool setProxIntHighThresh(uint8_t threshold)
{
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PIHT, threshold);

    return true;
}

/**
 * @brief Returns LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the value of the LED drive strength. 0xFF on failure.
 */
uint8_t getLEDDrive(void)
{
    uint8_t val;

    /* Read value from CONTROL register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL);

    /* Shift and mask out LED drive bits */
    val = (val >> 6) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value (0-3) for the LED drive strength
 * @return True if operation successful. False otherwise.
 */
bool setLEDDrive(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL);

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 6;
    val &= 0b00111111;
    val |= drive;

    /* Write register value back into CONTROL register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL, val);

    return true;
}

/**
 * @brief Returns receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the value of the proximity gain. 0xFF on failure.
 */
uint8_t getProximityGain(void)
{
    uint8_t val;

    /* Read value from CONTROL register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL);

    /* Shift and mask out PDRIVE bits */
    val = (val >> 2) & 0b00000011;

    return val;
}

/**
 * @brief Sets the receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return True if operation successful. False otherwise.
 */
bool setProximityGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL);

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 2;
    val &= 0b11110011;
    val |= drive;

    /* Write register value back into CONTROL register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL, val);

    return true;
}

/**
 * @brief Returns receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @return the value of the ALS gain. 0xFF on failure.
 */
uint8_t getAmbientLightGain(void)
{
    uint8_t val;

    /* Read value from CONTROL register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL);

    /* Shift and mask out ADRIVE bits */
    val &= 0b00000011;

    return val;
}

/**
 * @brief Sets the receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return True if operation successful. False otherwise.
 */
bool setAmbientLightGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL);

    /* Set bits in register to given value */
    drive &= 0b00000011;
    val &= 0b11111100;
    val |= drive;

    /* Write register value back into CONTROL register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONTROL, val);

    return true;
}

/**
 * @brief Get the current LED boost value
 *
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @return The LED boost value. 0xFF on failure.
 */
uint8_t getLEDBoost(void)
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG2);

    /* Shift and mask out LED_BOOST bits */
    val = (val >> 4) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED current boost value
 *
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @param[in] drive the value (0-3) for current boost (100-300%)
 * @return True if operation successful. False otherwise.
 */
bool setLEDBoost(uint8_t boost)
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG2);

    /* Set bits in register to given value */
    boost &= 0b00000011;
    boost = boost << 4;
    val &= 0b11001111;
    val |= boost;

    /* Write register value back into CONFIG2 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG2, val);

    return true;
}

/**
 * @brief Gets proximity gain compensation enable
 *
 * @return 1 if compensation is enabled. 0 if not. 0xFF on error.
 */
uint8_t getProxGainCompEnable(void)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG3);

    /* Shift and mask out PCMP bits */
    val = (val >> 5) & 0b00000001;

    return val;
}

/**
 * @brief Sets the proximity gain compensation enable
 *
 * @param[in] enable 1 to enable compensation. 0 to disable compensation.
 * @return True if operation successful. False otherwise.
 */
 bool setProxGainCompEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG3);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into CONFIG3 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG3, val);

    return true;
}

/**
 * @brief Gets the current mask for enabled/disabled proximity photodiodes
 *
 * 1 = disabled, 0 = enabled
 * Bit    Photodiode
 *  3       UP
 *  2       DOWN
 *  1       LEFT
 *  0       RIGHT
 *
 * @return Current proximity mask for photodiodes. 0xFF on error.
 */
uint8_t getProxPhotoMask(void)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG3);

    /* Mask out photodiode enable mask bits */
    val &= 0b00001111;

    return val;
}

/**
 * @brief Sets the mask for enabling/disabling proximity photodiodes
 *
 * 1 = disabled, 0 = enabled
 * Bit    Photodiode
 *  3       UP
 *  2       DOWN
 *  1       LEFT
 *  0       RIGHT
 *
 * @param[in] mask 4-bit mask value
 * @return True if operation successful. False otherwise.
 */

bool setProxPhotoMask(uint8_t mask)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG3);

    /* Set bits in register to given value */
    mask &= 0b00001111;
    val &= 0b11110000;
    val |= mask;

    /* Write register value back into CONFIG3 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_CONFIG3, val);

    return true;
}

/**
 * @brief Gets the entry proximity threshold for gesture sensing
 *
 * @return Current entry proximity threshold.
 */
uint8_t getGestureEnterThresh(void)
{
    uint8_t val = 0;

    /* Read value from GPENTH register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GPENTH);

    return val;
}

/**
 * @brief Sets the entry proximity threshold for gesture sensing
 *
 * @param[in] threshold proximity value needed to start gesture mode
 * @return True if operation successful. False otherwise.
 */
bool setGestureEnterThresh(uint8_t threshold)
{
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GPENTH, threshold);

    return true;
}

/**
 * @brief Sets the exit proximity threshold for gesture sensing
 *
 * @param[in] threshold proximity value needed to end gesture mode
 * @return True if operation successful. False otherwise.
 */
bool setGestureExitThresh(uint8_t threshold)
{
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GEXTH, threshold);

    return true;
}

/**
 * @brief Gets the gain of the photodiode during gesture mode
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the current photodiode gain. 0xFF on error.
 */
uint8_t getGestureGain(void)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2);

    /* Shift and mask out GGAIN bits */
    val = (val >> 5) & 0b00000011;

    return val;
}

/**
 * @brief Sets the gain of the photodiode during gesture mode
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] gain the value for the photodiode gain
 * @return True if operation successful. False otherwise.
 */
bool setGestureGain(uint8_t gain)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2);

    /* Set bits in register to given value */
    gain &= 0b00000011;
    gain = gain << 5;
    val &= 0b10011111;
    val |= gain;

    /* Write register value back into GCONF2 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2, val);

    return true;
}

/**
 * @brief Gets the drive current of the LED during gesture mode
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the LED drive current value. 0xFF on error.
 */
uint8_t getGestureLEDDrive(void)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2);

    /* Shift and mask out GLDRIVE bits */
    val = (val >> 3) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED drive current during gesture mode
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value for the LED drive current
 * @return True if operation successful. False otherwise.
 */
bool setGestureLEDDrive(uint8_t drive)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2);

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 3;
    val &= 0b11100111;
    val |= drive;

    /* Write register value back into GCONF2 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2, val);

    return true;
}

/**
 * @brief Gets the time in low power mode between gesture detections
 *
 * Value    Wait time
 *   0          0 ms
 *   1          2.8 ms
 *   2          5.6 ms
 *   3          8.4 ms
 *   4         14.0 ms
 *   5         22.4 ms
 *   6         30.8 ms
 *   7         39.2 ms
 *
 * @return the current wait time between gestures. 0xFF on error.
 */
uint8_t getGestureWaitTime(void)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2);

    /* Mask out GWTIME bits */
    val &= 0b00000111;

    return val;
}

/**
 * @brief Sets the time in low power mode between gesture detections
 *
 * Value    Wait time
 *   0          0 ms
 *   1          2.8 ms
 *   2          5.6 ms
 *   3          8.4 ms
 *   4         14.0 ms
 *   5         22.4 ms
 *   6         30.8 ms
 *   7         39.2 ms
 *
 * @param[in] the value for the wait time
 * @return True if operation successful. False otherwise.
 */
bool setGestureWaitTime(uint8_t time)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2);

    /* Set bits in register to given value */
    time &= 0b00000111;
    val &= 0b11111000;
    val |= time;

    /* Write register value back into GCONF2 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF2, val);

    return true;
}

/**
 * @brief Gets the low threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return True if operation successful. False otherwise.
 */
uint16_t getLightIntLowThreshold(void)
{
    uint8_t val_byte;
    uint16_t threshold = 0;

    /* Read value from ambient light low threshold, low byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AILTL);
    threshold = val_byte;

    /* Read value from ambient light low threshold, high byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AILTH);
    threshold = threshold + ((uint16_t)val_byte << 8);

    return true;
}

/**
 * @brief Sets the low threshold for ambient light interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool setLightIntLowThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;

    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;

    /* Write low byte */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AILTL, val_low);

    /* Write high byte */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AILTH, val_high);

    return true;
}

/**
 * @brief Gets the high threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return True if operation successful. False otherwise.
 */
uint16_t getLightIntHighThreshold(void)
{
    uint8_t val_byte;
    uint16_t threshold = 0;

    /* Read value from ambient light high threshold, low byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AIHTL);
    threshold = val_byte;

    /* Read value from ambient light high threshold, high byte register */
    val_byte = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AIHTH);
    threshold = threshold + ((uint16_t)val_byte << 8);

    return true;
}

/**
 * @brief Sets the high threshold for ambient light interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool setLightIntHighThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;

    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;

    /* Write low byte */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AIHTL, val_low);

    /* Write high byte */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AIHTH, val_high);

    return true;
}

/**
 * @brief Sets the low threshold for proximity interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool setProximityIntLowThreshold(uint8_t threshold)
{

    /* Write threshold value to register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PILT, threshold);

    return true;
}

/**
 * @brief Gets the high threshold for proximity interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 * @return True if operation successful. False otherwise.
 */
uint8_t getProximityIntHighThreshold(void)
{
    uint8_t threshold = 0;

    /* Read value from proximity low threshold register */
    threshold = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PIHT);

    return threshold;
}

/**
 * @brief Sets the high threshold for proximity interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
bool setProximityIntHighThreshold(uint8_t threshold)
{

    /* Write threshold value to register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PIHT, threshold);

    return true;
}

/**
 * @brief Gets if ambient light interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getAmbientLightIntEnable(void)
{
    uint8_t val;

    /* Read value from ENABLE register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE);

    /* Shift and mask out AIEN bit */
    val = (val >> 4) & 0b00000001;

    return val;
}

/**
 * @brief Turns ambient light interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
bool setAmbientLightIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 4;
    val &= 0b11101111;
    val |= enable;

    /* Write register value back into ENABLE register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE, val);

    return true;
}

/**
 * @brief Gets if proximity interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getProximityIntEnable(void)
{
    uint8_t val;

    /* Read value from ENABLE register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE);

    /* Shift and mask out PIEN bit */
    val = (val >> 5) & 0b00000001;

    return val;
}

/**
 * @brief Turns proximity interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
bool setProximityIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into ENABLE register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_ENABLE, val);

    return true;
}

/**
 * @brief Gets if gesture interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getGestureIntEnable(void)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF4);

    /* Shift and mask out GIEN bit */
    val = (val >> 1) & 0b00000001;

    return val;
}

/**
 * @brief Turns gesture-related interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
bool setGestureIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF4);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 1;
    val &= 0b11111101;
    val |= enable;

    /* Write register value back into GCONF4 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF4, val);

    return true;
}

/**
 * @brief Clears the ambient light interrupt
 *
 * @return True if operation completed successfully. False otherwise.
 */
bool clearAmbientLightInt(void)
{
    uint8_t throwaway;
    throwaway = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_AICLEAR);

    return true;
}

/**
 * @brief Clears the proximity interrupt
 *
 * @return True if operation completed successfully. False otherwise.
 */
bool clearProximityInt(void)
{
    uint8_t throwaway;
    throwaway = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_PICLEAR);

    return true;
}

/**
 * @brief Tells the state machine to either enter or exit gesture state machine
 *
 * @param[in] mode 1 to enter gesture state machine, 0 to exit.
 * @return True if operation successful. False otherwise.
 */
bool setGestureMode(uint8_t mode)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    val = I2C_Master_ReadByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF4);

    /* Set bits in register to given value */
    mode &= 0b00000001;
    val &= 0b11111110;
    val |= mode;

    /* Write register value back into GCONF4 register */
    I2C_Master_WriteByte(i2cBase,APDS9960_I2C_ADDR,APDS9960_GCONF4,val);

    return true;
}
