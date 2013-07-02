#include "Bumpers.h"
#include "Motors.h"
#include "Sr04.h"
#include "Pushbutton.h"

// ***************** this sketch is for the MSP430 using the Energia Programming Env
// bumper-related pins
#define LEFT_BUMPER		P1_4
#define RIGHT_BUMPER	P1_5
#define LEFT_LED		P1_6	// lit when left bumper is hit
#define RIGHT_LED		P1_0	// lit when right bumper is hit

// motor pins are hard-coded in Motors, reproduced below for reference
#define PWM_L		P2_0
#define PWM_R		P2_2
#define DIR_L		P2_1
#define DIR_R		P2_3

// Sr04 related pins
#define TRIGGER		P2_4
#define ECHO		P2_5

// button
#define BUTTON		P1_3

// other constants
#define DEBUG					1
#define MIN_DISTANCE			10	// in CM, Sr04 considers an obstacle is in front
#define HIGH_SPEED				240	// between 0 and 255
//#define LOW_SPEED				190
#define ANGLE					60	// degrees: positive is counter-clockwise
#define HALF_ANGLE				(ANGLE / 2)
#define FRONT_WALL_TURN_ANGLE	60
//#define QUARTER_ANGLE			(ANGLE / 4)
#define WALL_FOLLOWING_ANGLE	30
#define CORRECTION_ANGLE		30
#define LEFT_BUMPER_TURN_ANGLE	100	// turn is left bumper is hit during wall_following_behaviour
#define LEFT_SPEED_REDUCTION	60	// experimentally determined to make robot walk in a straight line
#define RIGHT_SPEED_REDUCTION	0	// experimentally determined to make robot walk in a straight line
#define TIME_PER_DEGREE			14		// in milliseconds: experimentally determined to turn a robot
#define FULL_BACKOFF_TIME		3000	// in milliseconds
#define HALF_BACKOFF_TIME		(FULL_BACKOFF_TIME / 2)	// in milliseconds
#define MAX_NO_CONTACT_TIME		2000	// in milliseconds

// Objects used by robot
Bumpers bumpers(LEFT_BUMPER, RIGHT_BUMPER);
Motors motors;
Sr04 sr04(TRIGGER, ECHO);
Pushbutton button(BUTTON);

// robot state
enum State {
	FORWARD,
	BACKWARD,
	STOPPED
};

// variables used by the robot
int behaviour = 0;
boolean button_pressed = false;
enum State state = STOPPED;
int left_bumper = false;
int right_bumper = false;


// used only by wall_following_behaviour
unsigned long last_contact_time = 0;

// used only by wander_behaviour
//	

// ***************** utility routines
void delay_and_poll(unsigned long period)
{
	unsigned long timeout = millis() + period;
	while (millis() < timeout)
	{
		if (button.getSingleDebouncedRelease())
		{
			button_pressed = true;
		}
	}
}

// ***************** robot movement routines
void move_forward(int speed)
{
	/*
	digitalWrite(PWM_L, HIGH);
	digitalWrite(DIR_L, LOW);
	digitalWrite(PWM_R, HIGH);
	digitalWrite(DIR_R, LOW);
	*/
	// ram up
	for (int i = 2; i < speed ; i = i + 2 )
	{
		if (i <= (speed - LEFT_SPEED_REDUCTION))
		{
			motors.setLeftSpeed(i);
		}
		if (i <= (speed - RIGHT_SPEED_REDUCTION))
		{
			motors.setRightSpeed(i);
		}
		delay_and_poll(10);
	}

	state = FORWARD;
}

void move_backward(int speed)
{
	/*
	digitalWrite(PWM_L, LOW);
	digitalWrite(DIR_L, HIGH);
	digitalWrite(PWM_R, LOW);
	digitalWrite(DIR_R, HIGH);
	*/
	// ram up
	for (int i = 2; i < speed ; i = i + 2 )
	{
		if (i <= (speed - LEFT_SPEED_REDUCTION))
		{
			motors.setLeftSpeed(-i);
		}
		if (i <= (speed - RIGHT_SPEED_REDUCTION))
		{
			motors.setRightSpeed(-i);
		}
		delay_and_poll(10);
	}

	state = BACKWARD;
}

void stop()
{
	motors.setSpeeds(0, 0);
	state = STOPPED;
}

void turn(int angle)
{
	stop();
	int time_delay = abs(angle) * TIME_PER_DEGREE;
	if (angle > 0)
	{
		// turn left
		motors.setLeftSpeed(-(HIGH_SPEED - LEFT_SPEED_REDUCTION));
		motors.setRightSpeed(HIGH_SPEED - RIGHT_SPEED_REDUCTION);
	}
	else
	{
		// turn right
		motors.setLeftSpeed(HIGH_SPEED - LEFT_SPEED_REDUCTION);
		motors.setRightSpeed(-(HIGH_SPEED - RIGHT_SPEED_REDUCTION));
	}
	delay_and_poll(time_delay);
	stop();
}

void escape(int speed, int time_delay, int angle)
{
	// back up and turn
	stop();
	move_backward(speed);
	delay_and_poll(time_delay);
	turn(angle);
}

// ***************** robot behaviour routines
// read in the bumper states
void read_bumpers()
{
	// get bumper states
	left_bumper = bumpers.is_left_bumper_hit();
	right_bumper = bumpers.is_right_bumper_hit();

	// set LEDs accordingly
	digitalWrite(LEFT_LED, left_bumper);
	digitalWrite(RIGHT_LED, right_bumper);
}

