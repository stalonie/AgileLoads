#include <IFX9201_XMC1300_StepperMotor.h>

#define DIR_PIN IFX9201_STEPPERMOTOR_STD_DIR		// Pin 9 is standard DIR Pin
#define STP_PIN IFX9201_STEPPERMOTOR_STD_STP		// Pin 10 is standard STP Pin
#define DIS_PIN IFX9201_STEPPERMOTOR_STD_DIS		// Pin 11 is standard DIS Pin

const int StepsPerRevolution = 200;  // change this to fit the total number of steps per revolution for your motor

// Stepper motor object
Stepper_motor MyStepperMotor = Stepper_motor(StepsPerRevolution, DIR_PIN, STP_PIN, DIS_PIN);

void setup() {

	// set pins' mode as OUTPUT, set default speed and enable the stepper motor
	MyStepperMotor.begin();

	// set the speed at 10 rpm:
	MyStepperMotor.setSpeed(100);

	// initialize the serial port:
	Serial.begin(9600);
}

void loop() {


	Serial.println("moves 90 degree in counterclockwise direction");
	MyStepperMotor.move_degree(360);
	delay(1000);


}
