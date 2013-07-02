#include "Bumpers.h"


#define LEFT_BUMPER		P1_4
#define RIGHT_BUMPER	P1_5
#define LEFT_LED		P1_0
#define RIGHT_LED		P1_6

Bumpers bumpers(LEFT_BUMPER, RIGHT_BUMPER);

void setup() {
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
}

void loop()
{
  digitalWrite(LEFT_LED, bumpers.is_left_bumper_hit());
  digitalWrite(RIGHT_LED, bumpers.is_right_bumper_hit());

}




