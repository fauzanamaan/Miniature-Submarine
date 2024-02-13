/*
       ____________________________
      |                            |
      |     Main Circuit Program   |
      |____________________________|
      !!!!----------------------!!!!
                  (⌐■_■)
      
*/


/*
  Libraries
*/
#include <Servo.h>
#include <IRremote.h>
/*
  Constants
*/


//    HEX CODES
const unsigned long EMERGENCY_hexCode = 0xE916FF00; // EMERGENCY >⊙﹏⊙∥
const unsigned long hexCode1 = 0xBA45FF00; // Rotate Left
const unsigned long hexCode2 = 0xB946FF00; // Front Light (RED)
const unsigned long hexCode3 = 0xB847FF00; // Rotate Right
const unsigned long hexCode4 = 0xBB44FF00; // Left Crab
const unsigned long hexCode5 = 0xBF40FF00; // Right Crab
const unsigned long hexCode6 = 0xBC43FF00; // Safety Lights ( YELLOW )
const unsigned long hexCode7 = 0xF807FF00; // Rudder ( 40º )
const unsigned long hexCode8 = 0xEA15FF00; // Rudder ( 140º )
const unsigned long hexCode9 = 0xF609FF00; // Rudder ( 90º )

//    PIN ASSIGNMENTS
const int SignalPin = A0; // Analog Reading from the Water Sensor
const int redLights = 1; // Front Red Light
const int yellowLights = 2; // Safety Yellow Light
const int motor1Pin = 3; // 1st DC Motor
const int motor2Pin = 4; // 2nd DC Motor
const int motor3Pin = 5; // 3rd DC Motor
const int motor4Pin = 6; // 4th DC Motor
const int servoPin = 7; // Servo Motor Pin
const int AlertPin = 8; // Bulb Alert -- Water Filling
const int IR_PIN = 10; // Define the IR receiver 

//    for delay purposes (OR auto-switch off purposes)
const int durationLights = 2500; // Duration for Lights

// Variables
unsigned long IR_HexCode = 0x12345678;
Servo myServo;
int waterLevel_value = 0;
int threshold = 61.44;
bool redLightsOn = false;
bool yellowLightsOn = false;

// Define the IR receiver object
IRrecv irrecv(IR_PIN);

void motorFunction(int Pin1, int Pin2);
void emergencyFunction();

void setup() {
  Serial.begin(9600);
  pinMode(redLights, OUTPUT); // Red Lights
  pinMode(yellowLights, OUTPUT); // Yellow Lights
  pinMode(motor1Pin, OUTPUT); // DC Motor 1 Pin
  pinMode(motor2Pin, OUTPUT); // DC Motor 2 Pin
  pinMode(motor3Pin, OUTPUT); // DC Motor 3 Pin
  pinMode(motor4Pin, OUTPUT); // DC Motor 4 Pin
  pinMode(SignalPin, INPUT); // Signal from Water Sensor Pin
  pinMode(AlertPin, OUTPUT); // Alerting via Light Bulb pin
  myServo.attach(servoPin); // Servo Motor Pin
  irrecv.enableIRIn();
}


void loop() {
  waterLevel_value = analogRead(SignalPin);
  Serial.print("Sensor value: ");
  Serial.println(waterLevel_value);
  
  if (waterLevel_value >= threshold){
    emergencyFunction();
    for (int i = 0; i <= 10000; i++){
      digitalWrite(AlertPin, HIGH);
      delay(500);
      digitalWrite(AlertPin, LOW);
      delay(250);
    }
  }
  // Check if the IR receiver has received a signal
  if (irrecv.decode()) {
    // Print the HEX value of the button press
    IR_HexCode = (irrecv.decodedIRData.decodedRawData, HEX);
    
    if (IR_HexCode == EMERGENCY_hexCode){
      emergencyFunction();
    }
    // Rotate Left
    if (IR_HexCode == hexCode1){
      motorFunction(motor2Pin, motor3Pin);
    }
    
    // Red Lights --- Front ---
    if (IR_HexCode == hexCode2){
      // Toggle the redLights LED
      redLightsOn = !redLightsOn;
      // Set the LED state based on the boolean variable
      digitalWrite(redLights, redLightsOn ? HIGH : LOW);
    }
    
    // Rotate Right
    if (IR_HexCode == hexCode3){
      motorFunction(motor1Pin, motor4Pin);
    }

    // Left Crab
    if (IR_HexCode == hexCode4){
      motorFunction(motor2Pin, motor4Pin);
    }
    
    // Right Crab
    if (IR_HexCode == hexCode5){
      motorFunction(motor1Pin, motor3Pin);
    }
    
    // Yellow Lights - !Safety!
    if (IR_HexCode == hexCode6){
      // Toggle the yellowLights LED
      yellowLightsOn = !yellowLightsOn;
      // Set the LED state based on the boolean variable
      digitalWrite(yellowLights, yellowLightsOn ? HIGH : LOW);
    }
    
    // Rudder - 40 Degrees
    if (IR_HexCode == hexCode7){
      myServo.write(40);
    }
    
    // Rudder - 140 Degrees
    if (IR_HexCode == hexCode8){
      myServo.write(140);
    }
    
    // Rudder - 90 Degrees
    if (IR_HexCode == hexCode9){
      myServo.write(90);
    }
    
    irrecv.resume();
  }
    
}

void emergencyFunction(){
  digitalWrite(redLights, LOW); // Red Lights
  digitalWrite(yellowLights, LOW); // Yellow Lights
  digitalWrite(motor1Pin, LOW); // DC Motor 1 Pin
  digitalWrite(motor2Pin, LOW); // DC Motor 2 Pin
  digitalWrite(motor3Pin, LOW); // DC Motor 3 Pin
  digitalWrite(motor4Pin, LOW); // DC Motor 4 Pin
  myServo.detach(); // This will stop power to the servo motor

}

void motorFunction(int Pin1, int Pin2){
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, HIGH);
  irrecv.resume();
  IR_HexCode = (irrecv.decodedIRData.decodedRawData, HEX);
  while (IR_HexCode == 0){
    irrecv.resume();
    if (irrecv.decode()) {
      IR_HexCode = (irrecv.decodedIRData.decodedRawData, HEX);
      continue;
    } else {
      digitalWrite(Pin1, LOW);
      digitalWrite(Pin2, LOW);
      break;
    }
  }
}

