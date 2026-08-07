#include "neo6m.h"

#include <signal.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>




static double convert_to_decimal_degrees(double raw_coordinate) {
    // Extract the integer part as degrees.
    // For 3445.30788, floor(3445.30788 / 100) gives 34.
    double degrees = floor(raw_coordinate / 100.0);

    // Extract the fractional part (after removing degrees) as minutes.
    // For 3445.30788, fmod(3445.30788, 100.0) gives 45.30788.
    double minutes = fmod(raw_coordinate, 100.0);

    // Calculate the decimal degrees.
    // 34 + (45.30788 / 60)
    return degrees + (minutes / 60.0);
}
static float parse_gprmc_speed(const char *nmea_sentence) {
    if (strstr(nmea_sentence, "$GPRMC") == NULL) return -1;

    char *tokens[12] = {0};
    char *temp = strdup(nmea_sentence);
    char *token = strtok(temp, ",");

    int i = 0;
    while (token != NULL && i < 12) {
        tokens[i++] = token;
        token = strtok(NULL, ",");
    }

    float speed_knots = 0.0;
    if (i >= 8 && tokens[7] != NULL && strlen(tokens[7]) > 0) {
        speed_knots = atof(tokens[7]);
    }

    free(temp);

    // Convert knots to km/h
    return speed_knots * 1.852;
}

void neo6m_get_parse_data(const char *buffer, double *latitude, double *longitude, char *lat_hemisphere, char *lon_hemisphere, double *speedKmh) {

    //data adquisiton latitude and longtitude
    char lat[12], lon[12];
    // Declare these new char variables to store 'S', 'W', 'N', or 'E'
    char lat_h; // Will store 'S' or 'N'
    char lon_h; // Will store 'W' or 'E'
    
    const char *indexGP;

    indexGP = strstr(buffer, "$GPGGA");
    
    if (indexGP != NULL) {
        
        sscanf(indexGP, "$GPGGA,%*[^,],%11[^,],%c,%11[^,],%c", lat, &lat_h, lon, &lon_h);

        *latitude = convert_to_decimal_degrees(atof(lat));
        *longitude = convert_to_decimal_degrees(atof(lon));

        *lat_hemisphere = lat_h;
        *lon_hemisphere = lon_h;
       
    }

    indexGP = strstr(buffer, "$GPRMC");

    if(indexGP != NULL){
        *speedKmh = parse_gprmc_speed(indexGP);
    }

    printf("Parsing GPS data: %lf / %c ^^ lon: %lf / %c, Speed: %lf km/h\n", *latitude, *lat_hemisphere, *longitude, *lon_hemisphere, *speedKmh);
}






















void set_tty_conf(struct termios *tty) {
    // Set control modes
    tty->c_cflag &= ~PARENB;        // Clear parity bit (No parity)
    tty->c_cflag &= ~CSTOPB;        // Clear stop field (1 stop bit)
    tty->c_cflag &= ~CSIZE;         // Clear bits-per-byte size
    tty->c_cflag |= CS8;            // 8 bits per byte
    tty->c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control
    tty->c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines

    // Set local, input, and output modes for raw data
    tty->c_lflag &= ~ICANON;        // Disable canonical mode (read line-by-line)
    tty->c_lflag &= ~ECHO;          // Disable echo
    tty->c_lflag &= ~ECHOE;         // Disable erasure
    tty->c_lflag &= ~ECHONL;        // Disable new-line echo
    tty->c_lflag &= ~ISIG;          // Disable interpretation of INTR, QUIT and SUSP
    tty->c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow control
    tty->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable special handling of bytes
    tty->c_oflag &= ~OPOST;         // Prevent special interpretation of output bytes
    tty->c_oflag &= ~ONLCR;         // Prevent conversion of newline to carriage return

    // Set timeout behavior (Blocking Read)
    tty->c_cc[VMIN] = 1;            // Wait for at least 1 character
    tty->c_cc[VTIME] = 10;          // 1 second timeout (10 deciseconds)

    // Set Baud Rate (9600)
    cfsetispeed(tty, B9600);
    cfsetospeed(tty, B9600);
}

void gps_init(char* SERIAL_PORT) {

    // 1. Open the serial port device file
    // O_RDWR: Read/Write, O_NOCTTY: Prevents terminal control

    int serial_port = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return 1;
    }

    // 2. Configure the serial port using termios
    struct termios tty;
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        close(serial_port);
        return 1;
    }

    //Configure the tty settings for the serial port
    set_tty_conf(&tty);

    // Save tty settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        close(serial_port);
        return 1;
    }


}

