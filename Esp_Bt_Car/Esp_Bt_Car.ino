#include <DabbleESP32.h>

#define RXD2 16
#define TXD2 17

String lastCommand = "";
bool autoMode = false;

void sendCommand(String cmd) {
  if (cmd != lastCommand) {
    Serial2.println(cmd);
    lastCommand = cmd;
  }
}

void setup() {
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Dabble.begin("MyBluetoothCar"); 
}

void loop() {
  Dabble.processInput();

  // 🔁 تغيير المود
  if (GamePad.isTrianglePressed()) {
    autoMode = true;
    sendCommand("A"); // Auto
    delay(300); // منع التكرار
  }

  if (GamePad.isCirclePressed()) {
    autoMode = false;
    sendCommand("M"); // Manual
    delay(300);
  }

  // 🎮 التحكم اليدوي
  if (!autoMode) {
    if (GamePad.isUpPressed()) {
      sendCommand("F");
    }
    else if (GamePad.isDownPressed()) {
      sendCommand("B");
    }
    else if (GamePad.isLeftPressed()) {
      sendCommand("L");
    }
    else if (GamePad.isRightPressed()) {
      sendCommand("R");
    }
    else if (GamePad.isSquarePressed()) {
      sendCommand("S");
    }
    else {
      sendCommand("S");
    }
  }

  delay(30);
}

// #include <DabbleESP32.h>
// #include <WiFi.h>
// #include <HTTPClient.h>

// #define RXD2 16
// #define TXD2 17

// const char* ssid = "WE9AF563";
// const char* password = "aa10fd01";

// const char* serverName = "http://192.168.1.5:8000/api/update";

// String lastCommand = "";
// bool autoMode = false;

// void sendCommand(String cmd) {
//   if (cmd != lastCommand) {
//     Serial2.println(cmd);
//     lastCommand = cmd;
//   }
// }

// void setup() {
//   Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
//   Dabble.begin("MyBluetoothCar"); 
//   WiFi.begin(ssid, password);
// }

// void loop() {
//   Dabble.processInput();

//   // 🔁 تغيير المود
//   if (GamePad.isTrianglePressed()) {
//     autoMode = true;
//     sendCommand("A"); // Auto
//     delay(300); // منع التكرار
//   }

//   if (GamePad.isCirclePressed()) {
//     autoMode = false;
//     sendCommand("M"); // Manual
//     delay(300);
//   }

//   // 🎮 التحكم اليدوي
//   if (!autoMode) {
//     if (GamePad.isUpPressed()) {
//       sendCommand("F");
//     }
//     else if (GamePad.isDownPressed()) {
//       sendCommand("B");
//     }
//     else if (GamePad.isLeftPressed()) {
//       sendCommand("L");
//     }
//     else if (GamePad.isRightPressed()) {
//       sendCommand("R");
//     }
//     else if (GamePad.isSquarePressed()) {
//       sendCommand("S");
//     }
//     else {
//       sendCommand("S");
//     }
//   }

//   delay(30);

//   if (Serial2.available() > 0) {
//       // قراءة السطر اللي باعه الأردوينو
//       String dataFromArduino = Serial2.readStringUntil('\n');
//       dataFromArduino.trim(); // تنظيف المسافات

//       // البحث عن مكان الفاصلة (,)
//       int commaIndex = dataFromArduino.indexOf(',');

//       // لو لقينا الفاصلة، معناها إن الداتا وصلت سليمة (مسافة + اتجاه)
//       if (commaIndex > 0) {
//         // فصل الداتا
//         String distanceVal = dataFromArduino.substring(0, commaIndex);
//         String directionVal = dataFromArduino.substring(commaIndex + 1);

//         Serial.println("Dist: " + distanceVal + " | Dir: " + directionVal);

//         // إرسال الداتا للواي فاي لو الشبكة متصلة
//         if (WiFi.status() == WL_CONNECTED) {
//           WiFiClient client;
//           HTTPClient http;

//           http.begin(client, serverName);
//           http.addHeader("Content-Type", "application/json");

//           char jsonBuffer[100];
//           sprintf(jsonBuffer, "{\"distance\":%s,\"direction\":\"%s\"}", distanceVal.c_str(), directionVal.c_str());

//           int httpResponseCode = http.POST(jsonBuffer);

//           http.end();
//         }
//       }
//     }
// }