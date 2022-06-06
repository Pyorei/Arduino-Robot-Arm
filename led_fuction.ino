/*******宏定义LED管脚映射表*******/
#define LED_PIN 13

/*******宏定义LED快捷指令表*******/
#define LED   digitalRead(LED_PIN)      //读取LED信号灯状态
#define LED_ON() digitalWrite(LED_PIN,LOW)   //LED信号灯点亮，低电平0
#define LED_OFF() digitalWrite(LED_PIN,HIGH)    //LED信号灯熄灭，高电平1

/*******LED初始化*******/
void led_init(void) {
  pinMode(LED_PIN,OUTPUT);          //设置引脚为输出模式，初始状态为关闭
  LED_OFF();
}

/*******LED变换一次*******/
void led_change(void) {
  if(LED==1) LED_ON();
  else LED_OFF();
}

/*******LED按1秒间隔闪烁*******/
void led_loop(void) {
  static unsigned long systick_ms_bak = 0;
  if (millis() - systick_ms_bak > 500) {
    systick_ms_bak = millis();
    led_change();
  }
}
