#ifndef NEO6M_H
#define NEO6M_H




/**
 * @file gps.h
 * @brief Header file for GPS data parsing functions.
 *
 * This header file contains the declaration of the function to parse GPS data from a given buffer.
 * The function extracts latitude, longitude, hemispheres, and speed in km/h from the provided GPS data string.
 *
 * @param buffer The GPS data string to parse.
 * @param latitude Pointer to store the parsed latitude.
 * @param longitude Pointer to store the parsed longitude.
 * @param lat_hemisphere Pointer to store the latitude hemisphere.
 * @param lon_hemisphere Pointer to store the longitude hemisphere.
 * @param speedKmh Pointer to store the parsed speed in km/h.
 */


void neo6m_get_parse_data(const char *buffer, double *latitude, double *longitude, char *lat_hemisphere, char *lon_hemisphere, double *speedKmh);
/**
 * @brief Initializes the NEO6M GPS module.
 * @param SERIAL_PORT The serial port to which the GPS module is connected.
 */
void neo6m_init(char* SERIAL_PORT);


/**
 * @brief Reads data from the NEO6M GPS module.
 * @param serial_port The file descriptor of the serial port.
 */
void neo6m_read_data(char* SERIAL_PORT, double *latitude, double *longitude, char *lat_hemisphere, char *lon_hemisphere, double *speedKmh);



/**
 * @brief Closes the connection to the NEO6M GPS module.
 * @param SERIAL_PORT The serial port to which the GPS module is connected.
 */
void neo6m_close_conn(char* SERIAL_PORT);

#endif