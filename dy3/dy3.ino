/*
  In addition, there are 5 optional settings you can adjust:
  1. The initial color is set to black (so the first color fades in), but
     you can set the initial color to be any other color
  2. The internal loop runs for 1020 interations; the 'wait' variable
     sets the approximate duration of a single crossFade1. In theory,
     a 'wait' of 10 ms should make a crossFade1 of ~10 seconds. In
     practice, the other functions the code is performing slow this
     down to ~11 seconds on my board. YMMV.
  3. If 'repeat' is set to 0, the program will loop indefinitely.
     if it is set to a number, it will loop that number of times,
     then stop on the last color in the sequence. (Set 'return' to 1,
     and make the last color black if you want it to fade out at the end.)
  4. There is an optional 'hold' variable, which pasues the
     program for 'hold' milliseconds when a color is complete,
     but before the next color starts.
  5. Set the DEBUGS flag to 1 if you want DEBUGSging output to be
     sent to the serial monitor.

  April 2007, Clay Shirky <clay.shirky@nyu.edu>
*/


#include "RunningAverage.h"
#include "IRremote.h"

const int receiver = 46;
IRrecv irrecv(receiver);
decode_results results;

const unsigned long maxIn = 1023 * 1023; //Maximum range on multiplied input

int red_val, green_val, blue_val;

//RunningAverage smoothPiezo(20); //Smooth out piezo values and crosstalk

int ledDelay = 0; //Light on time
int noise_thresh = 25;
long prevMillis = 0; //previous turn on time for light delay

int counterK = 0;
int counterHT = 0;
int counterLT = 0;

int brightnessK = 0;
int brightnessHT = 0;
int brightnessLT = 0;

int mode = 0;


//Hi Tom /
const int g2Pin = 5;
const int r2Pin = 6;
const int b2Pin = 7;
int piezoHT = analogRead(A1);

//Low Tom /
const int r3Pin = 9;
const int g3Pin = 10;
const int b3Pin = 8;
int piezoLT = analogRead(A2);

//Kick /
const int r1Pin = 3;   // Red LED,   connected to digital pin 9
const int g1Pin = 2;  // Green LED, connected to digital pin 10
const int b1Pin = 4;  // Blue LED,  connected to digital pin 11
int piezoK = analogRead(A0);

// Color arrays
int black[3]  = { 0, 0, 0 };
int white[3]  = { 100, 100, 100 };
int red[3]    = { 100, 0, 0 };
int green[3]  = { 0, 100, 0 };
int blue[3]   = { 0, 0, 100 };
int yellow[3] = { 40, 95, 0 };
int dimWhite[3] = { 30, 30, 30 };
int White[3] = { 100, 100, 100 };
// etc.

// Set initial color
int redVal1 = black[0];
int grnVal1 = black[1];
int bluVal1 = black[2];

int redVal2 = black[0];
int grnVal2 = black[1];
int bluVal2 = black[2];

int redVal3 = black[0];
int grnVal3 = black[1];
int bluVal3 = black[2];

int wait = 1;      // 10ms internal crossFade1 delay; increase for slower fades
int hold = 1;       // Optional hold when a color is complete, before the next crossFade1
int drumhold = 500;   // delay between drums
int DEBUGS = 0;      // DEBUGS counter; if set to 1, will write values back via serial
int loopCount = 60; // How often should DEBUGS report?
int repeat = 0;     // How many times should we loop before stopping? (0 for no stop)
int j = 0;          // Loop counter for repeat

// Initialize color variables
int prevR1 = redVal1;
int prevG1 = grnVal1;
int prevBlu1 = bluVal1;

int prevR2 = redVal2;
int prevG2 = grnVal2;
int prevB2 = bluVal2;

int prevR3 = redVal3;
int prevG3 = grnVal3;
int prevB3 = bluVal3;

int potK = analogRead(A13);
int potHT = analogRead(A14);
int potLT = analogRead(A15);

