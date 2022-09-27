#include <CurieBLE.h>
#include <Servo.h>
#include <IFX9201_XMC1300_StepperMotor.h>

#define DIR_PIN_LEFT 3
#define STP_PIN_LEFT 4
#define DIS_PIN_LEFT 5

#define DIR_PIN_RIGHT 9
#define STP_PIN_RIGHT 10
#define DIS_PIN_RIGHT 11

#define SPRAY_PIN 8

const int StepsPerRevolution = 200;

enum MoveDirection
{
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

MoveDirection moveDirection = STOP;
bool isSpray = false;
bool isToggleSpray = false;

int servoAngle = 30;
int servoPos = servoAngle;    // variable to store the servo position

int topSpeed = 50;
int currentSpeed = 1;

// Stepper motor object
Stepper_motor leftStepperMotorö = Stepper_motor(StepsPerRevolution, DIR_PIN_LEFT, STP_PIN_LEFT, DIS_PIN_LEFT);     // left stepper motor
Stepper_motor rightStepperMotor = Stepper_motor(StepsPerRevolution, DIR_PIN_RIGHT, STP_PIN_RIGHT, DIS_PIN_RIGHT); // right stepper motor

BLEService moveService("19B10000-E8F2-537E-4F6C-D104768A1215"); // create service

Servo sprayServo;

// create switch characteristic and allow remote device to read and write
BLECharCharacteristic switchChar("19B10001-E8F2-537E-4F6C-D104768A1215", BLERead | BLEWrite);

void setup()
{
  setupStepperMotors(1);

  sprayServo.attach(SPRAY_PIN);
  sprayServo.write(servoPos);

  // begin initialization
  BLE.begin();

  // set the local name peripheral advertises
  BLE.setLocalName("MARK-ROBOTICS");
  // set the UUID for the service this peripheral advertises
  BLE.setAdvertisedService(moveService);

  // add the characteristic to the service
  moveService.addCharacteristic(switchChar);

  // add service
  BLE.addService(moveService);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  switchChar.setEventHandler(BLEWritten, switchCharacteristicWritten);
  // set an initial value for the characteristic
  switchChar.setValue(0);

  // start advertising
  BLE.advertise();

  // initialize the serial port:
  Serial.begin(9600);

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{
  // poll for BLE events
  BLE.poll();

  switch (moveDirection)
  {
  case FORWARD:
    moveForward(50);
    break;
  case BACKWARD:
    moveBackward(50);
    break;
  case LEFT:
    moveLeft(50, 1 / 5);
    break;
  case RIGHT:
    moveRight(50, 1 / 5);
    break;
  default:
    currentSpeed = 1;
    break;
  }

  if(isToggleSpray){
    isToggleSpray = false;
    toggleSpray();
  }
}

void setupStepperMotors(int initialSpeed)
{
  leftStepperMotor.begin();
  rightStepperMotor.begin();

  // speed 100 = 10 rpm:
  setSpeed(initialSpeed);
}

void blePeripheralConnectHandler(BLEDevice central)
{
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central)
{
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic)
{

  int command = switchChar.value();

  // central wrote new value to characteristic
  Serial.print("Characteristic event, written: ");
  Serial.println(command);
  
  switch (command)
  {
  case 1:
    moveDirection = FORWARD;
    break;
  case 2:
    moveDirection = BACKWARD;
    break;
  case 3:
    moveDirection = LEFT;
    break;
  case 4:
    moveDirection = RIGHT;
    break;
  case 5:
    isToggleSpray = true;
    break;
  case 6:
    topSpeed = topSpeed + 20;
    setSpeed(topSpeed);
    break;
  case 7:
    topSpeed = topSpeed - 20;
    setSpeed(topSpeed);
    break;
  case 0: moveDirection = STOP;
    break;
  }
}

void setSpeed(int speed){
  leftStepperMotor.setSpeed(speed);
  rightStepperMotor.setSpeed(speed);
}

void accelerate(){
  if (currentSpeed < topSpeed){
    currentSpeed = currentSpeed + 1;
    setSpeed(currentSpeed);
  }
}

void moveForward(int stepsAmount)
{
  for (int i = 0; i <= stepsAmount; i++)
  {
    leftStepperMotor.step(-1);
    rightStepperMotor.step(1);

    accelerate();
  }
}

void moveBackward(int stepsAmount)
{
  for (int i = 0; i <= stepsAmount; i++)
  {
    leftStepperMotor.step(1);
    rightStepperMotor.step(-1);

    accelerate();
  }
}

void moveLeft(int stepsAmount, double degreeRation)
{
  for (int i = 0; i <= stepsAmount; i++)
  {
    leftStepperMotor.step(-int(1 / degreeRation));
    rightStepperMotor.step(1);
  }
}

void moveRight(int stepsAmount, double degreeRation)
{
  for (int i = 0; i <= stepsAmount; i++)
  {
    leftStepperMotor.step(-1);
    rightStepperMotor.step(int(1 / degreeRation));
  }
}

void toggleSpray()
{
  if (isSpray == false)
  {
    Serial.println("start spray");
    for (servoPos = 30; servoPos >= 0; servoPos -= 1) 
    {                                  
      sprayServo.write(servoPos);    
      delay(15);       
    }
    isSpray = true;
  }
  else
  {
    Serial.println("stop spray");
    for (servoPos = 0; servoPos <= 30; servoPos += 1)
    {
      sprayServo.write(servoPos);
      delay(15);
    }
    isSpray = false;
  }
}
