#include <Servo.h>

// تعريف أطراف حساس الألتراسونيك
const int trigPin = 11;
const int echoPin = 12;

// تعريف طرف السيرفو
const int servo = 10;
Servo servoMotor;

// تعريف أطراف التحكم في المواتير (L298N Motor Driver)
const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;

// متغيرات المسافة
float distance; 
float Ldistance = 0, Rdistance = 0;

// متغيرات الوقت
unsigned long startTime;
const unsigned long runTime = 180000; // 3 دقائق (180,000 ملي ثانية)

// متغير لحفظ حالة الاتجاه الحالي
String currentDirection = "Stop";

void setup() {
  // إعداد أطراف الحساس
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // إعداد أطراف المواتير
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // تفعيل السيرفو
  servoMotor.attach(servo);
  
  // تفعيل الاتصال التسلسلي (لعرض البيانات على الكمبيوتر)
  Serial.begin(9600);
  
  Serial.println("Smart Car is Ready!");
  Serial.println("---------------------------");
  
  // توجيه السيرفو للأمام في البداية
  servoMotor.write(90);
  delay(1000);

  // تسجيل وقت بداية التشغيل
  startTime = millis();
}

void loop() {
  // تشغيل نظام القيادة الذاتية
  amode();
}

// ----------------------------------------------------
// دوال التشغيل والتحكم (Functions)
// ----------------------------------------------------

void amode() {
  // التحقق من مرور 3 دقائق
  if(millis() - startTime >= runTime) { 
    stop(); 
    Serial.println("[Decision: TIME UP - CAR STOPPED]");
    return; 
  }
  
  // قراءة المسافة الأمامية
  distance = ultra();

  // لو في عائق على مسافة أقل من 35 سم
  if(distance < 35) {
    // طباعة القرار والمسافة
    Serial.print("Front Dist: "); 
    Serial.print(distance); 
    Serial.println(" cm -> [Decision: OBSTACLE! STOP & REVERSE]");
    
    stop();
    delay(200);
    reverse();
    delay(700);
    stop();
    delay(200);
    
    // مسح الطريق يمين وشمال (الدالة دي بتطبع القراءات جواها)
    scan(); 
  
    // اتخاذ قرار الدوران بناءً على المساحة الأكبر
    if(Rdistance > Ldistance) {
      Serial.println("[Decision: PATH CLEARED -> TURNING RIGHT]");
      turnRight();
      resetDistances();
    } 
    else if(Ldistance > Rdistance) {
      Serial.println("[Decision: PATH CLEARED -> TURNING LEFT]");
      turnLeft();
      resetDistances();
    }
    else {
      // لو المسافتين متساويين (للتأمين)
      Serial.println("[Decision: EQUAL DISTANCE -> DEFAULT TURN RIGHT]");
      turnRight();
      resetDistances();
    }
  } 
  else {
    // لو الطريق فاضي، كمل قدام
    Serial.print("Front Dist: "); 
    Serial.print(distance); 
    Serial.println(" cm -> [Decision: MOVING FORWARD]");
    Forward();
  }
  
  // خليت التأخير 100 ملي ثانية عشان القراءات متجريش بسرعة جداً على الشاشة وتلحق تقرأها
  delay(100); 
}

void scan() {
  Serial.println("--- Scanning Sides ---");
  
  // النظر لليمين
  servoMotor.write(0);
  delay(500);
  Rdistance = ultra();
  Serial.print("Right Dist: "); 
  Serial.print(Rdistance); 
  Serial.println(" cm");
  
  // النظر لليسار
  servoMotor.write(180);
  delay(500);
  Ldistance = ultra();
  Serial.print("Left Dist:  "); 
  Serial.print(Ldistance); 
  Serial.println(" cm");
  
  // العودة للأمام
  servoMotor.write(90);
  delay(500);
  
  Serial.println("----------------------");
}

float ultra() {
  long t;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  t = pulseIn(echoPin, HIGH);
  distance = t / 57.0;
  return distance;
}

void resetDistances() {
  Rdistance = 0;
  Ldistance = 0;
}

// ----------------------------------------------------
// دوال حركة المواتير
// ----------------------------------------------------

void Forward() {
   currentDirection = "Forward";
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
}

void turnLeft() {
   currentDirection = "Left";
   digitalWrite(IN4, LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN1, LOW);
   delay(150); 
}

void turnRight() {
   currentDirection = "Right";
   digitalWrite(IN4, HIGH);
   digitalWrite(IN3, LOW);
   digitalWrite(IN2, LOW);
   digitalWrite(IN1, HIGH);
   delay(150); 
}

void stop() {
   currentDirection = "Stop";
   digitalWrite(IN4, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN2, LOW);
   digitalWrite(IN1, LOW);
}

void reverse() {
   currentDirection = "Reverse";
   digitalWrite(IN4, LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN1, HIGH);
}