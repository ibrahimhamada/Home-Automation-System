#include <Servo.h>
#include<SoftwareSerial.h>

SoftwareSerial bt(A3,A2); /* (Rx,Tx) */  

///////////////////////////////////////////Pins & Variables//////////////////////////////////////////////

//////////////////////////////Gate System////////////////////////////////////
//Pins Numbers
#define servo_pin 9
#define Trig_pin 10
#define Echo_pin 4
Servo myservo;
//Variables
#define distance_threshold 10
long duration;
int distance;
int servo_pos=40;


////////////////////////////Lighting System////////////////////////
//Pins Numbers
#define pirPin 2
#define LDR_pin A0
const int LEDs_pins[] = {3, 6, 11};
//Variables
#define light_threshold 400
int LDR_reading;

long unsigned int timeStamp_noMotion;
long unsigned int time_before_considering_motion_ended = 5000;

boolean no_mothion_detected = true;
boolean capture_high_To_low_time;
int PIRValue = 0;



//////////////////////////Fan System//////////////////////////
//Pins Numbers
#define LM35_pin A5
#define fan_IN3 7
#define fan_IN4 8
#define fan_EnB 5
//Variables
#define temp_threshold 25
float LM35_reading;


//////////////////////////Irrigation System/////////////////////////////////
//Pins Numbers
#define pump_IN1 12
#define pump_IN2 13
#define soil_pin A4
//Variables
#define soilWet_threshold 500   // Define max value we consider soil 'wet'
#define soilDry_threshold 750   // Define min value we consider soil 'dry'
int soil_reading;


//Communication
char Incoming_char;
int Incoming_val;




void setup() {
  bt.begin(9600); 
  Serial.begin(9600);
  pin_modes();
  
}

bool flag_auto = false;
bool r = false;


void loop() {
  if(bt.available() > 0)  
  {
    Incoming_char = bt.read();
    
    if(Incoming_char == 'f' || Incoming_char == 'l'){
      while(bt.available() == 0){
      }
      Incoming_val = bt.read();
    }
    if(Incoming_char == 'o'){
      flag_auto = true;
    }
    if(Incoming_char == 'm' || Incoming_char == 'z'){
      flag_auto = false;
      close_fan();
      close_gate();
      close_pump();
      LEDs_OFF();
      
    }
     
    
    }

    
  if(flag_auto == true){
      //Gate System
      Gate_Sys();
      //Lighting System
      Lighting_Sys();
      //Fan System
      Fan_Sys();
      //Irrigation System
      Irrigation_Sys();  
    }
    
    else{
        if(Incoming_char=='a') LEDs_OFF();
        if(Incoming_char=='b') LEDs_ON();
        if(Incoming_char=='l') {
          LEDs_Brightness(Incoming_val);
        }
        if(Incoming_char=='c') close_fan();
        if(Incoming_char=='d') open_fan();
        if(Incoming_char=='f') {
          open_fan_speed(Incoming_val);
        }
        if(Incoming_char=='e') close_gate();
        if(Incoming_char=='g') open_gate();
        if(Incoming_char=='h') close_pump();
        if(Incoming_char=='i') open_pump();
      }
             

}


//Functinos
void pin_modes(){  
   // Sets the LEDs as Output
  for(int i=0; i<3; i++){
    pinMode(LEDs_pins[i], OUTPUT);
  }
  pinMode(pump_IN1, OUTPUT); // Sets the IN1 as an Output
  pinMode(pump_IN2, OUTPUT); // Sets the IN2 as an Output
  pinMode(fan_IN3, OUTPUT); // Sets the IN3 as an Output
  pinMode(fan_IN4, OUTPUT); // Sets the IN4 as an Output
  pinMode(fan_EnB, OUTPUT); // Sets the EnB as an Output
  
  pinMode(Trig_pin, OUTPUT); // Sets the trigPin as an Output
  pinMode(Echo_pin, INPUT); // Sets the echoPin as an Input
  
  pinMode(pirPin, INPUT); // Sets the PIR as an Input
 
  myservo.attach(servo_pin); //Define Servo Pin
  myservo.write(servo_pos); 
}



