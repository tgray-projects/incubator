#include <math.h>

#define STATE_HEATING 0
#define STATE_COOLING 1
#define STATE_NONE 2

const int temperaturePin = 0;
const int heaterPin = 3;
const float minTemp = 37.5;  // target 37.5
const float maxTemp = 38.9;

// Arduino pins tied to motor relay inputs
const int exhaustFanA = 4; // Motor A input a
const int exhaustFanB = 5; // Motor A input b
const int heatFanA = 6; // Motor B input a
const int heatFanB = 7; // Motor B input b

byte max_speed = 255; // set speed to maximum

int state = STATE_NONE;
float medTemp;

// from https://arduinomodules.info/ky-013-analog-temperature-sensor-module/
// thermistor board connected with ground and power swapped, per comments, for accurate reading
double thermister(int RawADC) {
  double temp;
  temp = log(((10240000/RawADC) - 10000));
  temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp ))* temp );
  temp = temp - 273.15; // Convert Kelvin to Celcius
  return temp;
}

void exhaustFanOff() {
  Serial.println("exhaust fan off");
  analogWrite(exhaustFanA, 0);
  analogWrite(exhaustFanB, 0);
}

void exhaustFanOn() {
  Serial.println("exhaust fan on");
  analogWrite(exhaustFanA, 0);
  analogWrite(exhaustFanB, max_speed); 
}

void heatFanOff() {
  Serial.println("heat fan off");
  analogWrite(heatFanA, 0);
  analogWrite(heatFanB, 0); 
}

void heatFanOn() {
  Serial.println("heat fan on");
  analogWrite(heatFanA, 0);
  analogWrite(heatFanB, max_speed); 
}

void heaterOn() {
  Serial.println("heater on");
  digitalWrite(heaterPin, HIGH);
}

void heaterOff() {
  Serial.println("heater off");
  digitalWrite(heaterPin, LOW);
}

void heat() {
  exhaustFanOff();
  heatFanOn();
  heaterOn();
}

void cool() {
  heatFanOff();
  heaterOff();
  exhaustFanOn();
}

void setup() {
  // initialize motor pins
  pinMode(exhaustFanA, OUTPUT);
  pinMode(exhaustFanB, OUTPUT);
  pinMode(heatFanA, OUTPUT);
  pinMode(heatFanB, OUTPUT);

  pinMode(heaterPin, OUTPUT);
  
  Serial.begin(9600);

  medTemp = (minTemp + maxTemp)/2;
}

void loop() {
  double celcius;
  celcius = thermister(analogRead(temperaturePin));
  Serial.print(celcius); //read pin A0
  Serial.println("c");
  
  switch(state) {
    case STATE_NONE:
      heaterOff();
      heatFanOn();
      exhaustFanOff();
      if (celcius < medTemp) {
        state=STATE_HEATING;
        Serial.println("Flame on!");
        heat();
      } else if (celcius > maxTemp) {
        state=STATE_COOLING;
        Serial.println("Chill out!");
        cool();
      } else {
        // we're good.
      }
      break;
    case STATE_HEATING:
      if (celcius > maxTemp) {
        state=STATE_NONE;
      }
      break;
    case STATE_COOLING:
      // exhaust on
      if (celcius < medTemp) {
        state=STATE_NONE;
      }
      break;
    default:
      Serial.println("Weird state.");
  }
  
  delay(2000);
}
