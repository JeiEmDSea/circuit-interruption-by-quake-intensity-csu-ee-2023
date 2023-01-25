#include <ezBuzzer.h>
#include <SimpleRelay.h>
#include <CountDown.h>
#include <StopWatch.h>
#include <Wire.h>
#include "MMA7660.h"

#define relay_pin 11
#define buzzer_pin 12

#define int5_G_force 0.115 // G
#define int6_G_force 0.215 // G
#define int7_G_force 0.401 // G
#define int8_G_force 0.747 // G
#define int9_G_force 1.39  // G

unsigned int int5_time_limit = 10; // seconds
unsigned int int6_time_limit = 7;  // seconds
unsigned int int7_time_limit = 3;  // seconds
unsigned int int8_time_limit = 1;  // seconds

unsigned int int5_cooldown = 10; // seconds
unsigned int int6_cooldown = 15; // seconds
unsigned int int7_cooldown = 20; // seconds
unsigned int int8_cooldown = 25; // seconds

float maxRecordedAcceleration = 0; // G

MMA7660 accelerometer;
SimpleRelay relay = SimpleRelay(relay_pin, true);
ezBuzzer buzzer(buzzer_pin);
StopWatch timer(StopWatch::SECONDS);
CountDown cooldown(CountDown::SECONDS);

void setup()
{
  Serial.begin(9600);
  accelerometer.init();
  relay.off();
  Serial.println("intensity,state");
}

void loop()
{
  buzzer.loop();

  float ax, ay, az;
  accelerometer.getAcceleration(&ax, &ay, &az);

  int counter = 1;

  do
  {
    maxRecordedAcceleration = maxRecordedAcceleration < ax ? ax : maxRecordedAcceleration;
    maxRecordedAcceleration = maxRecordedAcceleration < ay ? ay : maxRecordedAcceleration;
    delay(25);
    counter++;
  } while (counter < 10);
  Serial.print(String(maxRecordedAcceleration, 3) + ",");

  if (cooldown.isRunning())
  {
    Serial.println("1");
    buzzer.beep(2000);
  }
  else
  {
    if (maxRecordedAcceleration > int5_G_force && timer.isReset())
    {
      timer.start();
      Serial.println("0");
    }
    else if (maxRecordedAcceleration > int5_G_force && timer.isRunning())
    {
      if (intensityIsOverTimeLimit())
      {
        timer.reset();
        cooldown.start(getCooldownTime(maxRecordedAcceleration));
        maxRecordedAcceleration = 0;
        relay.off();
        Serial.println("1");
      }
      Serial.println("0");
    }
    else if (maxRecordedAcceleration < int5_G_force && timer.isReset())
    {
      Serial.println("-1");
      relay.on();
    }
  }
}

unsigned int getCooldownTime(float val)
{
  if (val >= int5_G_force && val < int6_G_force)
  {
    return int5_cooldown;
  }
  else if (val >= int6_G_force && val < int7_G_force)
  {
    return int6_cooldown;
  }
  else if (val >= int7_G_force && val < int8_G_force)
  {
    return int7_cooldown;
  }
  else if (val >= int8_G_force)
  {
    return int8_cooldown;
  }
}

bool intensityIsOverTimeLimit()
{
  return timer.elapsed() > int8_time_limit && maxRecordedAcceleration >= int8_G_force
    || timer.elapsed() > int7_time_limit && maxRecordedAcceleration >= int7_G_force
    || timer.elapsed() > int6_time_limit && maxRecordedAcceleration >= int6_G_force
    || timer.elapsed() > int5_time_limit && maxRecordedAcceleration >= int5_G_force;
}