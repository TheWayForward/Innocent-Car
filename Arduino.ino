#include <IRremote.h>
#include <Servo.h>
int RECV_PIN = A3;
IRrecv irrecv(RECV_PIN);
decode_results results;
int on = 0;//标志位
unsigned long last = millis();
int mode;
int Echo = A5;
int Trig = A4;
int Front_Distance = 0;
int Left_Distance = 0;
int Right_Distance = 0;

long run_car = 16718055;
long back_car = 16730805;
long left_car = 16716015;
long right_car = 16734885;
long stop_car = 16726215;
long left_turn = 16724175;
long right_turn = 16743045;
int exitit = 16750965;
int Left_motor_back = 5;
int Left_motor_go = 9;
int Right_motor_go = 10;
int Right_motor_back = 6;
int servopin = 2;
int myangle;
int pulsewidth;
int val;



void setup()
{
  pinMode(Left_motor_go,OUTPUT);
  pinMode(Left_motor_back,OUTPUT);
  pinMode(Right_motor_go,OUTPUT);
  pinMode(Right_motor_back,OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
  pinMode(servopin, OUTPUT);
  Serial.begin(9600);  //波特率9600
  irrecv.enableIRIn(); 
  Serial.println("Fine.");
}

float Distance_test()
{
  digitalWrite(Trig,LOW);
  delayMicroseconds(2);
  digitalWrite(Trig,HIGH);
  delayMicroseconds(2);
  float Fdistance = pulseIn(Echo, HIGH);
  Fdistance = Fdistance/58;
  return Fdistance;  
  }

void servopulse(int servopin, int myangle)
{
  pulsewidth = (myangle*11)+500;
  digitalWrite(servopin,HIGH);
  delayMicroseconds(pulsewidth);
  digitalWrite(servopin,LOW);
  delay(20-pulsewidth/1000);
  }

void front_detection()
{
  for(int i=0;i<=10;i++)
  {
    servopulse(servopin,80);  
  }  
   Front_Distance = Distance_test();
  
}

void left_detection()
{
  for(int i=0;i<=15;i++)
  {
    servopulse(servopin,195);  
  }  
  Left_Distance = Distance_test();
}

void right_detection()
{
  for(int i=0;i<=15;i++)
  {
  servopulse(servopin,15);  
  }  
  Right_Distance = Distance_test();
}

void forward(int time)
{
  digitalWrite(Right_motor_go,HIGH);
  digitalWrite(Right_motor_back,LOW);
  digitalWrite(Left_motor_go,HIGH);
  digitalWrite(Left_motor_back,LOW);
  Serial.println("gogogo");
  delay(time*100);
}

void brakeit(int time)
{
  digitalWrite(Right_motor_go,LOW);
  digitalWrite(Right_motor_back,LOW);
  digitalWrite(Left_motor_go,LOW);
  digitalWrite(Left_motor_back,LOW);
  Serial.println("Halt");
  delay(time*100);
}

void left(int time)
{
  digitalWrite(Right_motor_go,HIGH);
  digitalWrite(Right_motor_back,LOW);
  digitalWrite(Left_motor_go,LOW);
  digitalWrite(Left_motor_back,LOW);
  Serial.println("turn left");
  delay(time*100);
  
}

void right(int time)
{
  digitalWrite(Right_motor_go,LOW);
  digitalWrite(Right_motor_back,LOW);
  digitalWrite(Left_motor_go,HIGH);
  digitalWrite(Left_motor_back,LOW);
  Serial.println("turn right");
  delay(time*100);
}

void spin_left(int time)
{
  digitalWrite(Right_motor_go,HIGH);
  digitalWrite(Right_motor_back,LOW);
  digitalWrite(Left_motor_go,LOW);
  digitalWrite(Left_motor_back,HIGH);
  Serial.println("spin left");
  delay(time*100);
  
}

void spin_right(int time)
{
  digitalWrite(Right_motor_go,LOW);
  digitalWrite(Right_motor_back,HIGH);
  digitalWrite(Left_motor_go,HIGH);
  digitalWrite(Left_motor_back,LOW);
  Serial.println("spin right");
  delay(time*100);
}

void back(int time)
{
  digitalWrite(Right_motor_go,LOW);
  digitalWrite(Right_motor_back,HIGH);
  digitalWrite(Left_motor_go,LOW);
  digitalWrite(Left_motor_back,HIGH);  
  Serial.println("backwards");
  delay(time*100);
}

void loop()
{
  
  if (irrecv.decode(&results)) //调用库函数：解码
  {
   
    if (millis() - last > 250) //确定接收到信号
    {
      on = !on;//标志位置反
      digitalWrite(13, on ? HIGH : LOW);//板子上接收到信号闪烁一下led
      Serial.println(results.value);
      
      if(results.value==16769565)
      {
        mode=1;
        Serial.println(mode);  
      }
      if(results.value==16720605)
      {
        mode=0;
        Serial.println(mode);  
      }
    }
   if(mode==1)
   {
     int mode1;
     Serial.println("Detection Mode");
     front_detection();
     if(Front_Distance<64)
     {
      Serial.println("Blocked");
      back(2);
      brakeit(2);
      left_detection();
      right_detection();
      if(Left_Distance < 35 && Right_Distance < 35)
      spin_left(0.7);
      else if(Left_Distance > Right_Distance)
      {
        Serial.println("blocked");
        left(3);
        brakeit(1);
       }
       else
       {
       Serial.println("blocked");
       right(3);
       brakeit(1); 
       }
      }
      else if(Front_Distance > 16)
      {
        Serial.println("Go straight.");
        forward(5);
        }
     irrecv.resume();
     }

   
   else if(mode==0)
   {
     brakeit(0.01);
     int mode2;
     Serial.println("remote mode"); 
     irrecv.decode(&results);
     if(results.value == run_car)
     forward(1);
     if(results.value == back_car)
     back(1);
     if(results.value == left_car)
     left(1);
     if(results.value == right_car)
     right(1);
     if(results.value == left_turn)
     spin_left(1);
     if(results.value == right_turn)
     spin_right(1);
     if(results.value == stop_car)
     brakeit(1);
     irrecv.resume();
     
}
}

  else
  {
    if(mode==1)
   {
     int mode1;
     Serial.println("Detection Mode");
     front_detection();
     if(Front_Distance<48)
     {
      Serial.println("Blocked");
      back(2);
      brakeit(2);
      left_detection();
      right_detection();
      if(Left_Distance < 35 && Right_Distance < 35)
      spin_left(0.7);
      else if(Left_Distance > Right_Distance)
      {
        Serial.println("blocked");
        left(3);
        brakeit(1);
       }
       else
       {
       Serial.println("blocked");
       right(3);
       brakeit(1); 
       }
      }
      else if(Front_Distance > 16)
      {
        Serial.println("Go straight.");
        forward(5);
        }
     irrecv.resume();
     }

   
   else if(mode==0)
   {
     brakeit(0.01);
     int mode2;
     Serial.println("remote mode"); 
     irrecv.decode(&results);
     if(results.value == run_car)
     forward(1);
     if(results.value == back_car)
     back(1);
     if(results.value == left_car)
     left(1);
     if(results.value == right_car)
     right(1);
     if(results.value == left_turn)
     spin_left(1);
     if(results.value == right_turn)
     spin_right(1);
     if(results.value == stop_car)
     brakeit(1);
     irrecv.resume();
     
}
delay(1000);
}

}
