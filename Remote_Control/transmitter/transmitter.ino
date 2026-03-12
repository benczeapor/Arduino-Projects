#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define LED_PIN   4
#define POT       A0
#define LY        A1
#define LX        A2
#define RY        A3
#define RX        A4

#define SERVO_CENTER 90
#define RUDDER_RANGE 35
#define ELEVATOR_RANGE 25
#define AELERON_RANGE 20

#define LED_INTERVAL 250

unsigned long prev = 0;
unsigned long curr = 0;
bool led_state = false;

RF24 radio(9, 8);
const byte address[10] = "ADDRESS01";

byte data[7];
byte checksum = 0;

byte throttle = 0;
byte rudder = 0;
byte elevator = 0;
byte leftAel = 0;
byte rightAel = 0;

static int serial_putc(char c, FILE *stream)
{
  return Serial.write(c);
}

static FILE *serial_stream;

void setup() 
{
  Serial.begin(9600);
  serial_stream = fdevopen(serial_putc, NULL);
  stdout = serial_stream;
  while(!Serial)
  {

  }
  Serial.print("Begin: ");
  Serial.println(radio.begin() ? "Success!" : "Fail!");
  // radio.printDetails();
  radio.setCRCLength(RF24_CRC_16);
  radio.setChannel(108);
  radio.setRetries(15, 10);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(7);
  radio.stopListening();

  pinMode(POT, INPUT);
  pinMode(LX, INPUT);
  pinMode(LY, INPUT);
  pinMode(RX, INPUT);
  pinMode(RY, INPUT);
  pinMode(LED_PIN, OUTPUT);

  for(int i=0; i<6; i++)
  {
    data[i] = 0;
  }

  // digitalWrite(LED_PIN, 1);

}

void loop() 
{
  
  throttle = map(analogRead(POT), 0, 1023, 0, 180);
  rudder = map(analogRead(LX), 0, 1023, SERVO_CENTER + RUDDER_RANGE, SERVO_CENTER - RUDDER_RANGE);
  elevator = map(analogRead(RY), 0, 1023, SERVO_CENTER - ELEVATOR_RANGE, SERVO_CENTER + ELEVATOR_RANGE);
  int aeleronStick = analogRead(RX);
  leftAel = map(aeleronStick, 0, 1023, SERVO_CENTER + AELERON_RANGE, SERVO_CENTER - AELERON_RANGE);
  rightAel = map(aeleronStick, 0, 1023, SERVO_CENTER + AELERON_RANGE, SERVO_CENTER - AELERON_RANGE);
  Serial.print((int)leftAel);
  Serial.print(", ");
  Serial.println((int)rightAel);
  
  curr = millis();
  // Serial.println(led_state);
  if(curr - prev >= LED_INTERVAL)
  {
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state);
    prev = curr;
  }

  data[0] = throttle;
  data[1] = elevator;
  data[2] = rudder;
  data[3] = leftAel;
  data[4] = rightAel;
  data[5] = led_state;

  checksum = 0;

  for(int i=0; i<6; i++)
  {
    checksum += data[i];
  }

  data[6] = checksum;
  radio.write(&data, sizeof(data));

  delay(5);
}
