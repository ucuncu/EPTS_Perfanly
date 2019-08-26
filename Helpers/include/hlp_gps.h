#include "type.h"

#define GPS_BUFFER_LENGHT  82  //Max NMEA data 80 byte, 1 byte for safety,1 byte termination char '/0' 
#define GPS_BUFFER_HEIGHT  2
#define GPS_DATA_BUFFER_HEIGHT  1
#define GPS_TIME_BUFFER_LENGHT  10
#define GPS_LAT_BUFFER_LENGHT  11
#define GPS_LON_BUFFER_LENGHT  12
#define GPS_FIX_BUFFER_LENGHT  2
#define GPS_VELOCITY_BUFFER_LENGHT  6
#define GPS_DATE_BUFFER_LENGHT  7
#define GPS_ALTITUDE_BUFFER_LENGHT  6


extern char GPS_DATA_BUF[GPS_BUFFER_HEIGHT][GPS_BUFFER_LENGHT];
extern char GPS_DATA_BUF_TMP[GPS_BUFFER_HEIGHT][GPS_BUFFER_LENGHT];
extern char GPS_TIME_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_TIME_BUFFER_LENGHT];
extern char GPS_LAT_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_LAT_BUFFER_LENGHT];
extern char GPS_LON_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_LON_BUFFER_LENGHT];
extern char GPS_FIX_DATA_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_FIX_BUFFER_LENGHT];
extern char GPS_VELOCITY_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_VELOCITY_BUFFER_LENGHT];
extern char GPS_DATE_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_DATE_BUFFER_LENGHT];
extern char GPS_ALTITUDE_DATA_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_ALTITUDE_BUFFER_LENGHT];

extern void initGPS(void);
extern void gps_power_down(void);

//const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };

//struct NAV_PVT {
//  unsigned char cls;
//  unsigned char id;
//  unsigned short len;
//  unsigned long iTOW;          // GPS time of week of the navigation epoch (ms)

//  unsigned short year;         // Year (UTC) 
//  unsigned char month;         // Month, range 1..12 (UTC)
//  unsigned char day;           // Day of month, range 1..31 (UTC)
//  unsigned char hour;          // Hour of day, range 0..23 (UTC)
//  unsigned char minute;        // Minute of hour, range 0..59 (UTC)
//  unsigned char second;        // Seconds of minute, range 0..60 (UTC)
//  char valid;                  // Validity Flags (see graphic below)
//  unsigned long tAcc;          // Time accuracy estimate (UTC) (ns)
//  long nano;                   // Fraction of second, range -1e9 .. 1e9 (UTC) (ns)
//  unsigned char fixType;       // GNSSfix Type, range 0..5
//  char flags;                  // Fix Status Flags
//  unsigned char reserved1;     // reserved
//  unsigned char numSV;         // Number of satellites used in Nav Solution

//  long lon;                    // Longitude (deg)
//  long lat;                    // Latitude (deg)
//  long height;                 // Height above Ellipsoid (mm)
//  long hMSL;                   // Height above mean sea level (mm)
//  unsigned long hAcc;          // Horizontal Accuracy Estimate (mm)
//  unsigned long vAcc;          // Vertical Accuracy Estimate (mm)

//  long velN;                   // NED north velocity (mm/s)
//  long velE;                   // NED east velocity (mm/s)
//  long velD;                   // NED down velocity (mm/s)
//  long gSpeed;                 // Ground Speed (2-D) (mm/s)
//  long heading;                // Heading of motion 2-D (deg)
//  unsigned long sAcc;          // Speed Accuracy Estimate
//  unsigned long headingAcc;    // Heading Accuracy Estimate
//  unsigned short pDOP;         // Position dilution of precision
//  short reserved2;             // Reserved
//  unsigned long reserved3;     // Reserved
//};

//struct NAV_PVT pvt;