int threshK = map (potK, 0, 1023, 100, 1100);
int threshHT = map (potHT, 0, 1023, 100, 1100);
int threshLT = map (potLT, 0, 1023, 100, 1100);

int buttonPin = 22;

// Set up the LED outputs
void setup()
{

  for (int thisPin = 2; thisPin < 11; thisPin++) {
    pinMode(thisPin, OUTPUT); //configure pins for all drum LEDs
  }

  for (int thisPin = 13; thisPin < 16; thisPin++) {
    pinMode(thisPin, INPUT); //configure pins for all pots
  }

  for (int thisPin = 0; thisPin < 3; thisPin++) {
    pinMode(thisPin, INPUT); //configure pins for all piezos
  }

  pinMode(buttonPin, INPUT_PULLUP); //configure case push button 
  
  Serial.begin(9600);
  if (DEBUGS) {           // If we want to see values for DEBUGSging...
//    Serial.begin(9600);  // ...set up the serial ouput
  }

  //Serial.begin(9600);
  //Serial.println("IR Receiver Button Decode");
  //irrecv.enableIRIn();

  //Initialize smoothing
  //smoothPiezo.addValue(0);
}

// Main program: list the order of crossFade1s
void loop()
{

  drumtrigger();

//Until IR remote is implemented, hard code the loop  above
  //drumbright();
  //america();
  //if (irrecv.decode(&results))
  //{
  //  translateIR();
  // Serial.println(results.value, HEX);
  // irrecv.resume();
  // }

}


void drumtrigger()
{
  while (true) {
  
    brightnessK = 255;
    brightnessHT = 255;
    brightnessLT = 255;

    int buttonState = !digitalRead(buttonPin); // Read the state of the button (negated for PULLUP)

    if (buttonState == HIGH) { // Check if the button is pressed
      analogWrite(b1Pin, 0); // off
      analogWrite(g1Pin, brightnessHT); // on 
      analogWrite(r1Pin, 0); // off
      analogWrite(b2Pin, 0); // off
      analogWrite(g2Pin, brightnessHT); // on 
      analogWrite(r2Pin, 0); // off
      analogWrite(b3Pin, 0); // off
      analogWrite(g3Pin, brightnessHT); // on
      analogWrite(r3Pin, 0); // off
    } 

    potK = analogRead(A13);
    potHT = analogRead(A14);
    potLT = analogRead(A15);

    threshK = map (potK, 0, 1023, 100, 1100);
    threshHT = map (potHT, 0, 1023, 100, 1100);
    threshLT = map (potLT, 0, 1023, 100, 1100);

    piezoK = analogRead(A0);
    piezoHT = analogRead(A1);
    piezoLT = analogRead(A2);

// to use for serial plotter which shows all pots and piezo sensor
    Serial.print(potK);
    Serial.print(" ");
    Serial.print(potHT);
    Serial.print(" ");
    Serial.print(potLT);
    Serial.print(" ");
    Serial.print(piezoK);
    Serial.print(" ");
    Serial.print(piezoHT);
    Serial.print(" ");
    Serial.println(piezoLT);
    Serial.print(" ");
    Serial.println(buttonPin);

    if (piezoHT  > threshHT)
    {
      HT();
      counterHT++;
      if (counterHT > 4) {
        counterHT = 0;
      }
    }

    if (piezoLT  > threshLT)
    {
      LT();
      counterLT++;
      if (counterLT > 4) {
        counterLT = 0;
      }
    }

    if (piezoK  > threshK)
    {
      Kick();
      counterK++;
      if (counterK > 4) {
        counterK = 0;
      }
    }

  }
}

