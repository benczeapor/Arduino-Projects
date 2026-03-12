#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Servo.h>

#define LED_PIN   10
#define SERVO_DEFAULT 90

RF24 radio(9, 8);
const byte address[10] = "ADDRESS01";

byte data[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
byte checksum = 0;
byte prev = 0;

Servo motor;
Servo rudder;
Servo elevator;
Servo leftAel;
Servo rightAel;

#define LED_INTERVAL 250

unsigned long prevM = 0;
unsigned long curr = 0;
bool ledState = false;

static int serial_putc(char c, FILE *stream)
{
  return Serial.write(c);
}

static FILE *serial_stream;

void setup() 
{
  Serial.begin(115200);
  serial_stream = fdevopen(serial_putc, NULL);
  stdout = serial_stream;
  while(!Serial)
  {

  }
  SPI.begin();
  Serial.print("Begin: ");
  Serial.println(radio.begin() ? "Success!" : "Fail!");
  
  radio.setCRCLength(RF24_CRC_16);
  radio.setChannel(108);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(7);
  radio.startListening();

  radio.printDetails();

  pinMode(LED_PIN, OUTPUT);

  motor.attach(A0, 1000, 2000);
  motor.write(0);
  elevator.attach(A1);
  elevator.write(SERVO_DEFAULT);
  rudder.attach(A2);
  rudder.write(SERVO_DEFAULT);
  leftAel.attach(5);
  leftAel.write(SERVO_DEFAULT);
  rightAel.attach(6); 
  rightAel.write(SERVO_DEFAULT);
}

void loop() 
{
  // Serial.println("lofasz")
  for(int i=0; i<7; i++)
  {
    data[i] = 0xFF;
  }
  checksum = 0;
  int sum = -1;

  // curr = millis();

  //   if(curr - prevM >= LED_INTERVAL)
  //   {
  //     ledState = !ledState;
  //     digitalWrite(LED_PIN, ledState);
  //     prevM = curr;
  //   }

  if (radio.failureDetected) 
  {
    Serial.println("Radio failure detected. Resetting...");
    radio.begin();
    radio.setCRCLength(RF24_CRC_16);
    radio.setChannel(108);
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setPayloadSize(7);
    radio.startListening();
    radio.failureDetected = false; // clear the flag  
  }


  if(radio.available())
  {

    

    radio.read(&data, sizeof(data));
    // radio.flush_rx();
    for(int i=0; i<6; i++)
    {
      checksum += data[i];
      sum += data[i];
      // Serial.print(data[i]);
      // Serial.print(", ");

    }
    // Serial.println(radio.failureDetected);

    // if(sum == -1)
    // {
    //   Serial.println("lofasz");
    // }
    if(checksum == data[6] && sum != -1)
    {
      // for(int i=0; i<5; i++)
      // {
      //   Serial.print(data[i]);
      //   Serial.print(", ");
      // }
      // Serial.println();

      motor.write(data[0]);
      elevator.write(data[1]);
      rudder.write(data[2]);
      leftAel.write(data[3]);
      rightAel.write(data[4]);
      digitalWrite(LED_PIN, data[5]);
    }
  }
}
