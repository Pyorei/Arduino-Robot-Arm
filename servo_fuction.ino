/*****串口的初始化*****/
void servo_init(void) {
  for(byte i = 0; i < SERVO_NUM; i ++) {
    myservo[i].attach(servo_pin[i]);
    myservo[i].writeMicroseconds(servo_data[i].aim);
  }
}

/*****舵机运行index（编号）、aim（目标角度）、time（运行时间）*****/
void servo_run(u8 index, int aim, int time_set) {
  if(index < SERVO_NUM && (aim<=2500)&& (aim>=500) && (time_set<10000)) {
    if(aim>2500)  aim=2500;
    if(aim<500)   aim=500;
    
    if(time_set < 20) {
      servo_data[index].aim = aim;
      servo_data[index].cur = aim;
      servo_data[index].inc = 0;
    } else {
      servo_data[index].aim = aim;
      servo_data[index].time_set = time_set;
      servo_data[index].inc = (servo_data[index].aim-servo_data[index].cur)/(time_set/20);
    }
  }
}

/*****循环处理舵机的指令*****/
void servo_loop(void) {
  static long long systick_ms_bak = 0;
  if(millis() - systick_ms_bak > 20) {
    systick_ms_bak = millis();
    for(byte i=0; i<SERVO_NUM; i++) {
      if(servo_data[i].inc != 0) {
        if(servo_data[i].aim>2500)  servo_data[i].aim=2500;
        if(servo_data[i].aim<500)   servo_data[i].aim=500;
        if(abs(servo_data[i].aim - servo_data[i].cur) <= abs(servo_data[i].inc)) {
          myservo[i].writeMicroseconds(servo_data[i].aim);
          servo_data[i].cur = servo_data[i].aim;
          servo_data[i].inc = 0;
        } else {
          servo_data[i].cur += servo_data[i].inc;
          myservo[i].writeMicroseconds(int(servo_data[i].cur));
        }
      }
    }
  }
}
