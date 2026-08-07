#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

#include <signal.h>
#include <unistd.h> // For sleep()

#include "gps.h"




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

    int fd;
    const char *device = SPI_DEVICE;
    unsigned char mode = SPI_MODE_0;
    unsigned char bits = 8;
    unsigned int speed = 1000000; //1MHZ

    lora_init(fd, device, mode, bits, speed);
    
    lora_get_version(fd, bits, speed);//shows the version of the LoRa chip

    // Mostrar los datos recibidos
    //printf("Datos recibidos: 0x%02X 0x%02X\n", rx_buffer[0], rx_buffer[1]);
    
    
   
    

    

    // Read data from the UART interface
    char read_buffer[256];
    int num_bytes;

    double latitude, longitude, speedKmh;
    char lat_hemisphere, lon_hemisphere;

    printf("Listening for UART data...\n");
    

    // Register the handler for SIGINT (Ctrl+C)
    signal(SIGINT, handle_ctrl_c);

    while (keep_running) {
        // Clear the buffer for the read
        memset(&read_buffer, '\0', sizeof(read_buffer));
        // Read operation blocks until conditions in VMIN/VTIME are met
        num_bytes = read(serial_port, &read_buffer, sizeof(read_buffer) - 1);

        if (num_bytes < 0) {
            printf("Error reading: %s\n", strerror(errno));
        } else if (num_bytes == 0) {
            printf("Read timeout occurred.\n");
        } else {
            printf("Read %i bytes. Received message: %s\n", num_bytes, read_buffer);

            // Parse the GPS data
            parse_gps_data(read_buffer, &latitude, &longitude, &lat_hemisphere, &lon_hemisphere, &speedKmh);
            
            printf("Parsed GPS data: Lat: %lf %c, Lon: %lf %c, Speed: %lf km/h\n", latitude, lat_hemisphere, longitude, lon_hemisphere, speedKmh);
        }

    }

    printf("Exiting gracefully...\n");
    // Close the port
    close(serial_port);
    // Close the SPI device
    //close(fd);
    return 0;
}

