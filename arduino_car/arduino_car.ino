#include <Servo.h>
#include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// l298n motor drive pin 
const int IN1 = 2; 
const int IN2 = 3; 
const int IN3 = 4; 
const int IN4 = 5; 
// ultra sonic pin 
const int trig = 11;
const int echo = 12;
// Servo pin 
const int servo = 10;

Servo servoMotor;
// lcd 
// LiquidCrystal_I2C lcd(0x27, 16, 2);

float distance = 0, t = 0;
float Ldistance = 0, Rdistance = 0;

int mode = 0; // 0 = manual, 1 = auto

unsigned long startTime; // متغير لتخزين وقت بدء التشغيل
const unsigned long runTime = 180000; // )ميلي ثانية 180,000(دقائق 3مدة التشغيل
unsigned long lastDistanceSend = 0; // مؤقت لإرسال المسافة

String currentDirection = "Stop"; // متغير لتخزين الاتجاه الحالي

void setup()
{
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); 
  pinMode(IN4, OUTPUT);

  servoMotor.attach(servo);
  servoMotor.write(90);//ضبطالسرفوفيالمنتصف
  Serial.begin(9600);
  startTime=millis();//تسجيلوقتبدءالتشغيل

  // lcd.init();
  // lcd.backlight();
  // lcd.setCursor(0,0);
  // lcd.print("Smart Car");

  // lcd.setCursor(0,1);
  // lcd.print("Starting...");
  // delay(2000);
  // lcd.clear();


}
void loop()
{
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "A") mode = 1;
    else if (cmd == "M") mode = 0;

    if (mode == 0) {
      if (cmd == "F") Forward();
      else if (cmd == "B") reverse();
      else if (cmd == "L") turnLeft();
      else if (cmd == "R") turnRight();
      else if (cmd == "S") stop();
    }
  }

  // 👇 أهم سطر
  if (mode == 1) {
    amode();
  }


  // 3. إرسال المسافة والاتجاه كل ثانية للـ ESP32
 
  if (millis() - lastDistanceSend >= 1000) {
    float currentDist = ultra(); 
    
    // هنبعت الداتا بالشكل ده: Distance,Direction
    Serial.print(currentDist);
    Serial.print(",");
    Serial.println(currentDirection);
    
    lastDistanceSend = millis();
  }
  
}


void amode(){
   if(millis()-startTime>=runTime)// دقائق3التحققمنمرور
  {
  stop();// إيقافالسيارة
  // lcd.clear();
  // lcd.setCursor(0,0);
  // lcd.print("Time انتهى");
  return;// إنهاءالدالةloop
  }
  distance= ultra();

  // lcd.setCursor(0,0);
  // lcd.print("Dist: ");
  // lcd.print(distance);
  // lcd.print(" cm   ");

  if(distance<35)
  {
  //  lcd.setCursor(0,1);
  //  lcd.print("Obstacle!   ");
   stop();
   delay(200);
   reverse();
   delay(700);
   stop();
   delay(200);
   scan();
  
    if(Rdistance>Ldistance)
    {
    //  lcd.setCursor(0,1);
    //  lcd.print("Turning Right ");
    turnRight();
    resetDistances();
    }
    else if(Ldistance>Rdistance)
    {
    //  lcd.setCursor(0,1);
    //  lcd.print("Turning Left  ");
    turnLeft();
    resetDistances();
    }
    }
  else
  {
  //  lcd.setCursor(0,1);
  //  lcd.print("Forward       ");
   Forward();
  }
   delay(50);
}
void scan()
  {
  servoMotor.write(0);
  delay(500);
  Rdistance=ultra();
  servoMotor.write(180);
  delay(500);
  Ldistance=ultra();
  servoMotor.write(90);
  delay(500);
}
float ultra()
{
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  t =pulseIn(echo,HIGH);
  distance= t/57.0;
  return distance;
  }
void resetDistances()
{
  Rdistance=0;
  Ldistance=0;
}
void Forward()
{
   currentDirection = "Forward";
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
}
void turnLeft()
{
  currentDirection = "Left";
   digitalWrite(IN4, LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN1, LOW);
   delay(150);
}
void turnRight()
{
   currentDirection = "Right";
   digitalWrite(IN4, HIGH);
   digitalWrite(IN3, LOW);
   digitalWrite(IN2, LOW);
   digitalWrite(IN1, HIGH);
   delay(150);
}
void stop()
{
  currentDirection = "Stop";
   digitalWrite(IN4, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN2, LOW);
   digitalWrite(IN1, LOW);
}
void reverse()
{
  currentDirection = "Reverse";
   digitalWrite(IN4, LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN1, HIGH);
}