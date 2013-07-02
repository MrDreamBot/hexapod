#include "Motors.h"

#define PWM_L P2_0
#define PWM_R P2_2
#define DIR_L P2_1
#define DIR_R P2_3

//#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__)
//  #define USE_20KHZ_PWM
//#endif

static boolean flipLeft = false;
static boolean flipRight = false;

// constructor (doesn't do anything)
Motors::Motors()
{
}

// initialize timer1 to generate the proper PWM outputs to the motor drivers
void Motors::init2()
{
  pinMode(PWM_L, OUTPUT);
  pinMode(PWM_R, OUTPUT);
  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);

//#ifdef USE_20KHZ_PWM
  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
//  TCCR1A = 0b10100000;
//  TCCR1B = 0b00010001;
//  ICR1 = 400;
//#endif
}

// enable/disable flipping of left motor
void Motors::flipLeftMotor(boolean flip)
{
  flipLeft = flip;
}

// enable/disable flipping of right motor
void Motors::flipRightMotor(boolean flip)
{
  flipRight = flip;
}

// set speed for left motor; speed is a number between -255 and 255
void Motors::setLeftSpeed(int speed)
{
  init(); // initialize if necessary
    
  boolean reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed; // make speed a positive quantity
    reverse = 1;    // preserve the direction
  }
  if (speed > 255)  // Max 
    speed = 255;
    
//#ifdef USE_20KHZ_PWM
//  OCR1B = speed;
//#else
//  analogWrite(PWM_L, speed); 
//#endif 

	if (reverse ^ flipLeft) // flip if speed was negative or flipLeft setting is active, but not both
    {
		digitalWrite(PWM_L, LOW);
		analogWrite(DIR_L, speed); 
	}
	else
	{
		digitalWrite(DIR_L, LOW);
		analogWrite(PWM_L, speed); 
	}
}

// set speed for right motor; speed is a number between -255 and 255
void Motors::setRightSpeed(int speed)
{
  init(); // initialize if necessary
    
  boolean reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (speed > 255)  // Max PWM dutycycle
    speed = 255;
    
//#ifdef USE_20KHZ_PWM
//  OCR1A = speed;
//#else
//  analogWrite(PWM_R, speed); 
//#endif

	if (reverse ^ flipRight) // flip if speed was negative or flipRight setting is active, but not both
	{
		digitalWrite(PWM_R, LOW);
		analogWrite(DIR_R, speed);
	}
	else
	{
		digitalWrite(DIR_R, LOW);
		analogWrite(PWM_R, speed);
	}
}

// set speed for both motors
void Motors::setSpeeds(int leftSpeed, int rightSpeed)
{
  setLeftSpeed(leftSpeed);
  setRightSpeed(rightSpeed);
}