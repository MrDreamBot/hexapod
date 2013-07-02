#ifndef Sr04_h
#define Sr04_h

#include <AbstractUltrasonics.h>

class Sr04: public AbstractUltrasonics
{
  public:
    Sr04(int triggerPin, int echoPin);
    unsigned long getPulseLength();

  private:
    int triggerPin;
    int echoPin;
    
};

#endif