#include <HX711.h>
 
HX711 scale;
HX711 scale2;
HX711 scale3;
HX711 scale4;

float calibration_factor = 417390;
float calibration_factor2 = 438320;
float calibration_factor3 = 428128;
float calibration_factor4 = 446500;

unsigned long startTime; // Variable to store the start time
unsigned long elapsedTime; // Total time lapsed
bool sendingScaleData = false; // When receiving signal from Java -> start sending data

unsigned long previous = 0;
const long interval = 100;

long packageNo = 0;

// Pin definitions
const int scale1_DT = 3;
const int scale1_SCK = 2;
const int scale2_DT = 5;
const int scale2_SCK = 4;
const int scale3_DT = 7;
const int scale3_SCK = 6;
const int scale4_DT = 9;
const int scale4_SCK = 8;

// String to store the connection status of each scale
String connectionStatus = "";
 
//=============================================================================================
//                         SETUP
//=============================================================================================
void setup() {
  Serial.begin(9600);
  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");

  // Initialize scale 1
  pinMode(scale1_DT, INPUT_PULLUP);
  if (digitalRead(scale1_DT) == LOW) {
    connectionStatus += "1";  // Scale 1 connected
  } else {
    connectionStatus += "0";  // Scale 1 not connected
  }

  // Initialize scale 2
  pinMode(scale2_DT, INPUT_PULLUP);
  if (digitalRead(scale2_DT) == LOW) {
    connectionStatus += "1";  // Scale 2 connected
  } else {
    connectionStatus += "0";  // Scale 2 not connected
  }

  // Initialize scale 3
  pinMode(scale3_DT, INPUT_PULLUP);
  if (digitalRead(scale3_DT) == LOW) {
    connectionStatus += "1";  // Scale 3 connected
  } else {
    connectionStatus += "0";  // Scale 3 not connected
  }

  // Initialize scale 4
  pinMode(scale4_DT, INPUT_PULLUP);
  if (digitalRead(scale4_DT) == LOW) {
    connectionStatus += "1";  // Scale 4 connected
  } else {
    connectionStatus += "0";  // Scale 4 not connected
  }

  // print connection status (i.e. 1100 if scale 1 and 2 are connected)
  Serial.println(connectionStatus);

  scale.begin(3, 2); // (DT, SCK)
  scale.set_scale();
  scale.tare(); // Reset value to 0

  scale2.begin(5, 4); // (DT, SCK)
  scale2.set_scale();
  scale2.tare(); // Reset value to 0

  scale3.begin(7, 6); // (DT, SCK)
  scale3.set_scale();
  scale3.tare(); // Reset value to 0

  scale4.begin(9, 8); // (DT, SCK)
  scale4.set_scale();
  scale4.tare(); // Reset value to 0
 
  // long zero_factor = scale.read_average(); //đọc thông tin
  // long zero_factor2 = scale2.read_average(); //đọc thông tin scale 2
  // long zero_factor3 = scale3.read_average(); //đọc thông tin scale 3
  // long zero_factor4 = scale4.read_average(); //đọc thông tin scale 4

}
 
//=============================================================================================
//                         LOOP
//=============================================================================================
void loop() {
 
  scale.set_scale(calibration_factor); // Adjust the scale based on calibration factor
  scale2.set_scale(calibration_factor2);
  scale3.set_scale(calibration_factor3);
  scale4.set_scale(calibration_factor4);

  // delay(100);
 
  // Listener
  if(Serial.available())
  {
    char temp = Serial.read();
    if (temp == 's') {
      startTime = millis();
      sendingScaleData = true;
      Serial.println("Begin sending data.");
    } else if (temp == 'e') {
      sendingScaleData = false;
      elapsedTime = millis() - startTime;
      Serial.println("Stop sending data.");
      Serial.print("Elapsed Time: ");
      Serial.print(elapsedTime / 1000);  // Print time in seconds
      Serial.println(" seconds");
      Serial.print("Total packages sent: ");
      Serial.println(packageNo);
      packageNo = 0;
    }

  }

  if (sendingScaleData) {
    unsigned long currentMillis = millis();

    if (currentMillis - previous >= interval) {
      previous = currentMillis;
      
      unsigned long totalSeconds = currentMillis / 1000;
      unsigned long hours = totalSeconds / 3600;
      unsigned long minutes = (totalSeconds % 3600) / 60;
      unsigned long seconds = totalSeconds % 60;
      unsigned long milliseconds = currentMillis % 1000;

      packageNo++;

      if (hours < 10) Serial.print("0");
      Serial.print(hours);
      Serial.print(":");
      if (minutes < 10) Serial.print("0");
      Serial.print(minutes);
      Serial.print(":");
      if (seconds < 10) Serial.print("0");
      Serial.print(seconds);
      Serial.print(".");
      if (milliseconds < 100) Serial.print("0");
      if (milliseconds < 10) Serial.print("0");
      Serial.print(milliseconds);

      Serial.print(";");

      Serial.print(packageNo);
      Serial.print(";");

      float reading = scale.get_units();
      if (abs(reading) < 0.001) { // Threshold for noise
        reading = 0.0;
      }
      Serial.print(reading, 3);
      Serial.print(";");

      float reading2 = scale2.get_units();
      if (abs(reading2) < 0.001) { // Threshold for noise
        reading2 = 0.0;
      }
      Serial.print(reading2, 3);
      Serial.print(";");

      float reading3 = scale3.get_units();
      if (abs(reading3) < 0.001) { // Threshold for noise
        reading3 = 0.0;
      }
      Serial.print(reading3, 3);
      Serial.print(";");

      float reading4 = scale4.get_units();
      if (abs(reading4) < 0.001) { // Threshold for noise
        reading4 = 0.0;
      }
      Serial.print(reading4, 3);

      Serial.println();
    }
  }
}