void drumbright()
{
  while (true) {
    potK = analogRead(A13);
    potHT = analogRead(A14);
    potLT = analogRead(A15);

    threshK = map (potK, 0, 1023, 2, 900);
    threshHT = map (potHT, 0, 1023, 2, 900);
    threshLT = map (potLT, 0, 1023, 2, 900);

    piezoK = analogRead(A0);
    piezoHT = analogRead(A1);
    piezoLT = analogRead(A2);

    if (piezoHT  > threshHT)
    {
      brightnessHT = map (potHT, 0, 600, 0, 255);
      HT();
      counterHT++;
      if (counterHT > 4) {
        counterHT = 0;
      }
    }

    if (piezoLT  > threshLT)
    {
      brightnessLT = map (potLT, 0, 600, 0, 255);
      LT();
      counterLT++;
      if (counterLT > 4) {
        counterLT = 0;
      }
    }

    if (piezoK  > threshK)
    {
      brightnessLT = map (potK, 0, 600, 0, 255);
      Kick();
      counterK++;
      if (counterK > 4) {
        counterK = 0;
      }
    }
    delay(1);
    analogWrite(b1Pin, 0);
    analogWrite(g1Pin, 0);
    analogWrite(r1Pin, 0);
    analogWrite(b2Pin, 0);
    analogWrite(g2Pin, 0);
    analogWrite(r2Pin, 0);
    analogWrite(b3Pin, 0);
    analogWrite(g3Pin, 0);
    analogWrite(r3Pin, 0);


  }
}


void HT() {
  if (counterHT == 0) {
    analogWrite(b2Pin, brightnessHT);
    analogWrite(g2Pin, 0);
    analogWrite(r2Pin, 0);
  }
  else if (counterHT == 1) {
    analogWrite(b2Pin, 0);
    analogWrite(g2Pin, brightnessHT);
    analogWrite(r2Pin, 0);

  }
  else if (counterHT == 2) {
    analogWrite(b2Pin, 0);
    analogWrite(g2Pin, brightnessHT);
    analogWrite(r2Pin, brightnessHT);

  }
  else if (counterHT == 3) {
    analogWrite(b2Pin, 0);
    analogWrite(g2Pin, 0);
    analogWrite(r2Pin, brightnessHT);

  }
  else if (counterHT == 4) {
    analogWrite(b2Pin, brightnessHT);
    analogWrite(g2Pin, 0);
    analogWrite(r2Pin, brightnessHT);
  }
  else if (counterHT == 5) {
    analogWrite(b2Pin, brightnessHT);
    analogWrite(g2Pin, brightnessHT);
    analogWrite(r2Pin, 0);
  }
  else if (counterHT == 6) {
    analogWrite(b2Pin, brightnessHT);
    analogWrite(g2Pin, brightnessHT);
    analogWrite(r2Pin, brightnessHT);
  }
}

void LT() {
  if (counterLT == 0) {
    analogWrite(b3Pin, brightnessLT);
    analogWrite(g3Pin, 0);
    analogWrite(r3Pin, 0);

  }
  else if (counterLT == 1) {
    analogWrite(b3Pin, 0);
    analogWrite(g3Pin, brightnessLT);
    analogWrite(r3Pin, 0);

  }
  else if (counterLT == 2) {
    analogWrite(b3Pin, 0);
    analogWrite(g3Pin, brightnessLT);
    analogWrite(r3Pin, brightnessLT);

  }
  else if (counterLT == 3) {
    analogWrite(b3Pin, 0);
    analogWrite(g3Pin, 0);
    analogWrite(r3Pin, brightnessLT);

  }
  else if (counterLT == 4) {
    analogWrite(b3Pin, brightnessLT);
    analogWrite(g3Pin, 0);
    analogWrite(r3Pin, brightnessLT);
  }
  else if (counterLT == 5) {
    analogWrite(b2Pin, brightnessLT);
    analogWrite(g2Pin, brightnessLT);
    analogWrite(r2Pin, 0);
  }
  else if (counterLT == 6) {
    analogWrite(b2Pin, brightnessLT);
    analogWrite(g2Pin, brightnessLT);
    analogWrite(r2Pin, brightnessLT);
  }

}

