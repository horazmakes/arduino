// serialCatapult.ino
#include <Servo.h>

Servo triggerServo;

const float CPLANE_BORDER = 1023/2;

const float Y_AXIS_THRESHOLD_FACTOR = 0.25;

const float Y_THRESHOLD = Y_AXIS_THRESHOLD_FACTOR * CPLANE_BORDER;

const float FULL_SPEED = 255.0;
const int FORWARD = HIGH;
const int BACKWARD = LOW;

const int NUMBER_OF_FIELDS = 7;
int fieldIndex = 0;
int values[NUMBER_OF_FIELDS];

const int MOTOR_A_BRAKE_PIN = 9;
const int MOTOR_A_DIRECTION_PIN = 12;
const int MOTOR_A_SPEED_PIN = 3;

const int TRIGGER_SERVO_PIN = 5;

unsigned long lastSerialAvailable = 0;
const unsigned long NO_SERIAL_AVAILABLE_INTERVAL = 50;

int sign = 1;

const int TRIGGER_LOCK_POSITION = 0;
const int TRIGGER_RELEASE_POSITION = 90;

int triggerServoDegrees = TRIGGER_LOCK_POSITION;

void setup() {
  Serial.begin(9600);

  triggerServo.attach(TRIGGER_SERVO_PIN);
  triggerServo.write(triggerServoDegrees);
  delay(100);
  triggerServo.detach();
  
  // Setup Channel A
  pinMode(MOTOR_A_DIRECTION_PIN, OUTPUT);   // Initiates Motor Channel A pin
  pinMode(MOTOR_A_BRAKE_PIN, OUTPUT);       // Initiates Brake Channel A pin
  digitalWrite(MOTOR_A_BRAKE_PIN, LOW);     // Disengage the Brake for Channel A
}

void loop() {
  if (Serial.available() > 0) {
    lastSerialAvailable = millis();

    char ch = Serial.read();
    
    if (ch == '-') {
      sign = -1;
    } else if (ch >= '0' && ch <= '9') {
      values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0');
    } else if (ch == ',') {
      values[fieldIndex] = values[fieldIndex] * sign;
      sign = 1;
      if (fieldIndex < NUMBER_OF_FIELDS - 1) {
        fieldIndex++;
      }
    } else if (ch == '\n') {
      int x = values[0];
      int y = values[1];
      int select = values[2];
      int right = values[3];
      int up = values[4];
      int down = values[5];
      int left = values[6];

      if (up == 0) {
        releaseTrigger();
      } else if (down == 0) {
        lockTrigger();
      }
      
      driveMotorShield(y);
      
      for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
        values[i] = 0;
      }
      
      fieldIndex = 0;
    }
  } else if (millis() - lastSerialAvailable > NO_SERIAL_AVAILABLE_INTERVAL) {
    driveMotorShield(0);
  }
}

void driveMotorShield(int y) {
  // At center: No motion
  if (abs(y) < Y_THRESHOLD) {
    analogWrite(MOTOR_A_SPEED_PIN, 0);

    return;
  }

  float s = FULL_SPEED * y / CPLANE_BORDER;

  boolean d = s < 0 ? BACKWARD : FORWARD;

  s = abs(s);

  // Motor A
  digitalWrite(MOTOR_A_DIRECTION_PIN, d); // Establishes direction of Channel A
  analogWrite(MOTOR_A_SPEED_PIN, s);      // Spins the motor on Channel A
}

void releaseTrigger() {
  triggerServo.attach(TRIGGER_SERVO_PIN);
  
  for (; triggerServoDegrees < TRIGGER_RELEASE_POSITION; triggerServoDegrees += 1) {
    triggerServo.write(triggerServoDegrees);
    delay(5);
  }
  
  triggerServo.detach();
}

void lockTrigger() {
  triggerServo.attach(TRIGGER_SERVO_PIN);
  
  for (; triggerServoDegrees > TRIGGER_LOCK_POSITION; triggerServoDegrees -= 1) {
    triggerServo.write(triggerServoDegrees);
    delay(5);
  }
  
  triggerServo.detach();
}
