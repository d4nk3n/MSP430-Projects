/**
 * @brief Read Temperature from DS18B20 with 1-Wire protocol
 * @attention We use P2.0 as DQ
 */

#ifndef DS18B20_H

#define DS18B20_H

/**
 * @brief initialize ds18b20
 * @return 1 if found ds18b20, 0 if not.
 */
int ds18b20_init();


/**
 * @brief set DQ input and pullup
 * @return None
 */
void ds18b20_set_input();


/**
 * @brief read a single bit from ds18b20
 * @return bit read from ds18b20, lsb first
 */
int ds18b20_readbit();


/**
 * @brief read byte from ds18b20
 * @return data read from ds18b20
 */
int ds18b20_readbyte();


/**
 * @brief write byte to ds18b20
 * @return None
 */
void ds18b20_writebyte(int wbyte);

/**
 * @brief get temperature from raw data
 * @param rdata 16-bit long, raw data read from ds18b20
 * @param tempstr Celsius temperature in string
 * @return None
 */
void ds18b20_convert2temp(int rdata, char *tempstr);

#endif
