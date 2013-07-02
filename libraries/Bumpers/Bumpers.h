#ifndef bumpers_h
#define bumpers_h

#include "Energia.h"

class Bumpers
{
  public:
    Bumpers(int left_bumper_pin, int right_bumper_pin);
    int is_left_bumper_hit();
    int is_right_bumper_hit();

  private:
    int left_bumper_pin;
    int right_bumper_pin;
    
};

#endif