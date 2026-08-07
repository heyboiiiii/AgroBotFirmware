#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

#include <signal.h>
#include <unistd.h> // For sleep()

#include "neo6m.h"




// Volatile flag ensures the compiler doesn't cache this variable in a CPU register
volatile sig_atomic_t keep_running = 1;

// The signal handler function
void handle_ctrl_c(int sig) {
    keep_running = 0; 
}


#define SERIAL_PORT "/dev/ttyUSB0"
#define SPI_DEVICE "/dev/spidev0.0"




int main() {
    /*
    ******************************************************************************
                            GPS NEO6M UART interface
    ******************************************************************************
    */

    
    

    







    /*

    ******************************************************************************
                            LORA XL1278 SPI interface
    ******************************************************************************
    
    */

    int fd; // SPI interface
    const char *device = SPI_DEVICE; // Device
    //unsigned char mode = NULL; // Spi mode
    unsigned char bits = 8;// 8 bits
    unsigned int speed = 1000000; //1MHZ

    lora_init(fd, device, NULL, bits, speed);//Initialize the SPI interface for the LoRa module
    
    lora_get_version(fd, bits, speed); //Shows the version of the LoRa chip
    
    /*

    ******************************************************************************
                            GPS NEO6M UART interface
    ******************************************************************************
    
    */

    double latitude, longitude; 
    double speedKmh;
    char lat_hemisphere, lon_hemisphere;

    //printf("Listening for UART data...\n");
    

    
    // Register the handler for SIGINT (Ctrl+C)
    signal(SIGINT, handle_ctrl_c);

    
    
    while (keep_running) {
        

    }

    printf("Exiting gracefully...\n");
    // Close the UART port
    neo6m_close_conn(SERIAL_PORT);    
    // Close the SPI device
    //close(fd);
    return 0;
}

void lora_listening(){

}

void gps_listening(){

}