#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Servo.h>

#define BUTTON_PIN 2
#define BUTTON_LIGHT_PIN 3
#define EYES_LIGHT_PIN 5
#define BUSY_PIN 13
#define ARM_SERVO_PIN 7

#define MUSIC 1
#define SOUND_EFFECTS 2

#define MUSIC_TRACKS_QTY 15
#define SOUND_EFFECTS_TRACKS_QTY 10

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
Servo armServo;

bool movingArmModeOn;

int busyState = 0;
int prevBusyState = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 0;

int buttonLightLevel = 0;
bool buttonLightAsc = true;
int buttonLightStep = 15;

int eyesLightLevel = 0;
bool eyesLightAsc = false;
int eyesLightStep = 2;

int armServosPositionDegrees = 90;
bool armServosPositionClockwise = true;
int armServosPositionStep = 1;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LIGHT_PIN, OUTPUT);
  pinMode(EYES_LIGHT_PIN, OUTPUT);
  pinMode(BUSY_PIN, INPUT_PULLUP);

  armServo.attach(ARM_SERVO_PIN);

  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println("Halloween 2022");

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1. Please recheck the connection!");
    Serial.println("2. Please insert the SD card!");
    while (true) {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  
  Serial.println("DFPlayer Mini online.");

  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms

  myDFPlayer.volume(20);  //Set volume value. From 0 to 30
  
  playRandomTrackInFolder(MUSIC, MUSIC_TRACKS_QTY);

  // initialise the BUSY states
  busyState = digitalRead(BUSY_PIN);
  prevBusyState = busyState;

  // Set servo to initial position
  movingArmModeOn = false;
  armServo.write(armServosPositionDegrees);
}

void loop() {
  int pushButton = digitalRead(BUTTON_PIN);
  if (pushButton == LOW) {
    eyesLightAsc = true;
    playRandomTrackInFolder(SOUND_EFFECTS, SOUND_EFFECTS_TRACKS_QTY);
    movingArmModeOn = true;
    delay(500);
  }

  int reading = digitalRead(BUSY_PIN);
  if (reading != prevBusyState) {
    lastDebounceTime = millis();    
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != busyState) {
      busyState = reading;
      
      if (busyState == HIGH) {
        eyesLightAsc = false;
        playRandomTrackInFolder(MUSIC, MUSIC_TRACKS_QTY);
      }
    }
  }
  prevBusyState = reading;
 
  updateButtonLight();
  updateEyesLight();
  updateArmServosPosition();

  delay(50);
}

void playRandomTrackInFolder(int folderNumber, int tracksQty) {
  int track = random(1, tracksQty + 1);
  
  Serial.print("Playing track ");
  Serial.print(track);
  Serial.print(" in folder ");
  Serial.print(folderNumber);
  Serial.println("...");
  myDFPlayer.playFolder(folderNumber, track);
}

void updateButtonLight() {
  int buttonLightLevelDelta = buttonLightAsc ? buttonLightStep : -buttonLightStep;
  buttonLightLevel += buttonLightLevelDelta;
  if (buttonLightLevel > 255) {
    buttonLightLevel = 255;
    buttonLightAsc = false;
  } else if (buttonLightLevel < 0) {
    buttonLightLevel = 0;
    buttonLightAsc = true;
  }
  
  analogWrite(BUTTON_LIGHT_PIN, buttonLightLevel);
}

void updateEyesLight() {
  int eyesLightLevelDelta = eyesLightAsc ? eyesLightStep : -eyesLightStep;
  eyesLightLevel += eyesLightLevelDelta;
  if (eyesLightLevel > 255) {
    eyesLightLevel = 255;
  } else if (eyesLightLevel < 0) {
    eyesLightLevel = 0;
  }
  
  analogWrite(EYES_LIGHT_PIN, eyesLightLevel);
}

void updateArmServosPosition() {
  if (!movingArmModeOn) {
    return;
  }
  
  int armServoPositionDegreesDelta = armServosPositionClockwise ? armServosPositionStep : -armServosPositionStep;
  armServosPositionDegrees += armServoPositionDegreesDelta;

  if (armServosPositionDegrees == 90 && !armServosPositionClockwise) {
    movingArmModeOn = false;
  }  

  if (armServosPositionDegrees > 130) {
    armServosPositionDegrees = 130;
    armServosPositionClockwise = false;
  } else if (armServosPositionDegrees < 50) {
    armServosPositionDegrees = 50;
    armServosPositionClockwise = true;
  }
  
  armServo.write(armServosPositionDegrees);
}
