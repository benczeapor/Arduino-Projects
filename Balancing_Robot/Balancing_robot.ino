#include <SoftwareSerial.h>
#include <Wire.h>
#include <MPU6050_light.h>

#define MOTOR_ENB 3
#define MOTOR_ENA 5

#define MOTOR_IN1 6
#define MOTOR_IN2 7
#define MOTOR_IN3 8
#define MOTOR_IN4 9

short motorL = 0, motorR = 0;

MPU6050 mpu(Wire);

int angle, pwm, acc;

float Kp = 4.0;
float Kd = 2.5;
float Ki = 0.5;

float setPoint = 0.0;
float input, output;
float error, lastError;
float cumError, rateError;  

unsigned long currentTime, previousTime;
double elapsedTime;

float p, i, d;

void driveMotors()
{
  if(motorL < 0)
  {
    digitalWrite(MOTOR_IN1, 0);
    digitalWrite(MOTOR_IN2, 1);
  }
  else
  {
    digitalWrite(MOTOR_IN1, 1);
    digitalWrite(MOTOR_IN2, 0);
  }
  analogWrite(MOTOR_ENB, abs(motorL));

  if(motorR < 0)
  {
    digitalWrite(MOTOR_IN3, 1);
    digitalWrite(MOTOR_IN4, 0);
  }
  else
  {
    digitalWrite(MOTOR_IN3, 0);
    digitalWrite(MOTOR_IN4, 1);
  }
  analogWrite(MOTOR_ENA, abs(motorR));
}

// the pid control loop was constructed based on information available at https://wired.chillibasket.com/2015/03/pid-controller/
void computePid(float angle)
{
  currentTime = millis();
  elapsedTime = (double)(currentTime - previousTime) / 1000.0;

  input = angle;
  error = setPoint - input;

  cumError += error * elapsedTime;

  if(abs(output) >= 200) 
  {
    cumError = cumError - (error * elapsedTime);
  }

  cumError = constrain(cumError, -100, 100);

  rateError = (error - lastError) / elapsedTime;

  p = Kp * error * error * error;
  i = Ki * cumError;
  d = Kd * rateError;

  output = p + i + d;

  lastError = error;
  previousTime = currentTime;

  int motorSpeed = constrain(output, -200, 200);

  motorL = -motorSpeed;
  motorR = -motorSpeed;

  if(abs(angle) > 45)
  {
    motorL = 0;
    motorR = 0;
  }
}

void setup() 
{
  Wire.begin();
  mpu.begin();
  mpu.calcOffsets(true, false);

  delay(500);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
}

void loop() 
{
  mpu.update();
  angle = mpu.getAngleY();

  computePid(angle + 7.5);
  driveMotors();
}
