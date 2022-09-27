#include <IFX9201_XMC1300_StepperMotor.h>

#define DIR_PIN_LEFT 3
#define STP_PIN_LEFT 4
#define DIS_PIN_LEFT 5

#define DIR_PIN_RIGHT 9
#define STP_PIN_RIGHT 10
#define DIS_PIN_RIGHT 11

bool IS_CONFIGURE_LEFT_MOTOR = false; 

const int StepsPerRevolution = 200;  // change this to fit the total number of steps per revolution for your motor

#if defined(XMC4700_Relax_Kit)
#define CONFIG_SERIAL Serial1
#else
#define CONFIG_SERIAL Serial
#endif

// Stepper motor
#if (IS_CONFIGURE_LEFT_MOTOR)
  Stepper_motor stepperMotor = Stepper_motor(StepsPerRevolution, DIR_PIN_LEFT, STP_PIN_LEFT, DIS_PIN_LEFT);       // left stepper motor 
#else 
  Stepper_motor stepperMotor = Stepper_motor(StepsPerRevolution, DIR_PIN_RIGHT, STP_PIN_RIGHT, DIS_PIN_RIGHT);    // right stepper motor 
#endif

IFX9201_STEPPERMOTOR_config_t stepper_motor_config =
{
	.SteppingpMode = IFX9201_STEPPERMOTOR_STEPPINGMODE_HALF,
	.FreqPWMOut = 20000u,
	.PWMDutyCycleNormFactor = 2000u,
	.NumMicrosteps = 64,
	.Store = IFX9201_STEPPERMOTOR_STEPPINGMODE_DO_NOT_STORE_CONFIG
};

void setup() {
	// Signal Led
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
	bool success = false;
	IFX9201_STEPPERMOTOR_config_t config_read_back;

	// Apply config via serial interface
	stepperMotor.configure(CONFIG_SERIAL, &stepper_motor_config);
  
	// Add a short delay to give the �C time to apply the config
	delay(100);

	// Read back written config
	stepperMotor.configRead(CONFIG_SERIAL, &config_read_back);

	if( (stepper_motor_config.SteppingpMode == config_read_back.SteppingpMode) && \
			 (stepper_motor_config.FreqPWMOut == config_read_back.FreqPWMOut) && \
			 (stepper_motor_config.PWMDutyCycleNormFactor == config_read_back.PWMDutyCycleNormFactor) && \
			 (stepper_motor_config.NumMicrosteps == config_read_back.NumMicrosteps) && \
			 (stepper_motor_config.Store == config_read_back.Store) )
	{
		// Configuration succeeded
		success = true;
		digitalWrite(LED_BUILTIN, HIGH);
		while(1);
	}

	delay(1000);
}
