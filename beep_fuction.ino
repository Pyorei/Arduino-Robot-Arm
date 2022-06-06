/*******BEEP管脚映射表*******/
#define BEEP_PIN 4

/*******BEEP快捷指令表*******/
#define BEEP_ON() digitalWrite(BEEP_PIN,HIGH)   //蜂鸣器BEEP打开,高电平1?
#define BEEP_OFF() digitalWrite(BEEP_PIN,LOW)  //蜂鸣器BEEP关闭，低电平0

/*******BEEP初始化*******/
void beep_init(void) {
  pinMode(BEEP_PIN,OUTPUT);         //设置引脚为输出模式
  BEEP_OFF();
}

/*******BEEP短鸣两声*******/
void beep_short(void){
  BEEP_ON();delay(100);BEEP_OFF();delay(100);
  BEEP_ON();delay(100);BEEP_OFF();delay(100);
}
