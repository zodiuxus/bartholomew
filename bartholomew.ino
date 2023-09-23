/*
  IMPORTANT:
  This project uses some sort of Nano clone,
  so adjust your pins accordingly.

  GPIO PINS:
  D5-D13
  A0-A7
*/

int incData = 0;
float distance = 0;
bool enoughWater = false;

// Pin definitions and what they'll be used for
// 2 for ultrasonic distance sensor
#define TRIGGER_PIN 8
#define ECHO_PIN 7
#define MIN_DIST 10.0
#define MAX_DIST 50.0

// 1 for the pump power
#define PUMP 9

// The analog and 1 digital for the water level sensor
#define WATER_POWER 6
#define WATER_ANA A0

// And lastly, a water level warning LED (if the water level is too low it'll start blinking)
#define WATER_LED 5
#define WATER_MIN 0
#define WATER_MAX 1000
#define WATER_THRESHOLD 250 // gonna avoid using magic numbers for obvious reasons.

// Just to note, the pump has a volume of approx. 14cm^3,
// and as such it will displace ~14mL of water
// (or whatever other liquid you plan to use)

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(PUMP, OUTPUT);

  pinMode(WATER_POWER, OUTPUT);
  pinMode(WATER_LED, OUTPUT);
  digitalWrite(WATER_POWER, LOW);
}

// If there is enough water in the container, we turn off the LED and send a 1 (true) to the loop
bool waterLevel() {
  digitalWrite(WATER_POWER, HIGH);
  delay(10);
  int value = analogRead(WATER_ANA);
  digitalWrite(WATER_POWER, LOW);
  delay(100);
  if (value < WATER_THRESHOLD) {
    digitalWrite(WATER_LED, HIGH);
    return false;
  }
  else {
    digitalWrite(WATER_LED, LOW);
    return true;
  }
}

// Controls the ultrasonic distance sensor and returns the measurement
float dist() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  float duration = pulseIn(ECHO_PIN, HIGH);

  distance = (duration/2)*0.0343;
  delay(20);
  return distance;
}

void loop() {
  if (Serial.available() > 0) {
    incData = Serial.read();

    if(incData) {
      digitalWrite(LED_BUILTIN, HIGH);
      if((distance >= MIN_DIST && distance <= MAX_DIST) && enoughWater){
        digitalWrite(PUMP, HIGH);
        delay(750);
      }
      else {
        digitalWrite(PUMP, LOW);
      }
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  distance = dist();
  enoughWater = waterLevel();

  
  delay(500);
}