/////////////////////////////// Gate System ////////////////////////////////
void Gate_Sys(){
  // Clears the trigPin
  digitalWrite(Trig_pin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(Trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_pin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(Echo_pin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  if(distance <= distance_threshold){
    open_gate();
    delay(2000);
    close_gate();
  }
  else{
    close_gate();
  }
  
}

void close_gate(){
  myservo.write(40);
}

void open_gate(){
  myservo.write(180);
}


/////////////////////////////// Lighting System ////////////////////////////////
void Lighting_Sys(){
  LDR_reading = analogRead(LDR_pin);
  Serial.print("LDR Reading = ");
  Serial.println(LDR_reading);
  if (LDR_reading <= light_threshold) {

    if(digitalRead(pirPin) == HIGH) {
      if(no_mothion_detected) {
         PIRValue = 1;
         Serial.println("Motion detected.");
         no_mothion_detected = false;
         LEDs_ON();
         delay(50);
      }
      capture_high_To_low_time = true;
   }
   if(digitalRead(pirPin) == LOW) {
      if(capture_high_To_low_time){
         timeStamp_noMotion = millis();
         capture_high_To_low_time = false;
      }
      if((!no_mothion_detected && millis() - timeStamp_noMotion > time_before_considering_motion_ended)) {
         PIRValue = 0;
         no_mothion_detected = true;
         Serial.println("Motion ended.");
         LEDs_OFF();
         delay(50);
      }
   }    
  }
  else {
    LEDs_OFF();
    no_mothion_detected = true;
  }
}

void LEDs_ON(){
  for(int i=0; i<3; i++){
      analogWrite(LEDs_pins[i],255);
   }
}

void LEDs_OFF(){
  for(int i=0; i<3; i++){
      analogWrite(LEDs_pins[i],0);
    }
}

void LEDs_Brightness(int intensity){
  for(int i=0; i<3; i++){
      analogWrite(LEDs_pins[i],intensity);
    }
}



/////////////////////////////// Fan System ////////////////////////////////
void Fan_Sys(){
  LM35_reading = LM35();  
  Serial.print("Temperature in Celsius = ");
  Serial.println(LM35_reading);
  if (LM35_reading >= temp_threshold){
    Serial.println("OPEN FAN");
    open_fan();
  }
  else{
    close_fan();
  }
}

float LM35(){
  int LM35_reading;
  float temp_val;
  LM35_reading = analogRead(LM35_pin);  /* Read Temperature */
  temp_val = LM35_reading*(5/(1024*10e-3)); /* LM35 gives output of 10mv/°C */
  return temp_val;
}

void close_fan(){
  digitalWrite(fan_IN3, LOW);
  digitalWrite(fan_IN4, LOW);
}

void open_fan(){
  digitalWrite(fan_IN3, HIGH);
  digitalWrite(fan_IN4, LOW);
  analogWrite(fan_EnB, 70);
}

void open_fan_speed(int speed_val){
  digitalWrite(fan_IN3, HIGH);
  digitalWrite(fan_IN4, LOW);
  analogWrite(fan_EnB, speed_val);
}



/////////////////////////////// Irrigation System ////////////////////////////////
void Irrigation_Sys(){
  int moisture = analogRead(soil_pin);
  Serial.print("Soil Moisture Reading: ");
  Serial.println(moisture);
  
  // Determine status of our soil
  if (moisture < soilWet_threshold) {
    Serial.println("Status: Soil is too wet");
    close_pump();
  } else if (moisture >= soilWet_threshold && moisture < soilDry_threshold) {
    Serial.println("Status: Soil moisture is perfect");
    close_pump();
  } else {
    Serial.println("Status: Soil is too dry - time to water!");
    open_pump();
  }
  
  delay(1000);  // Take a reading every second for testing
          // Normally you should take reading perhaps once or twice a day
  Serial.println();
  
}

void close_pump(){
  digitalWrite(pump_IN1, LOW);
  digitalWrite(pump_IN2, LOW);
}

void open_pump(){
  digitalWrite(pump_IN1, HIGH);
  digitalWrite(pump_IN2, LOW);
}
