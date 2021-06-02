const float JOYSTICK_BORDER = 1023/2;
const float CPLANE_BORDER = 1023/2;

const float X_AXIS_THRESHOLD_FACTOR = 0.25;
const float Y_AXIS_THRESHOLD_FACTOR = 0.25;

const float X_THRESHOLD = X_AXIS_THRESHOLD_FACTOR * JOYSTICK_BORDER;
const float Y_THRESHOLD = Y_AXIS_THRESHOLD_FACTOR * JOYSTICK_BORDER;

const byte PIN_ANALOG_X = 0;
const byte PIN_ANALOG_Y = 1;
const byte PIN_BUTTON_SELECT = 2;
const byte PIN_BUTTON_RIGHT = 3;
const byte PIN_BUTTON_UP = 4;
const byte PIN_BUTTON_DOWN = 5;
const byte PIN_BUTTON_LEFT = 6;

void setup() {
  Serial.begin(115200);
  
  while (!Serial) { // Arduino Leonardo
    ;
  }

  pinMode(PIN_BUTTON_SELECT, INPUT);
  digitalWrite(PIN_BUTTON_SELECT, HIGH);

  pinMode(PIN_BUTTON_RIGHT, INPUT);
  digitalWrite(PIN_BUTTON_RIGHT, HIGH);

  pinMode(PIN_BUTTON_UP, INPUT);
  digitalWrite(PIN_BUTTON_UP, HIGH);
 
  pinMode(PIN_BUTTON_DOWN, INPUT);
  digitalWrite(PIN_BUTTON_DOWN, HIGH);
 
  pinMode(PIN_BUTTON_LEFT, INPUT);
  digitalWrite(PIN_BUTTON_LEFT, HIGH);
}

void loop() {
  int jx = analogRead(PIN_ANALOG_X);
  int jy = analogRead(PIN_ANALOG_Y);

  int x = CPLANE_BORDER * (jx / JOYSTICK_BORDER - 1);
  int y = CPLANE_BORDER * (jy / JOYSTICK_BORDER - 1);

  int select = digitalRead(PIN_BUTTON_SELECT);
  int right = digitalRead(PIN_BUTTON_RIGHT);
  int up = digitalRead(PIN_BUTTON_UP);
  int down = digitalRead(PIN_BUTTON_DOWN);
  int left = digitalRead(PIN_BUTTON_LEFT);

  if (abs(x) > X_THRESHOLD || abs(y) > Y_THRESHOLD ||
    select == LOW || right == LOW || up == LOW || down == LOW || left == LOW) {
    sendSerialData(x, y, select, right, up, down, left);
  }

  delay(50);
}

void sendSerialData(int x, int y, int select, int right, int up, int down, int left) {
  Serial.print(x);
  Serial.print(',');
  Serial.print(y);
  Serial.print(',');
  Serial.print(select);
  Serial.print(',');
  Serial.print(right);
  Serial.print(',');
  Serial.print(up);
  Serial.print(',');
  Serial.print(down);
  Serial.print(',');
  Serial.print(left);
  Serial.print('\n');
}
