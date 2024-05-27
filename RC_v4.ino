
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "BluetoothSerial.h"

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
BluetoothSerial serialBT;

char btSignal;
int motor_max_spd = 255;

// Motor A connections
const int ENA = 14;
const int IN1 = 26;
const int IN2 = 27;

// Motor B connections
const int ENB = 5;
const int IN3 = 2;
const int IN4 = 4;

int T_Spd = 150;
int tolerance = 25;


int dur = 0;

int mode = 0;
int movement, angle, duration;
char *ptr;

void setup() {
  // Initialize motor pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(115200);

  // Initialize the magnetometer
  if (!mag.begin()) {
    // Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }

  if (!serialBT.begin("Steve's_ESP32")) {
    //Serial.println("An error occurred initializing Bluetooth");
  } else {
    // Serial.println("Bluetooth initialized successfully");
  }
}

void loop() {

  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Serial.print("Received from Raspberry Pi: ");
    Serial.println(data);
    char inputBuffer[data.length() + 1];  // +1 for null terminator
    data.toCharArray(inputBuffer, sizeof(inputBuffer));
    Serial.print("mode=");
    Serial.println(mode);
    Serial.print("movement=");
    Serial.println(movement);
    Serial.print("angle=");
    Serial.println(angle);
    Serial.print("duration=");
    Serial.println(duration);

    // Tokenize the string and parse integers
    ptr = strtok(inputBuffer, " ");
    mode = atoi(ptr);
    ptr = strtok(NULL, " ");
    movement = atoi(ptr);
    ptr = strtok(NULL, " ");
    angle = atoi(ptr);
    ptr = strtok(NULL, " ");
    duration = atoi(ptr);
    if (mode == 0) {
      Serial.println("bton");
      if (serialBT.available()) {
        // Read one character from the Bluetooth serial
        btSignal = serialBT.read();

        // Print the received character to the serial monitor
        Serial.print("Received: ");
        Serial.println(btSignal);

        if (btSignal == 'F') {
          moveForward();
        } else if (btSignal == 'S') {
          stopMotors();
        } else if (btSignal == 'B') {
          moveBackward();
        } else if (btSignal == 'R') {
          turnRight();
        } else if (btSignal == 'L') {
          turnLeft();
        } else if (btSignal == '9') {
          turn_angle(90);
        } else if (btSignal == '8') {
          turn_angle(-90);
        }
      }

    } else if (mode == 1) {
      //enter code for voice access
      Serial.println("vc mode activated");
      if (movement == 1) {
        moveForward_vca(duration);
      } else if (movement == -1) {
        moveBackward_VC(duration);
      } else {
        stopMotors();
      }
      if (angle > 0) {
        turn_angle(angle);
      }
    }
  } else {
    if (mode == 0) {
      if (serialBT.available()) {
        // Read one character from the Bluetooth serial
        btSignal = serialBT.read();
        Serial.println("bt mode1");
        // Print the received character to the serial monitor
        Serial.print("Received: ");
        Serial.println(btSignal);


        if (btSignal == 'F') {
          moveForward();
        } else if (btSignal == 'S') {
          stopMotors();
        } else if (btSignal == 'B') {
          moveBackward();
        } else if (btSignal == 'R') {
          turnRight();
        } else if (btSignal == 'L') {
          turnLeft();
        } else if (btSignal == '9') {
          turn_angle(90);
        } else if (btSignal == '8') {
          turn_angle(-90);
        }
      }
    }
  }
}

void moveForward() {
  Serial.println("Moving forward");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, motor_max_spd);  // Full speed

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, motor_max_spd);  // Full speed
}

void moveForward_vca(int dur) {
  if (dur == 0) {
    Serial.println("Moving forward via vc");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, motor_max_spd);  // Full speed

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, motor_max_spd);  // Full speed
  } else {
    Serial.println("Moving forward via vc with delay");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, motor_max_spd);  // Full speed

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, motor_max_spd);  // Full speed}
    delay(1000*dur);
    stopMotors();
  }
}

void moveBackward_VC(int dur) {
  Serial.println("movingbackward via vc");
  if (dur == 0) {
    moveBackward();
  } else {
    moveBackward();
    delay(dur*1000);
    stopMotors();
  }
}

void moveBackward() {
  Serial.println("Moving backward");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, motor_max_spd);  // Full speed

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, motor_max_spd);  // Full speed
}

void turnLeft() {
  Serial.println("Turning left");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, T_Spd);  // Adjust speed as needed

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, T_Spd);  // Adjust speed as needed
}

void turnRight() {
  Serial.println("Turning right");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, T_Spd);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, T_Spd);
}

void stopMotors() {
  Serial.println("Stopping motors");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}


void turn_angle(int TurnAngle) {
  int tolerance = 20;
  int currentAngle = mag_data();
  Serial.println(currentAngle);


  currentAngle = (currentAngle + 360) % 360;

  int targetAngle = currentAngle + TurnAngle;

  targetAngle = (targetAngle + 360) % 360;

  while (true) {
    currentAngle = mag_data();
    Serial.print("cr=");
    Serial.println(currentAngle);
    Serial.print("ta=");
    Serial.println(targetAngle);



    currentAngle = (currentAngle + 360) % 360;


    int angleDifference = targetAngle - currentAngle;


    if (angleDifference > 180) {
      angleDifference -= 360;
    } else if (angleDifference < -180) {
      angleDifference += 360;
    }


    if (abs(angleDifference) <= tolerance) {
      break;
    }


    if (angleDifference > 0) {
      turnLeft();
    } else {
      turnRight();
    }

    delay(5);
  }

  stopMotors();
}

int mag_data() {
  sensors_event_t event;
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);

  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;

  // Check for wrap due to addition of declination.
  if (heading > 2 * PI)
    heading -= 2 * PI;

  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180 / M_PI;
  int data = (int)headingDegrees;
  return data;
}
