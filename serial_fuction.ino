
/*****串口初始化*****/
void serial_init(u32 baud){
  Serial.begin(baud);
}

/*****串口发送字节*****/
void send_byte(u8 dat) {
  Serial.write(dat);
}

/*****串口发送字符串*****/
void send_string(char *s) {
  while (*s) {
    Serial.print(*s++);
  }
}

/*****串口中断似乎是默认运行*****/
void serialEvent(void) {
  static u8 temp_data;

  while(Serial.available()) {
    temp_data = Serial.read();
    /*******返回接收到的指令*******/
    send_byte(temp_data);
    /*******若正在执行命令，则不存储命令*******/
    if(data_ready) return;
    /*******检测命令起始*******/
    if(temp_data == '$') {
      receive_data_index = 0;
    }
    /*******检测命令结尾*******/
    else if(temp_data == '!'){
      receive_data[receive_data_index] = temp_data;
      Serial.println();
      data_ready = 1;
      return;
    } 
    receive_data[receive_data_index++] = temp_data;
    /*******检测命令长度*******/
    if(receive_data_index >= DATA_MAX_SIZE) {
      receive_data_index = 0;
    }
  }
  return;
}

/*****处理串口保存下来的数据$F1000T1000!*****/
void data_loop(void) {
  if(data_ready) {
    int time_temp=0;
    int aim_temp=0;
    for(int j = 1; j < sizeof(receive_data)-1; j++){
      if(receive_data[j]=='A') servo_index=0;
      if(receive_data[j]=='B') servo_index=1;
      if(receive_data[j]=='C') servo_index=2;
      if(receive_data[j]=='D') servo_index=3;
      if(receive_data[j]=='E') servo_index=4;
      if(receive_data[j]=='F') servo_index=5;
      if(receive_data[j]=='T'){
        for(int k=0;k<4;k++){
            j++;
            if(k==0){
              time_temp=time_temp+(receive_data[j]-'0')*1000;
            }else{
              time_temp=time_temp+(receive_data[j]-'0')*1000/pow(10,k);
            }
          }
      }
      if(j == 1){
        for(int k=0;k<4;k++){
          j++;
          if(k==0){
            aim_temp=aim_temp+(receive_data[j]-'0')*1000;
          }else{
            aim_temp=aim_temp+(receive_data[j]-'0')*1000/pow(10,k);
          }
        }
      }
    }
    
    time_data[servo_index]=(int)time_temp;
    aim_data[servo_index]=(int)aim_temp;
    servo_run(servo_index,aim_data[servo_index],time_data[servo_index]);
    receive_data_index = 0;
    data_ready = 0;
    memset(receive_data, 0, sizeof(receive_data));
  }
}
