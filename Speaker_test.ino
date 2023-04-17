

#include"pitches.h" // header file with all possible Hz
#include "Volume.h" // header file to maintain volume of a speaker 


Volume vol(6);
int frequency = 740; // default frequency
float MAX_DIST = 100;
//all pins 
int triggerPin = 2; 
int echoPin = 3;
int triggerPin_down = 12;
int echoPin_down = 13;

const int buzzer = 9; 
const int button = 11;


void alarm(){ // this function maintains the buzzer. Button press leads to the sound from the buzzer  
  int  buttonState = digitalRead(button);

  if (buttonState != HIGH){
    tone(buzzer, 1000); // Send 1KHz sound signal...
    delay(100);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    delay(100);        // ...for 1sec
  }
}
long readUltrasonicDistance(int t_pin, int e_pin) // Read the distance from the 1st ultrasonic sensor
{
  pinMode(t_pin, OUTPUT); // Clear the trigger
  digitalWrite(t_pin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(t_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(t_pin, LOW);
  pinMode(e_pin, INPUT);
  return pulseIn(e_pin, HIGH);
}
// 0.5 , 2, 4
//values for high-distanced object
int distantVolume = 64;
int distantFreq = 500;
//values for medium-distanced object
int mediumVolume = 128;
int mediumFreq = 600;

//values for close-distanced object
int closeVolume = 255;
int closeFreq = 440;

void playNoise(int distance) { // noise from the speaker due to the 1st ultrasonic sensor's data
  int v;
  if (distance >= MAX_DIST){ // distance is more than 4m
    v = -1;
  } else if (distance >= 200 && distance < MAX_DIST){ //distance is more than 2m
    v = distantVolume;
    frequency = distantFreq;
  } else if (distance >= 50 && distance < 200){ //distance is more than 0.5m
    v = mediumVolume;
    frequency = mediumFreq;
  } else { //distance is less than 0.5m
    v = closeVolume;
    frequency = closeFreq;
  }
  Serial.print("Vol ");

  Serial.println(v);
  if (v <= 0 || distance == 0) { //stop 
    vol.noTone();
    vol.delay(1000);
    Serial.println("Stop!");
  }
  else { //define the number of pulses (duration of the sound) based on the distance from the object 
    int max_pulses = 3;
    if (v == closeVolume){
      max_pulses = 5;
      Serial.println("Close");
    }
    for (int i = 0; i < max_pulses; i++) {
      vol.tone(frequency, v);
      vol.delay(100);
    }
  }
  vol.noTone();
  vol.delay(1000);
}

void playNoise_down() { // noise from the speaker due to the 2nd ultrasonic sensor's data
  for (int i = 0; i < 3; i++) { //three slightly different noises
    vol.tone((i + 60 * i), 255); 
    vol.delay(100);
  }
  vol.noTone();
  vol.delay(1000);
}

void obst_alarm(int dist) { // function for the alarm due to the object under the cane
  if (dist > 30 && dist <= 400) {
    Serial.println("CHECK OK");
    playNoise_down();
  }
}

void setup() {
  vol.begin();
  pinMode(button, INPUT); 
  digitalWrite(button, HIGH);
  Serial.begin(9600);
}


void loop() {
  alarm(); // triggered if the button is presses
  int cm = 0.01723 * readUltrasonicDistance(triggerPin, echoPin); //data from the 1st ultrasonic sensor
  Serial.println(cm);
  playNoise(cm); // play noise based on the data from the 1st sensor (if necessary)

  // measure the ping time in cm
  int cm_down = 0.01723 * readUltrasonicDistance(triggerPin_down, echoPin_down); //data from the 2nd ultrasonic sensor
  Serial.print("cm: ");
  Serial.println(cm_down);
  obst_alarm(cm_down); // play noise based on the data from the 2nd sensor (if necessary)
  delay(1000); // Wait for 100 millisecond(s)
}