void Kick() {
  if (counterK == 0) {
    analogWrite(b1Pin, brightnessK);
    analogWrite(g1Pin, 0);
    analogWrite(r1Pin, 0);

  }
  else if (counterK == 1) {
    analogWrite(b1Pin, 0);
    analogWrite(g1Pin, brightnessK);
    analogWrite(r1Pin, 0);

  }
  else if (counterK == 2) {
    analogWrite(b1Pin, 0);
    analogWrite(g1Pin, brightnessK);
    analogWrite(r1Pin, brightnessK);

  }
  else if (counterK == 3) {
    analogWrite(b1Pin, 0);
    analogWrite(g1Pin, 0);
    analogWrite(r1Pin, brightnessK);

  }
  else if (counterK == 4) {
    analogWrite(b1Pin, brightnessK);
    analogWrite(g1Pin, 0);
    analogWrite(r1Pin, brightnessK);
  }
  else if (counterK == 5) {
    analogWrite(b2Pin, brightnessK);
    analogWrite(g2Pin, brightnessK);
    analogWrite(r2Pin, 0);
  }
  else if (counterK == 6) {
    analogWrite(b2Pin, brightnessK);
    analogWrite(g2Pin, brightnessK);
    analogWrite(r2Pin, brightnessK);
  }
}

/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS

  The program works like this:
  Imagine a crossFade1 that moves the red LED from 0-10,
    the green from 0-5, and the blue from 10 to 7, in
    ten steps.
    We'd want to count the 10 steps and increase or
    decrease color values in evenly stepped increments.
    Imagine a + indicates raising a value by 1, and a -
    equals lowering it. Our 10 step fade would look like:

    1 2 3 4 5 6 7 8 9 10
  R + + + + + + + + + +
  G   +   +   +   +   +
  B     -     -     -

  The red rises from 0 to 10 in ten steps, the green from
  0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.

  In the real program, the color percentages are converted to
  0-255 values, and there are 1020 steps (255*4).

  To figure out how big a step there should be between one up- or
  down-tick of one of the LED values, we call calculateStep(),
  which calculates the absolute gap between the start and end values,
  and then divides that gap by 1020 to determine the size of the step
  between adjustments in the value.
*/

int calculateStep(int prevValue, int endValue) {

  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero,
    step = 1020 / step;            //   divide by 1020
  }
  return step;
}

/* The next function is calculateVal. When the loop value, i,
   reaches the step size appropriate for one of the
   colors, it increases or decreases the value of that color by 1.
   (R, G, and B are each calculated separately.)
*/

int calculateVal(int step, int vals, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      vals += 1;
    }
    else if (step < 0) {         //   ...or decrement it if step is negative
      vals -= 1;
    }
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (vals > 255) {
    vals = 255;
  }
  else if (vals < 0) {
    vals = 0;
  }
  return vals;

}

/* crossFade1() converts the percentage colors to a
   0-255 range, then loops 1020 times, checking to see if
   the value needs to be updated each time, then writing
   the color values to the correct pins.
*/

