/*
 * SPDX-FileCopyrightText:
 *
 * SPDX-License-Identifier: 
*/

#include <time.h>
#include <stdio.h>
#include <stdint.h>

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

static uint8_t lora_read_register(int fd, uint8_t reg, unsigned char bits, unsigned int speed) {
    uint8_t tx_data[2] = { reg & 0x7F, 0x00 };
    uint8_t rx_data[2] = {0};

    //Data structure for SPI transfer
    struct spi_ioc_transfer tr = {
        .tx_buf = (uint8_t)tx_data,
        .rx_buf = (uint8_t)rx_data,
        .len = 2,
        .speed_hz = speed,
        .delay_usecs = 0,
        .bits_per_word = bits,
    };

    //Send data and receive response
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("Error en la transferencia SPI");
        return 1;
    }
    return rx_data[1];
}






// Funcion de escritura registros fifo LoRA( MSB + adress // payload )
static void lora_write_register(uint8_t address, uint8_t payload, int fd, unsigned char bits, unsigned int speed) {
    uint8_t tx_data[2] = { address | 0x80, payload }; 

    //Data structures for SPI transfer
    struct spi_ioc_transfer tr = {
        .tx_buf = (uint8_t)tx_data,
        .rx_buf = (uint8_t)NULL,
        .len = 2,
        .speed_hz = speed,
        .delay_usecs = 0,
        .bits_per_word = bits,
    };
    //Send data 
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("Error en la transferencia SPI");
        return 1;
    }
    
}











// Función para enviar una cadena
static void lora_send_packetb(  const uint8_t *data,
                                size_t length,
                                int fd, 
                                unsigned char bits, 
                                unsigned int speed)     
{
    // defino largo del payload
    lora_write_register(0x22, length, fd, bits, speed);

    // configuro direcciones del FIFO (FIFO TX base and FIFO pointer)
    lora_write_register(0x0E, 0x00, fd, bits, speed);
    lora_write_register(0x0D, 0x00, fd, bits, speed);

    //----------------------------------------
    // Write payload to FIFO
    //----------------------------------------

    uint8_t *tx = malloc(length + 1);

    if (tx == NULL) {
        fprintf(stderr, "Unable to allocate SPI buffer\n");
        return;
    }
    
    tx[0] = 0x80;          // FIFO register (0x00) with write bit
    memcpy(&tx[1], data, length);

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = 0,
        .len = length + 1,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        perror("SPI_IOC_MESSAGE");
        free(tx);
        return;
    }

    free(tx);//Limpio memoria del buffer de transmisión

    //----------------------------------------
    // Start transmission
    //----------------------------------------

    lora_write_register(0x12,0xFF, fd, bits, speed); // limpio flags irq antes de transmitir
    lora_write_register(0x01,0x83, fd, bits, speed); // regop: lora+tx

    //----------------------------------------
    // Wait until TxDone
    //----------------------------------------

    uint8_t irq_flags;

    for (int timeout = 1000; timeout > 0; timeout--) {

        irq_flags = lora_read_register(0x12, fd, bits, speed);

        if (irq_flags & 0x08) {      // TxDone
            printf("Packet sent (%zu bytes)\n", length);
            break;
        }

        usleep(1000);                // 1 ms
    }

    // Clear IRQ flags
    lora_write_register(0x12, 0xFF, fd, bits, speed);

    // Return to sleep
    lora_write_register(0x01, 0x80, fd, bits, speed);
}



void lora_spi_init(int fd, unsigned char device, unsigned char mode, unsigned char bits, unsigned int speed){

    // 1. Abrir el dispositivo SPI

    fd = open(device, O_RDWR); //Open Read/Write
    if (fd < 0) {
        perror("Error al abrir el dispositivo SPI");
        return 1;
    }

    
    // 2. Configurar el dispositivo SPI
    
    // Configurar el modo (Polaridad y Fase del reloj)
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("Error al configurar el modo SPI");
        return 1;
    }

    // Configurar los bits por palabra
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("Error al configurar los bits por palabra");
        return 1;
    }

    // Configurar la velocidad máxima del reloj
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("Error al configurar la velocidad");
        return 1;
    }
}




void lora_init(int fd, unsigned char device, unsigned char mode, unsigned char bits, unsigned int speed){
    // Configurar pines CS y RST como salida
    // Luego aplico RESET al modulo LoRa

    // inicializacion de SPI para LoRa
    lora_spi_init(fd, device, mode, bits, speed);

    //Verify chip version
    lora_get_version(fd, bits, speed);

    // Configuración básica LoRa (modo standby, frecuencia, potencia, etc.)
    lora_write_register(0x01, 0x80, fd, bits, speed); // RegOpMode: LoRa + Sleep
    usleep(10000); // 10 ms
    lora_write_register(0x01, 0x81, fd, bits, speed); // RegOpMode: LoRa + standby

    // Frecuencia 433 MHz (para SX1278)
    lora_write_register(0x06, 0x6C, fd, bits, speed);
    lora_write_register(0x07, 0x80, fd, bits, speed);
    lora_write_register(0x08, 0x00, fd, bits, speed);

    // Potencia de transmisión
    lora_write_register(0x09, 0x8F, fd, bits, speed); // Potencia supuestamente "ideal"
}

uint8_t lora_get_version(int fd, unsigned char bits, unsigned int speed) {
    uint8_t version = lora_read_register(0x42, fd, bits, speed);
    printf("LoRa chip version: 0x%02X\n", version);
    return version;
}