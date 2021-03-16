/*
Created 2016
by AlexGyver
AlexGyver Home Labs Inc.
*/
//-----------дисплей-----------
#include <TimerOne.h>
#include <TM74HC595Display.h>
byte SCLK = 7;
byte RCLK = 6;
byte DIO = 5;
TM74HC595Display disp(SCLK, RCLK, DIO);
unsigned char LED_0F[29];
//--------дисплей-------

byte disp_gnd=4; //земля дисплея
byte disp_vcc=8;  //питание дисплея

byte butt_gnd=9;  //земля кнопки
byte butt_pin=10;  //состояние кнопки

byte sens_vcc=2; //питание датчика
byte sens_pin=3; //выход датчика (принимает значение от 1 до 0)

unsigned long lastturn; //переменные хранения времени
float DIST=0.0; //переменная хранения расстояния в виде десятичной дроби
float w_length=0.87; //длина окружности колеса в метрах
byte n=4; //число отверстий / магнитов

volatile boolean flag; //флаг вывода на дисплей

void setup() {
  attachInterrupt(1,gap,RISING); //подключить прерывание на 3 пин при повышении сигнала
  
  pinMode(disp_vcc,OUTPUT); //пин как выход
  pinMode(disp_gnd,OUTPUT); //пин как выход 
  pinMode(butt_gnd,OUTPUT); //пин как выход
  pinMode(sens_vcc,OUTPUT); //пин как выход
  pinMode(butt_pin,INPUT_PULLUP); //пин кнопки подтягиваем
  
  //подаём напряжение на пины
  digitalWrite(disp_vcc,1); 
  digitalWrite(disp_gnd,0);
  digitalWrite(butt_gnd,0);
  digitalWrite(sens_vcc,1);

  //для дисплея цифры----------
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9

  LED_0F[10] = 0b01000000; //.0
  LED_0F[11] = 0b01111001; //.1
  LED_0F[12] = 0b00100100; //.2
  LED_0F[13] = 0b00110000; //.3
  LED_0F[14] = 0b00011001; //.4
  LED_0F[15] = 0b00010010; //.5
  LED_0F[16] = 0b00000010; //.6
  LED_0F[17] = 0b01111000; //.7
  LED_0F[18] = 0b00000000; //.8
  LED_0F[19] = 0b00010000; //.9
  Timer1.initialize(1500); // set a timer of length 1500
  Timer1.attachInterrupt(timerIsr); // attach the service routine here
  disp.digit4(0); 
  //--------------
  w_length=(float)w_length/n; //длина дуги колеса с учетом числа отверстий
}

void gap() {
  if (millis()-lastturn > 80) {  //защита от случайных измерений    
    lastturn=millis();  //запомнить время последнего оборота
    DIST=DIST+w_length;  //прибавляем длину колеса к дистанции при каждом обороте оного
    flag=1; //вывести на дисплей
  }
}

void loop() {
  if (flag==1) {
    disp.clear(); //очистить дисплей
    
    int cel_di=floor(DIST);  //целые
    int sot_di=(((float)cel_di/1000)-floor((float)cel_di/1000))*10;  //сотни
    int des_di=(((float)cel_di/100)-floor((float)cel_di/100))*10;  //десятки
    int ed_di=round(((float)((float)cel_di/10)-floor((float)cel_di/10))*10);  //единицы (с точкой)
    int dr_di=(float)(DIST-floor(DIST))*10;  //десятые части
    
    if (sot_di!=0) {
      disp.set(LED_0F[sot_di],3);  //вывод сотен расстояния
    }
    
    if (des_di!=0 || sot_di!=0) {
      disp.set(LED_0F[des_di],2);  //вывод десятков расстояния
    }
    
    disp.set(LED_0F[ed_di+10],1);  //вывод единиц расстояния, с точкой        
    disp.set(LED_0F[dr_di],0);  //вывод после точки  
    
    flag=0;  //опустить флаг
  }

  if (digitalRead(butt_pin)==1) {  //если кнопка нажата
    DIST=0;  //обнулить расстояние
    flag=1;  //вывести на дисплей
  }
delay(5);
}

void timerIsr()  //нужно для дисплея
{
  disp.timerIsr();
}