void crossFade1(int kick[3], int tom[3], int lowtom[3]) {

  // Convert to 0-255
  int R1 = (kick[0] * 255) / 100;
  int G1 = (kick[1] * 255) / 100;
  int Blu1 = (kick[2] * 255) / 100;

  int R2 = (tom[0] * 255) / 100;
  int G2 = (tom[1] * 255) / 100;
  int B2 = (tom[2] * 255) / 100;

  int R3 = (lowtom[0] * 255) / 100;
  int G3 = (lowtom[1] * 255) / 100;
  int B3 = (lowtom[2] * 255) / 100;

  int stepR1 = calculateStep(prevR1, R1);
  int stepG1 = calculateStep(prevG1, G1);
  int stepBlu1 = calculateStep(prevBlu1, Blu1);

  int stepR2 = calculateStep(prevR2, R2);
  int stepG2 = calculateStep(prevG2, G2);
  int stepB2 = calculateStep(prevB2, B2);

  int stepR3 = calculateStep(prevR3, R3);
  int stepG3 = calculateStep(prevG3, G3);
  int stepB3 = calculateStep(prevB3, B3);

  for (int i = 0; i <= 1020; i++) {

    redVal1 = calculateVal(stepR1, redVal1, i);
    grnVal1 = calculateVal(stepG1, grnVal1, i);
    bluVal1 = calculateVal(stepBlu1, bluVal1, i);

    redVal2 = calculateVal(stepR2, redVal2, i);
    grnVal2 = calculateVal(stepG2, grnVal2, i);
    bluVal2 = calculateVal(stepB2, bluVal2, i);

    redVal3 = calculateVal(stepR3, redVal3, i);
    grnVal3 = calculateVal(stepG3, grnVal3, i);
    bluVal3 = calculateVal(stepB3, bluVal3, i);

    analogWrite(r1Pin, redVal1);   // Write current values to LED pins
    analogWrite(g1Pin, grnVal1);
    analogWrite(b1Pin, bluVal1);

    analogWrite(r2Pin, redVal2);   // Write current values to LED pins
    analogWrite(g2Pin, grnVal2);
    analogWrite(b2Pin, bluVal2);

    analogWrite(r3Pin, redVal3);   // Write current values to LED pins
    analogWrite(g3Pin, grnVal3);
    analogWrite(b3Pin, bluVal3);

    // Update current values for next loop
    prevR1 = redVal1;
    prevG1 = grnVal1;
    prevBlu1 = bluVal1;

    prevR2 = redVal2;
    prevG2 = grnVal2;
    prevB2 = bluVal2;

    prevR3 = redVal3;
    prevG3 = grnVal3;
    prevB3 = bluVal3;

    delay(hold);
  }
}

void america() {
  while (mode = 1) {
    crossFade1(red, black, black);
    delay(drumhold);
    crossFade1(black, red, black);
    delay(drumhold);
    crossFade1(black, black, red);
    delay(drumhold);
    crossFade1(white, black, black);
    delay(drumhold);
    crossFade1(black, white, black);
    delay(drumhold);
    crossFade1(black, black, white);
    delay(drumhold);
    crossFade1(blue, black, black);
    delay(drumhold);
    crossFade1(black, blue, black);
    delay(drumhold);
    crossFade1(black, black, blue);
    delay(drumhold);
    crossFade1(red, black, black);
    delay(drumhold);
    crossFade1(black, white, black);
    delay(drumhold);
    crossFade1(black, black, blue);
    delay(drumhold);

    if (repeat) { // Do we loop a finite number of times?
      j += 1;
      if (j >= repeat) { // Are we there yet?
        exit(j);         // If so, stop.
      }
    }
  }
}

void hannukah() {
  while (mode = 2) {
    crossFade1(white, blue, blue);
    delay(drumhold);
    crossFade1(blue, white, white);
    delay(drumhold);
    crossFade1(white, blue, blue);
    delay(drumhold);
    crossFade1(blue, white, white);
    delay(drumhold);
    crossFade1(white, blue, blue);
    delay(drumhold);
    crossFade1(blue, white, white);
    delay(drumhold);

    crossFade1(white, white, white);
    delay(drumhold);
    crossFade1(blue, blue, blue);
    delay(drumhold);
    crossFade1(white, white, white);
    delay(drumhold);
    crossFade1(blue, blue, blue);
    delay(drumhold);
    crossFade1(white, white, white);
    delay(drumhold);
    crossFade1(blue, blue, blue);
    delay(drumhold);


    if (repeat) { // Do we loop a finite number of times?
      j += 1;
      if (j >= repeat) { // Are we there yet?
        exit(j);         // If so, stop.
      }
    }
  }
}

void translateIR()
{

  switch (results.value)

  {
    case 0xFFA25D: drumbright(); break; //mode = 1; //america(); //break; //Ch-
    case 0xFF629D: drumtrigger(); break; //mode = 2; //hannukah(); //break; //Ch
    case 0xFFE21D: //mode = 3; //drumtrigger(); //break; //Ch+
    case 0xFF22DD: //mode = 4;    //Back
    case 0xFF02FD: ;  break; //Forward
    case 0xFFC23D: ;  break; //Play
  }// End Case

  delay(400); //How long to wait before giving repeat signal


} //END translateIR

