#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Motor Driver Pins (L298N)
#define ENA 3  // Speed for Left Motor (PWM)
#define IN1 5  // Left Motor Forward
#define IN2 6  // Left Motor Backward
#define IN3 9  // Right Motor Forward
#define IN4 10 // Right Motor Backward
#define ENB 11 // Speed for Right Motor (PWM)

// Ultrasonic Sensor Pins
#define TRIG 7
#define ECHO 8
#define STOP_DISTANCE 10  // Stop if obstacle is closer than 10 cm
#define SLOW_DOWN_DISTANCE 20  // Slow down if within 20 cm

// Gesture Thresholds
const float forwardThreshold = -5.0;
const float backwardThreshold = 5.0;
const float leftThreshold = -5.0;
const float rightThreshold = 5.0;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    Serial.println("Initializing MPU6050...");
    if (!mpu.begin()) {
        Serial.println("MPU6050 connection failed! Check wiring.");
        while (1);
    }
    Serial.println("MPU6050 Ready!");

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
}

void loop() {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    float x = accel.acceleration.x;
    float y = accel.acceleration.y;
    int distance = getDistance();

    // Default Stop
    stopMotors();
    
    // Detect motion and move accordingly
    if (y < forwardThreshold && x < leftThreshold) {
        moveForwardLeft(distance);
    } else if (y < forwardThreshold && x > rightThreshold) {
        moveForwardRight(distance);
    } else if (y > backwardThreshold && x < leftThreshold) {
        moveBackwardLeft(distance);
    } else if (y > backwardThreshold && x > rightThreshold) {
        moveBackwardRight(distance);
    } else if (y < forwardThreshold) {
        moveForward(distance);
    } else if (y > backwardThreshold) {
        moveBackward(distance);
    } else if (x < leftThreshold) {
        turnLeft(distance);
    } else if (x > rightThreshold) {
        turnRight(distance);
    } else{
       stopMotors();  
    }

    delay(500);
}

// Ultrasonic Sensor Function
int getDistance() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH);
    int distance = duration * 0.034 / 2;  // Convert time to distance (cm)
    return distance;
}

// Movement functions with obstacle detection
void moveForward(int distance) {
    if (distance < STOP_DISTANCE) {
        Serial.println("Obstacle detected! Stopping.");
        stopMotors();
        return;
    }
    int speed = (distance < SLOW_DOWN_DISTANCE) ? map(distance, STOP_DISTANCE, SLOW_DOWN_DISTANCE, 50, 150) : 150;
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void moveBackward(int distance) {
   
    analogWrite(ENA, 150);
    analogWrite(ENB, 150);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void moveForwardLeft(int distance) {
    if (distance < STOP_DISTANCE) {
        stopMotors();
        return;
    }
    analogWrite(ENA, 150);
    analogWrite(ENB, 0);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void moveForwardRight(int distance) {
    if (distance < STOP_DISTANCE) {
        stopMotors();
        return;
    }
    analogWrite(ENA, 0);
    analogWrite(ENB, 150);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void moveBackwardLeft(int distance) {
    analogWrite(ENA, 150);
    analogWrite(ENB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void moveBackwardRight(int distance) {
    analogWrite(ENA, 0);
    analogWrite(ENB, 150);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void turnLeft(int distance) {
    if (distance < STOP_DISTANCE) {
        stopMotors();
        return;
    }
    analogWrite(ENA, 150);
    analogWrite(ENB, 0);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void turnRight(int distance) {
    if (distance < STOP_DISTANCE) {
        stopMotors();
        return;
    }
    analogWrite(ENA, 0);
    analogWrite(ENB, 150);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

// Stop motors
void stopMotors() {
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}
