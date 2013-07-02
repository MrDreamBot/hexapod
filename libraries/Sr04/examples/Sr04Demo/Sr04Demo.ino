#include <AbstractUltrasonics.h>
#include <Sr04.h>

Sr04 ultrasonic(PB_7,PB_6);

void setup() {
  Serial.begin(9600);
  Serial.println("testing...");
}

void loop()
{
  Serial.print(ultrasonic.getDistanceCm());
  Serial.println("cm");
    
  delay(1000);
}




