/*****LED和BEEP的初始化*****/
void led_beep_setup(void){
  led_init();
  beep_init();
}

/*****串口的初始化*****/
void serial_setup(void){
  serial_init(115200);
}

/*****舵机的初始化*****/
void setup_servo(void) {
  for(servo_index=0;servo_index<SERVO_NUM;servo_index++) {
    servo_data[servo_index].aim=1500;
    servo_data[servo_index].cur=1500;
    servo_data[servo_index].inc=0;
    servo_data[servo_index].time_set=0;
  }
  servo_init();
}

/*****setup部分运行完毕的提示*****/
void setup_finish(void){
  beep_short();
}
