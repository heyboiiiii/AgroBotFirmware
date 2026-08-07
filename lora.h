/**
 * @file lora.h
 * @brief Core functions to work with module LoRa XL1278 in Raspberry PI ZERO 2W(SPI).
 * @author Della Torre Joaquin
 * @date 2026-08-06
*/

#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include <stdio.h>


/***************************************************

//Pins assigment for The primary SPI bus (SPI0) on the Raspberry Pi Zero 2 W

SCLK ---> Pin 23 (GPIO 11).
MOSI ---> Pin 19 (GPIO 10).
MISO ---> Pin 21 (GPIO 9).


************************************************** */

/** 
 * @brief Initialize LoRa module.
 * Default config:
 *  
 *  Ptx = max
 *  Freq = 433Mhz
 *   
 * @param fd Device interface 
 * @param device SPI device
 * @param mode SPI MODE
 * @param bits Number of bits to tx and rx per packet
 * @param speed Bus comunication speed
 * @param tr Structure to store payload
 */


void lora_init(int fd, unsigned char device, unsigned char mode, unsigned char bits, unsigned int speed,struct spi_ioc_transfer *tr);








/** 
 * @brief Read register inside LoRa module.
 * 
 * @param fd Device interface
 * @param reg Register address to read
 * @param bits Number of bits to tx and rx per packet
 * @param speed Bus comunication speed
*/

static uint8_t lora_read_register(int fd,uint8_t reg,unsigned char bits, unsigned int speed);





/** 
 * @brief Write register inside LoRa module.
 * 
 * 
 * @param address Address of the register to write
 * @param payload Value to write in register
 * @param fd Device interface
 * @param bits Number of bits to tx and rx per packet
 * @param speed Bus comunication speed
*/

static void lora_write_register(uint8_t address, uint8_t payload, int fd, unsigned char bits, unsigned int speed);





/** 
 * @brief Send a binary packet through LoRa module.
 * 
 * 
 * @param data Pointer to the data to send
 * @param length Length of the data to send
 * @param fd Device interface
 * @param bits Number of bits to tx and rx per packet
 * @param speed Bus comunication speed
*/

static void lora_send_packetb(  const uint8_t *data,
                                size_t length,
                                int fd, 
                                unsigned char bits, 
                                unsigned int speed)  ;
#endif