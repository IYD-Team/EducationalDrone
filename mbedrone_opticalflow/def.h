#define DEBUG_RC_SERIAL

#define RIGHT_HORIZONTAL 0
#define RIGHT_VERTICAL 1
#define LEFT_HORIZONTAL 2
#define LEFT_VERTICAL 3
#define CH_NUM 4
const uint8_t rcPin[CH_NUM]={9,5,4,8};
const int rc_offset[CH_NUM]={0,-6,0,0};

//#define ULTRASONIC_PIN 6
#define RC6CHPIN 6
#define MBED_RX 2
#define MBED_TX 7

#define ARM 'a'
#define DISARM 'd'

#define PZ 0.1
#define GAINZ 0.2

#define FILTERSIZE 4
