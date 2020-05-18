const float CPLANE_BORDER = 1023/2;
const float MAX_TURN_LOSS_FACTOR = 1.0;

const float X_AXIS_THRESHOLD_FACTOR = 0.25;
const float Y_AXIS_THRESHOLD_FACTOR = 0.25;

const float X_THRESHOLD = X_AXIS_THRESHOLD_FACTOR * CPLANE_BORDER;
const float Y_THRESHOLD = Y_AXIS_THRESHOLD_FACTOR * CPLANE_BORDER;

const float FULL_SPEED = 255.0;
const int FORWARD = HIGH;
const int BACKWARD = LOW;

const int NUMBER_OF_FIELDS = 7;
int fieldIndex = 0;
int values[NUMBER_OF_FIELDS];

const int MOTOR_A_BRAKE_PIN = 9;
const int MOTOR_B_BRAKE_PIN = 8;
const int MOTOR_A_DIRECTION_PIN = 12;
const int MOTOR_B_DIRECTION_PIN = 13;
const int MOTOR_A_SPEED_PIN = 3;
const int MOTOR_B_SPEED_PIN = 11;

unsigned long lastSerialAvailable = 0;
const unsigned long NO_SERIAL_AVAILABLE_INTERVAL = 50;

int sign = 1;

void setup() {
  Serial.begin(9600);

  // Setup Channel A
  pinMode(MOTOR_A_DIRECTION_PIN, OUTPUT);   // Initiates Motor Channel A pin
  pinMode(MOTOR_A_BRAKE_PIN, OUTPUT);       // Initiates Brake Channel A pin
  digitalWrite(MOTOR_A_BRAKE_PIN, LOW);     // Disengage the Brake for Channel A

  // Setup Channel B
  pinMode(MOTOR_B_DIRECTION_PIN, OUTPUT);   // Initiates Motor Channel B pin
  pinMode(MOTOR_B_BRAKE_PIN, OUTPUT);       // Initiates Brake Channel B pin
  digitalWrite(MOTOR_B_BRAKE_PIN, LOW);     // Disengage the Brake for Channel B
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
      
      driveMotorShield(x, y);
      
      for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
        values[i] = 0;
      }
      
      fieldIndex = 0;
    }
  } else if (millis() - lastSerialAvailable > NO_SERIAL_AVAILABLE_INTERVAL) {
    driveMotorShield(0, 0);
  }
}

void driveMotorShield(int x, int y) {
  int ls = 0;
  int rs = 0;

  // At center: No motion
  if (abs(x) < X_THRESHOLD && abs(y) < Y_THRESHOLD) {
    analogWrite(MOTOR_A_SPEED_PIN, 0);
    analogWrite(MOTOR_B_SPEED_PIN, 0);

    return;
  }

  // On x-band: Full turn (left/right)
  if (abs(x) >= X_THRESHOLD && abs(y) < Y_THRESHOLD) {
    float full_turn_speed = FULL_SPEED * x / CPLANE_BORDER;
    ls = full_turn_speed;
    rs = -full_turn_speed;
  } else { // Not at center or x-band
    float default_speed = FULL_SPEED * y / CPLANE_BORDER;

    float turn_factor = 1;
    if (x >= 0) {
      turn_factor -= MAX_TURN_LOSS_FACTOR * x / CPLANE_BORDER;
    } else {
      turn_factor += MAX_TURN_LOSS_FACTOR * x / CPLANE_BORDER;
    }

    if (x >= 0) {
      ls = default_speed;
      rs = turn_factor * default_speed;
    } else {
      ls = turn_factor * default_speed;
      rs = default_speed;
    }
  }

  boolean ld = ls < 0 ? BACKWARD : FORWARD;
  boolean rd = rs < 0 ? BACKWARD : FORWARD;

  ls = abs(ls);
  rs = abs(rs);

  // Motor A
  digitalWrite(MOTOR_A_DIRECTION_PIN, rd); // Establishes direction of Channel A
  analogWrite(MOTOR_A_SPEED_PIN, rs);      // Spins the motor on Channel A

  // Motor B
  digitalWrite(MOTOR_B_DIRECTION_PIN, ld);  // Establishes direction of Channel B
  analogWrite(MOTOR_B_SPEED_PIN, ls);       // Spins the motor on Channel B
}