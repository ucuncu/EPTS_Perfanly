
#ifndef _MMA8452Q_H_
#define _MMA8452Q_H_

#define MMA8452Q_ADD 0x38
#define MMA8452Q_WHO_AM_I_ADD 0x0D

#define STATUS           0x00
#define OUT_X_MSB        0x01    
#define OUT_X_LSB        0x02
#define OUT_Y_MSB        0x03
#define OUT_Y_LSB        0x04
#define OUT_Z_MSB        0x05
#define OUT_Z_LSB        0x06
#define SYSMOD           0x0B
#define INT_SOURCE       0x0C
#define WHO_AM_I         0x0D   
#define XYZ_DATA_CFG     0x0E
#define HP_FILTER_CUTOFF 0x0F
#define PL_STATUS        0x10
#define PL_CFG           0x11
#define PL_COUNT         0x12
#define PL_BF_ZCOMP      0x13
#define P_L_THS_REG      0x14
#define FF_MT_CFG        0x15
#define FF_MT_SRC        0x16
#define FF_MT_THS        0x17
#define FF_MT_COUNT      0x18
#define TRANSIENT_CFG    0x1D
#define TRANSIENT_SRC    0x1E
#define TRANSIENT_THS    0x1F
#define TRANSIENT_COUNT  0x20
#define PULSE_CFG        0x21
#define PULSE_SRC        0x22
#define PULSE_THSX       0x23
#define PULSE_THSY       0x24
#define PULSE_THSZ       0x25
#define PULSE_TMLT       0x26
#define PULSE_LTCY       0x27
#define PULSE_WIND       0x28
#define ASLP_COUNT       0x29
#define CTRL_REG1        0x2A
#define CTRL_REG2        0x2B
#define CTRL_REG3        0x2C
#define CTRL_REG4        0x2D
#define CTRL_REG5        0x2E
#define OFF_X            0x2F
#define OFF_Y            0x30
#define OFF_Z            0x31

extern void read_acc (void);
extern void tapHandler( void );
extern void portraitLandscapeHandler( void );
extern void checkAccelerometer(void);
extern void acc_low_power_mode (void);

//extern int ACC_X_BUF[10];
//extern int ACC_Y_BUF[10];
//extern int ACC_Z_BUF[10];

extern int ACC_X_BUF;
extern int ACC_Y_BUF;
extern int ACC_Z_BUF;
//extern int ACC_ERR_BUF;

#endif
