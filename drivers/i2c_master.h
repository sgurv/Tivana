/*
 * i2c_master.h
 *
 *  Created on: Nov 10, 2022
 *      Author: sandeep
 */

#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

void I2C_Master_WriteByte(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress, uint8_t data);
uint8_t I2C_Master_WriteBytes(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress,
                           uint16_t length, uint8_t *data);
uint8_t I2C_Master_ReadByte(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress);
uint8_t I2C_Master_ReadBytes(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress,
                          uint16_t length, uint8_t* data);

#endif /* DRIVERS_I2C_MASTER_H_ */
