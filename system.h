//////////////////////////////////////////////////////
// SYSTEM CONFIGURATION FILE
// Change the configurations below depending on your specific node setup.
#ifndef _SYSTEM_H
#define _SYSTEM_H
/////////////////////////////////////////
// GENERAL SETTINGS
// General Settings
#define DEVICE_ID      7

// Run this command to see the REF_VOLTAGE: "espefuse.py --port /dev/cu.usbserial-1410 adc_info" COM3
#define REF_VOLTAGE 1086 //1128
#define CONNECTION_TRY_LIMIT 10 // Number of cycles to try WiFi connection 
#define MAX_SAVED_READINGS    20 // How many max previous readings to backup
#define FAILED_READINGS_LIMIT 50 // number of failed readings to store in memory

// Send data as together or each reading seperate
#define SEND_DATA_GROUPED 0

// Reset the device on failed reading instead of sleeping
#define RESET_ON_FAIL 0

// Disable brownout detection
#define DISABLE_BROWNOUT 1

// Time to deep sleep in seconds
#define TIME_TO_SLEEP  60

// Time to deep sleep in seconds after failed send
#define TIME_TO_SLEEP_ON_FAIL  30 


/////////////////////////////////////////
// BATTERY SETTINGS
#define BATTERY_MAX   4100 //maximum voltage of battery 4.2v
#define BATTERY_MIN   2800 //minimum voltage of battery before shutdown
// Battery Voltage Divider Resistor Values

const float R1 = 100000; // resistance of R1 (100K)
const float R2 = 22000;  // resistance of R2 (22K)
// I = (BATTERY_V * R2) / (R1 + R2)


/////////////////////////////////////////
// SYSTEM SETTINGS
// Do not change these unless you know what you are doing!
#define VERSION 8.0

// Reading Type Constants to Save Memory
#define READING_TYPE_VOLTAGE      0
#define READING_TYPE_SOIL         1
#define READING_TYPE_TEMPERATURE  2
#define READING_TYPE_HUMIDITY     3
#define READING_TYPE_RAIN         4
#define READING_TYPE_PRESSURE     5 
#define READING_TYPE_FLOAT        6
#define READING_TYPE_PH           7
#define READING_TYPE_EC           8
#define READING_TYPE_LIGHT        9
#define READING_TYPE_HEATINDEX    10

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

#endif
