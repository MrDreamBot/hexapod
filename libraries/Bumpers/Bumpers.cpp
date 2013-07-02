#include "Bumpers.h"

Bumpers::Bumpers(int left_bumper_pin, int right_bumper_pin)
{
	this->left_bumper_pin = left_bumper_pin;
	this->right_bumper_pin = right_bumper_pin;
	pinMode(left_bumper_pin, INPUT_PULLUP);
	pinMode(right_bumper_pin, INPUT_PULLUP);
}

int Bumpers::is_left_bumper_hit()
{
	int reading = digitalRead(left_bumper_pin);
	if (reading == HIGH)
	{
		return false;
	}
	return true;
}

int Bumpers::is_right_bumper_hit()
{
	int reading = digitalRead(right_bumper_pin);
	if (reading == HIGH)
	{
		return false;
	}
	return true;
}
