//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__HARDSERIAL

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 9600
#define REMOTEXY_BLUETOOTH_NAME "HopefullyCanGoUK"

#include <RemoteXY.h>
#include <Wire.h>
#include <PCA9685.h>
#include <Servo.h>

#define M1in 10
#define M1out 11
#define M2in 12
#define M2out 13
#define M3in 6
#define M3out 7
#define M4in 8
#define M4out 9
#define M5in 2
#define M5out 3
#define M6in 4
#define M6out 5

Servo servo1;
Servo servo2;
PCA9685 driver;
// PCA9685 outputs = 12-bit = 4096 steps
// 3.75% of 20ms = 0.75ms ; 6.25% of 20ms = 1.25ms
// 3.75% of 4096 = 153.6 steps; 6.25% of 4096 = 256 steps
PCA9685_ServoEval pwmServo(162, 256); // (-70 deg, +70deg)

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 61 bytes
  { 255,5,0,0,0,54,0,17,0,0,0,31,1,200,84,1,1,3,0,5,
  16,12,51,51,32,2,26,31,5,132,11,54,54,32,2,26,31,2,78,53,
  44,22,0,2,26,31,31,77,111,100,101,32,49,0,77,111,100,101,32,50,
  0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  int8_t joystick_02_x; // from -100 to 100
  int8_t joystick_02_y; // from -100 to 100
  uint8_t switch_01; // =1 if switch ON and =0 if OFF

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

void setup() 
{
  RemoteXY_Init ();
  Serial.begin(9600);
  Wire.begin();                 // Wire must be started first
  Wire.setClock(400000);        // Supported baud rates are 100kHz, 400kHz, and 1000kHz
  driver.resetDevices();        // Software resets all PCA9685 devices on Wire line

  driver.init();         // Address pins A5-A0 set to B000000
  driver.setPWMFrequency(50);   // Set frequency to 50Hz 
  
  pinMode(M1in, OUTPUT);
  pinMode(M1out, OUTPUT);
  pinMode(M2in, OUTPUT);
  pinMode(M2out, OUTPUT);
  pinMode(M3in, OUTPUT);
  pinMode(M3out, OUTPUT);
  pinMode(M4in, OUTPUT);
  pinMode(M4out, OUTPUT);
  pinMode(M5in, OUTPUT);
  pinMode(M5out, OUTPUT);
  pinMode(M6in, OUTPUT);
  pinMode(M6out, OUTPUT);
}

void Reverse()
{  // Set initial rotation direction
  digitalWrite(M1in, LOW);
  digitalWrite(M1out, HIGH);
  digitalWrite(M2in, LOW);
  digitalWrite(M2out, HIGH);
  digitalWrite(M3in, LOW);
  digitalWrite(M3out, HIGH);
  digitalWrite(M4in, LOW);
  digitalWrite(M4out, HIGH);
  digitalWrite(M5in, LOW);
  digitalWrite(M5out, HIGH);
  digitalWrite(M6in, LOW);
  digitalWrite(M6out, HIGH);
}
void Forward()
{
  digitalWrite(M1in, HIGH);
  digitalWrite(M1out, LOW);
  digitalWrite(M2in, HIGH);
  digitalWrite(M2out, LOW);
  digitalWrite(M3in, HIGH);
  digitalWrite(M3out, LOW);
  digitalWrite(M4in, HIGH);
  digitalWrite(M4out, LOW);
  digitalWrite(M5in, HIGH);
  digitalWrite(M5out, LOW);
  digitalWrite(M6in, HIGH);
  digitalWrite(M6out, LOW);
}

void loop() 
{ RemoteXY_Handler ();

  if (RemoteXY.switch_01!=0) {
  // Map joystick values to motor speed
    int motorSpeed = map(RemoteXY.joystick_02_y, -100, 100, -1023, 1023);
    
  // Set motor speed and direction
    if (RemoteXY.joystick_02_y > 0) {
      Forward();
    } else if (RemoteXY.joystick_02_y < 0) {
      Reverse();
  }
    // Write motor speed to enable pin
    driver.setChannelPWM(0, abs(motorSpeed));
    driver.setChannelPWM(1, abs(motorSpeed));
    driver.setChannelPWM(2, abs(motorSpeed));
    driver.setChannelPWM(3, abs(motorSpeed));
    driver.setChannelPWM(4, abs(motorSpeed));
    driver.setChannelPWM(5, abs(motorSpeed));

    int servopos1 = map(RemoteXY.joystick_01_x, -100, 100, 512,-512);
  

    // Set servo positions
    driver.setChannelPWM(8, pwmServo.pwmForAngle(servopos1));
    driver.setChannelPWM(9, pwmServo.pwmForAngle(servopos1));
    
  } else {
    // Map joystick values to motor speed
  int motorSpeedL = map(RemoteXY.joystick_01_y, -100, 100, -1023, 1023);
  int motorSpeedR = map(RemoteXY.joystick_02_y, -100, 100, -1023, 1023);

  // Set motor speed and direction
  if (RemoteXY.joystick_02_y > 0) {
    Forward();
  } else if (RemoteXY.joystick_02_y < 0) {
    Reverse();
  }
  
  if (RemoteXY.joystick_01_y < 0) {
    Reverse();
  } else if (RemoteXY.joystick_01_y > 0) {
   Forward();
  }
  
  // Write motor speed to enable pin
  driver.setChannelPWM(1, abs(motorSpeedL));
  driver.setChannelPWM(2, abs(motorSpeedL));
  driver.setChannelPWM(5, abs(motorSpeedL));
  driver.setChannelPWM(0,abs(motorSpeedR));
  driver.setChannelPWM(4, abs(motorSpeedR));
  driver.setChannelPWM(3, abs(motorSpeedR));
  }

}