#include <Servo.h>

#define DATA_MAX_SIZE 12
#define SERVO_NUM 6

typedef struct {
  int aim;
  float cur;
  float inc;
  int time_set;
}servo_struct;

servo_struct servo_data[SERVO_NUM];
u8 servo_index;

byte  servo_pin[SERVO_NUM] = {7, 3, 5, 6, 9 ,8};
int aim_data[SERVO_NUM];
int time_data[SERVO_NUM];

u8 i=0;
u8 receive_data[DATA_MAX_SIZE]={0}, receive_data_index, data_ready;
Servo myservo[SERVO_NUM];

void setup() {
  led_beep_setup();
  serial_setup();
  setup_servo();
  setup_finish();
}

void loop() {
  led_loop();
  data_loop();
  servo_loop();
}