// handle bumpers: returns true if actioned on bumper hit
boolean handle_bumpers()
{
	boolean handled = true;

	read_bumpers();
	if (left_bumper && right_bumper)
	{
		// randomly turn left or right after backing off
		escape(HIGH_SPEED, FULL_BACKOFF_TIME, ((random(100) > 40)? ANGLE: -ANGLE));
	}
	else if (left_bumper)
	{
		// back off and turn right
		stop();
		move_backward(HIGH_SPEED);
		delay_and_poll(HALF_BACKOFF_TIME);
		turn(-ANGLE);
	}
	else if (right_bumper)
	{
		// back off and turn left
		stop();
		move_backward(HIGH_SPEED);
		delay_and_poll(HALF_BACKOFF_TIME);
		turn(ANGLE);
	}
	else
	{
		handled = false;
	}
	return handled;
}


// handle sr04: returns true if actioned when sr04 detects an obstacle
boolean handle_sr04()
{
	int distance = sr04.get_distance_cm();
	if (distance <= MIN_DISTANCE)
	{
		// set LEDs 
	    digitalWrite(LEFT_LED, HIGH);
	    digitalWrite(RIGHT_LED, HIGH);

		// randomly turn left or right after backing off
		escape(HIGH_SPEED, HALF_BACKOFF_TIME, ((random(100) > 40)? ANGLE: -ANGLE));
		return true;
	}

	// clear LEDs
	digitalWrite(LEFT_LED, LOW);
	digitalWrite(RIGHT_LED, LOW);

	return false;
}

void wander_behaviour()
{
	// handle obstacle detection/action by bumpers
	if (!handle_bumpers())
	{
		// handle obstacle detection/action by Sr04
		if (!handle_sr04())
		{
			// all clear, wander forward
			if (state == STOPPED)
			{
				move_forward(HIGH_SPEED);
			}
		}

	}

}

// right wall follower
void wall_following_behaviour()
{
	read_bumpers();
	if (right_bumper)
	{

		if (sr04.get_distance_cm() <= MIN_DISTANCE)
		{
			// quick turn left 
			turn(FRONT_WALL_TURN_ANGLE);
			if (DEBUG)
			{
				Serial.println("RB & Sr04");
			}
		}
		else {
			// slow turn left
			turn(WALL_FOLLOWING_ANGLE);
			if (DEBUG)
			{
				Serial.println("RB & !Sr04");
			}
		}

	}
	else if (left_bumper)
	{
		// position to the the right bumper close to the wall
		escape(HIGH_SPEED, HALF_BACKOFF_TIME, LEFT_BUMPER_TURN_ANGLE);
		if (DEBUG)
		{
			Serial.println("LB");
		}

	}
	else 
	{
		if (sr04.get_distance_cm() <= MIN_DISTANCE)
		{
			
			// quick turn left
			turn(FRONT_WALL_TURN_ANGLE);
			if (DEBUG)
			{
				Serial.println("! RB & Sr04");
			}
		}
		else {
			// correct possible issue by further turning right to locate the wall
			if ((millis() - last_contact_time) > MAX_NO_CONTACT_TIME)
			{
				turn(-CORRECTION_ANGLE);
				if (DEBUG)
				{
					Serial.print(millis());
					Serial.println(": ! RB & ! Sr04 & Turn");
				}
			}
			else
			{
				if (DEBUG)
				{
					Serial.print(millis());
					Serial.println(": ! RB & ! Sr04");
				}
			}
			
		}
	}

	// move forward
	if (state == STOPPED)
	{

		move_forward(HIGH_SPEED);

		// all turns involves a stop() so reset last_contact_time
		last_contact_time = millis();

		if (DEBUG)
		{
			Serial.print(last_contact_time);
			Serial.println(": Moving forward");
		}
	}
}

// ***************** Arduino style sketch
// setup hardware resources not already set up by other classes
void setup() {
	pinMode(LEFT_LED, OUTPUT);
	pinMode(RIGHT_LED, OUTPUT);
	digitalWrite(LEFT_LED, LOW);
	digitalWrite(RIGHT_LED, LOW);
	randomSeed(millis());
	stop();

	if (DEBUG)
	{
		Serial.begin(9600);
	}
	
	// do not start until pushbutton is pressed
	button.waitForButton();
	last_contact_time = millis();
}

// set behaviour of robot
void loop()
{
	switch (behaviour)
	{
	case 0:
		wander_behaviour();
		break;

	case 1:
		wall_following_behaviour();
		break;

	case 2:
		stop();
		break;
/*
	case 3:
		// for testing only
		turn(90);
		delay_and_poll(10000);
		move_forward(HIGH_SPEED);
		delay_and_poll(10000);
		stop();
		delay_and_poll(10000);
		break;
*/
	default:
		break;
	
	}

	delay_and_poll(150);

	// button_pressed is monitored ans set by delay_and_poll()
	if (button_pressed)
	{
		// switch to next behaviour
		button_pressed = false;
		behaviour = (behaviour + 1) % 3;

		// reset LEDs
		digitalWrite(LEFT_LED, LOW);
		digitalWrite(RIGHT_LED, LOW);

		// stop for 1 second before changing behaviour
		stop();
		delay_and_poll(1000);

		// reset state for wall_following_behaviour
		last_contact_time = millis();

	}
}
