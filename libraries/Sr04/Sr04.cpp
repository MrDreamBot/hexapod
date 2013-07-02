#include "Sr04.h"

Sr04::Sr04(int triggerPin, int echoPin)
{
	this->triggerPin = triggerPin;
	this->echoPin = echoPin;
}

unsigned long Sr04::getPulseLength()
{
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin,HIGH);
}

