#include <ezBuzzer.h>
#include <SimpleRelay.h>
#include <CountDown.h>
#include <StopWatch.h>
#include <Wire.h>
#include "MMA7660.h"
#include "DS3231.h"
#include "SdFat.h"

#define relay_pin 11
#define buzzer_pin 12

// ? List of G forces
float intensity[10] = {0, 0, 0, 0, 0, 0.115, 0.215, 0.401, 0.747, 1.39};

// ? Indicates how long the system waits before interrupting the circuit
const unsigned int time_limit[10] = {0, 0, 0, 0, 0, 10, 7, 3, 1, 0.5};

// ? Indicates how long the program rings the alarm and keep the circuit interrupted
const unsigned int cooldown[10] = {0, 0, 0, 0, 0, 10, 15, 20, 25, 30};

float maxRecordedAcceleration = 0.0F; // G
int currentIntensity = 0;

MMA7660 accelerometer;
SimpleRelay relay = SimpleRelay(relay_pin, true);
ezBuzzer buzzer(buzzer_pin);
StopWatch timer(StopWatch::SECONDS);
CountDown countdown(CountDown::SECONDS);
DS3231 rtc(SDA, SCL);
SdFat sdCard;
SdFile sdFile;

void setup()
{
  Serial.begin(9600);
  accelerometer.init();
  rtc.begin();
  sdCard.begin();
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

  getCurrentIntensity();

  // ! Serial.print(String(maxRecordedAcceleration, 3));
  Serial.print(currentIntensity);
  Serial.print(",");

  if (countdown.isRunning())
  {
    Serial.println("10");
    buzzer.beep(2000);
    recordValues(1);
  }
  else
  {
    if (maxRecordedAcceleration > intensity[5] && timer.isReset())
    {
      timer.start();
      buzzer.beep(2000);
      Serial.println("0");
      recordValues(0);
    }
    else if (maxRecordedAcceleration > intensity[5] && timer.isRunning())
    {
      if (timer.elapsed() > time_limit[currentIntensity] && maxRecordedAcceleration >= intensity[currentIntensity])
      {
        timer.reset();
        countdown.start(cooldown[currentIntensity]);
        maxRecordedAcceleration = 0;
        relay.off();
        Serial.println("10");
        recordValues(1);
      }
      Serial.println("0");
      recordValues(0);
    }
    else if (maxRecordedAcceleration < intensity[5] && timer.isReset())
    {
      Serial.println("-10");
      currentIntensity = 0;
      relay.on();
      recordValues(-1);
    }
  }
}

void getCurrentIntensity()
{
  for (int i = 5; i <= 9; i++)
  {
    if (maxRecordedAcceleration >= intensity[i])
    {
      currentIntensity = i;
    }
  }
}

void recordValues(int circuitState)
{
  String date = rtc.getDateStr();
  String logFile = String(String(date) + ".log.txt");

  if (sdFile.open(logFile.c_str(), O_CREAT | O_WRITE | O_APPEND))
  {
    sdFile.print(rtc.getTimeStr());
    sdFile.print(",");
    sdFile.print(String(currentIntensity));
    sdFile.print(",");
    sdFile.print(String(maxRecordedAcceleration));
    sdFile.print(",");
    sdFile.println(String(circuitState));
    sdFile.close();
  }
}