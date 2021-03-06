# Arduino-Robot-Arm

此处重点介绍串口和舵机函数库的使用，更多详细的内容可以参考CSDN的博文[https://blog.csdn.net/Pariya_Official/article/details/121054507?spm=1001.2014.3001.5501](https://blog.csdn.net/Pariya_Official/article/details/121054507?spm=1001.2014.3001.5501)

### 串口的使用

这里将有用的串口信息存放到数组receive_data中，由data_ready做判断是否需要将数组送入别的方法做字符串的整理和筛选，具体代码如下：

`串口的使用`

```c++
#define DATA_MAX_SIZE 12	//起始符到终止符的总字符数

u8 receive_data[DATA_MAX_SIZE]={0}, receive_data_index, data_ready，servo_index;

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

/*****串口中断，会在loop循环结束时自动运行，即每次循环执行一次*****/
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
    servo_run(servo_index,aim_data[servo_index],time_data[servo_index]);	//见舵机部分
    receive_data_index = 0;
    data_ready = 0;
    memset(receive_data, 0, sizeof(receive_data));
  }
}

void setup(){
    serial_init(115200);
}

void loop(){
    data_loop();
}
```

代码基本是参考商家提供的源码进行了些许修改，baud为波特率即一秒处理多少字节的数据。此外，可以不将串口的数据存为数组，改为逐个的将串口数据读取判断，且可以使用Serial.parseInt()将一段数字直接取出，无需将字符串的数据重新整理，但代码整体会显得较为臃肿，一个方法内需要有读取数据和处理数据两个功能。

### Servo库的使用

此处使用的是舵机（Servo），通过PWM信号驱动，需要每20ms接受一次信号。因此，为了实现舵机控制的顺滑性，需要对一个角度动作进行拆解连续控制，以下代码可供参考：

`舵机的连续控制`

```c++
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

Servo myservo[SERVO_NUM];	//创建舵机类数组

/*****舵机的初始化*****/
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
  if(millis() - systick_ms_bak > 20) {	//每隔20ms控制一次舵机
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

void setup(){
    servo_init();
}

void loop(){
    servo_loop();
}
```

与串口的代码相同，此处也是参考商家的源码进行整理，重点还是以实现功能为主。此处引入舵机运行的时间并非最佳，后续还可以根据PID算法对各个角度的运行时间进行优化，使得机械臂能够更加快速、顺滑地到达指定坐标。此外，舵机的自动控制需要引入更多的传感器和算法，而指令控制需要引入上一小节串口数据处理的相关方法。
