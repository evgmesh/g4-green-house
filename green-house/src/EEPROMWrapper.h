/*
 * EEPROMWrapper.h
 *
 *  Created on: Dec 4, 2022
 *      Author: tylen
 */

#ifndef EEPROMWRAPPER_H_
#define EEPROMWRAPPER_H_

#include "FreeRTOS.h"
#include "task.h"
#include "chip.h"
#include <assert.h>
#include <string>


typedef void (*IAP_call) (uint32_t[], uint32_t[]);

typedef struct _eeprom
{
  uint32_t data;
  uint32_t addr;
  uint32_t size;
  uint32_t mode;
  uint32_t clock;
} EEPROM;

#define READ true
#define WRITE false
#define EEPROM_MAX_BUFER_SIZE 1000

class EEPROM_Wrapper
{
public:
  /**
   * @brief Construct a new eeprom wrapper object
   *
   */
  EEPROM_Wrapper ();
  virtual ~EEPROM_Wrapper ();
  /**
   * @brief Read a string from EEPROM
   *
   * @param addr - address to read from
   * @param amount - amount of bytes to read
   * @return std::string - that was read
   */
  std::string str_read_from (uint32_t addr, uint32_t amount);
  /**
   * @brief Write a string to EEPROM
   *
   * @param addr - address to write on
   * @param str - string to write
   */
  void write_to (uint32_t addr, std::string str);
  /**
   * @brief Read data from EEPROM
   *
   * @param addr - address to read from
   * @param amount - amount of bytes to read
   * @return void* - data that was read
   */
  void *read_from (uint32_t addr, uint32_t amount);
  /**
   * @brief Write data to EEPROM
   *
   * @param addr - address to write on
   * @param data - data to be written
   * @param size_of_data - size of data to be written
   */
  void write_to (uint32_t addr, void *data, uint32_t size_of_data);

private:
  IAP_call iap_exec;
  uint32_t command[5], result[5];
  EEPROM rom = { 0, 0, 0, 0, 72000 };
  void eeprom_execute (EEPROM *rom);
  void eeprom_use (uint8_t *data, uint32_t addr, uint32_t size, bool mode);
  uint8_t buffer[EEPROM_MAX_BUFER_SIZE] = { 0 };
};

#endif /* EEPROMWRAPPER_H_ */
