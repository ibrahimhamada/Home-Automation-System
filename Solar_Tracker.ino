#include <Servo.h> // include Servo library 

Servo horizontal; // horizontal servo
int servoh = 90; // stand horizontal servo

Servo vertical; // vertical servo 
int servov = 180; // stand vertical servo
                          // br yellow  tr  blue 2   lb blue   lt green
// LDR pin connections
// name = analogpin;
int ldrlt = A0; //LDR top left
int ldrrt = A4; //LDR top rigt
int ldrld =A1; //LDR down left
int ldrrd = A3; //ldr down rigt

void setup()
{
  Serial.begin(9600);
// servo connections
// name.attacht(pin);
  horizontal.attach(3); 
  vertical.attach(6);
  horizontal.write(90);
  vertical.write(180);
  delay(1000);
  
}

void loop() 
{
  int lt = analogRead(ldrlt); // top left
  int rt = analogRead(ldrrt); // top right
  int ld = analogRead(ldrld); // down left
  int rd = analogRead(ldrrd); // down rigt
  Serial.print(lt);
  Serial.print(" ");
  Serial.print(rt);
  Serial.print(" ");
  Serial.print(ld);
  Serial.print(" ");
  Serial.print(rd);
  Serial.print(" ");
  Serial.println("");


int avt = (lt + rt) / 2; // average value top
int avd = (ld + rd) / 2; // average value down
int avl = (lt + ld) / 2; // average value left
int avr = (rt + rd) / 2; // average value right

int dvert = avt - avd; // check the diffirence of up and down
int dhoriz = avl - avr;// check the diffirence og left and rigt

if (dvert > 80 || dvert < -80) // check if the diffirence is in the tolerance else change vertical angle
{
if (avt > avd)
{ 
servov = --servov;
if (servov > 170)
{
servov = 170;
}
}
else if (avt < avd)
{
servov= ++servov;
if (servov < 0)
{
servov = 0;
}
}
vertical.write(servov);
}

if (dhoriz > 80 || dhoriz < -80) // check if the diffirence is in the tolerance else change horizontal angle
{
if (avl > avr)
{
servoh = --servoh;
if (servoh < 0)
{
servoh = 0;
}
}
else if (avl < avr)
{
servoh = ++servoh;
if (servoh > 175)
{
servoh = 175;
}
}
else if (avl == avr)
{
// nothing
}
horizontal.write(servoh);
}
delay(15);
} 
