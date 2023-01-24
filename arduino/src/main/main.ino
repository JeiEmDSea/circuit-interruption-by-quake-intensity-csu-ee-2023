#define relay 11  // relay pin
#define buzzer 12 // buzzer pin

/*variables*/
const int xpin = A0;
const int ypin = A1;
const int zpin = A2;
const int SmootherN = 10; // <<** Set within range 10 to 100

float alpha; // Calculate in Setup()
float beta;
float xlastAvg = 0;
float ylastAvg = 0;
float zlastAvg = 0;
int xsample = 0;
int ysample = 0;
int zsample = 0;
int COUNTER = 0; // <<***** use to ignore 1st N calculations
float xg;
float yg;
float zg;
int n = 0;
long start;

unsigned long movementStarted;
unsigned long stableStarted;
unsigned long delaystable;
unsigned long currentMillis;
/*Macros*/

#define maxValint5 0.580
#define minValint5 0.329
#define minValint6 0.580
#define maxValint6 0.773
#define maxValint7 1.380
#define minValint7 0.773
#define maxValint8 1.380
#define nmaxValint5 -.092
#define nminValint5 -0.329
#define nminValint6 -0.580
#define nmaxValint6 -0.773
#define nmaxValint7 -1.380
#define nminValint7 -0.773
#define nminValint8 -1.380

unsigned long int5 = 10000;
unsigned long int6 = 7000;
unsigned long int7 = 3000;
unsigned long int8 = 1000;

unsigned long period = 5000;
bool moving = false;

void setup()
{

  alpha = ((SmootherN - 1) / SmootherN); // pre-calculate
  beta = 1 - alpha;                      // pre-calculate to avoid CPU cycle waste
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
}

void loop()
{
  float xcuravg = 0.0;
  float ycuravg = 0.0;
  float zcuravg = 0.0;

  // Take Data
  xsample = analogRead(xpin);
  ysample = analogRead(ypin);
  zsample = analogRead(zpin);

  // Smooth Data
  xcuravg = (xsample * beta) + (xlastAvg * alpha);
  ycuravg = (ysample * beta) + (ylastAvg * alpha);
  zcuravg = (zsample * beta) + (zlastAvg * alpha);

  COUNTER++;
  if (COUNTER > 127)
    COUNTER = 127; // Limit to 127

  // IGNORE FIRST SmootherN VALUES
  //----------------------------------------
  if (COUNTER > SmootherN)
  {
    int x = map(xsample, 256, 387, -100, 100); // 271 change to 266
    xg = ((float)x / (-100.00));
    Serial.print(xg);

    int y = map(ysample, 251, 383, -100, 100); // 267 change to 261
    yg = ((float)y / (-100.00));
    Serial.print(",");
    Serial.print(yg);

    int z = map(zsample, 256, 387, -100, 100); // 266 to 261
    zg = ((float)z / (100.00));
    Serial.print(",");
    Serial.println(zg);
    currentMillis = millis();
  }
  else
  {
    // do nothing
  }

  currentMillis = millis();

  if (((xg > 1.032 && xg < 1.329) || (xg < 0.968 && xg > 0.671)) || ((yg > 0.104 && yg < 0.329) || (yg < -0.104 && yg > -0.329)) || ((zg > 0.104 && zg < 0.329) || (zg < -0.104 && zg > -0.329)))
  {
    if (moving == false)
    {
      movementStarted = currentMillis;
      tone(buzzer, 1000);
    }
    n = 1;
  }
  else if ((xg > 1.081 && xg < 1.270) || (xg > 0.73 && xg < 0.919) || (yg > minValint5 && yg < maxValint5) || (yg > nmaxValint5 && yg < nminValint5) || (zg > minValint5 && zg < maxValint5) || (zg > nmaxValint5 && zg < nminValint5)) // int 5
  {
    if (moving == false) // Did we just started moving?
    {
      movementStarted = currentMillis; // Capture the time we started moving
      tone(buzzer, 1000);
    }

    moving = true;
    n = 2;                                      // Flag that we are moving
    if (currentMillis - movementStarted > int5) // Check how long we have been moving for
    {
      digitalWrite(relay, HIGH);
    }
  }
  else if ((xg > 1.270 && xg < 1.378) || (xg < 0.73 && xg > 0.622) || (yg > maxValint5 && yg < maxValint6) || (yg > nmaxValint5 && yg < nminValint6) || (zg > maxValint5 && zg < maxValint6) || (zg > nmaxValint5 && zg < nminValint6)) // int 6
  {

    if (moving == false) // Did we just started moving?
    {
      movementStarted = currentMillis; // Capture the time we started moving
      tone(buzzer, 1000);
    }

    moving = true;
    n = 3;                                      // Flag that we are moving
    if (currentMillis - movementStarted > int6) // Check how long we have been moving for
    {
      digitalWrite(relay, HIGH);
    }
  }
  else if ((xg > 1.378 && xg < 1.675) || (xg < 0.622 && xg > 0.325) || (yg > maxValint6 && yg < maxValint7) || (yg > nmaxValint6 && yg < nminValint7) || (zg > maxValint6 && zg < maxValint7) || (zg > nmaxValint6 && zg < nminValint7)) // int 7
  {
    if (moving == false) // Did we just started moving?
    {
      movementStarted = currentMillis; // Capture the time we started moving
      tone(buzzer, 1000);
    }

    moving = true;
    n = 4;                                      // Flag that we are moving
    if (currentMillis - movementStarted > int7) // Check how long we have been moving for
    {
      digitalWrite(relay, HIGH);
    }
  }
  else if ((xg > 1.675) || (xg < 0.325) || (yg > maxValint8) || (yg < nminValint8) || (zg > maxValint8) || (zg < nminValint8)) // int 8
  {
    if (moving == false) // Did we just started moving?
    {
      movementStarted = currentMillis; // Capture the time we started moving
      tone(buzzer, 1000);
    }

    moving = true;
    n = 5;                                      // Flag that we are moving
    if (currentMillis - movementStarted > int8) // Check how long we have been moving for
    {
      digitalWrite(relay, HIGH);
    }
  }
  else // Not moving.
  {
    if ((n == 1) || (moving == true && n == 2) || (moving == true && n == 3) || (moving == true && n == 4) || (moving == true && n == 5))
    {
      delaystable = currentMillis;
      n = 0;
    }
    // if the natural frequency of the building is 2 hz 250ms delay, always divide the converted frequency of the building to seconds to 2.
    if (currentMillis - delaystable > 300)
    {
      moving = false; // Flag that we are not moving
      noTone(buzzer); // Turn relay OFF (active LOW)
    }
    else if (currentMillis - movementStarted > period)
    {

      digitalWrite(relay, HIGH);
    }
  }
}
