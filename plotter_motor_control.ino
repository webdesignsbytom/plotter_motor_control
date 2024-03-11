#include <Stepper.h>

// Steps in rotation of motors
const int stepsPerRevolution = 200; 
const int stepsPerRevolutionOfAxisZ = 100; // Z axis smaller motor

// Set stepper motors for all axis
// Need more input pins
Stepper stepperX(stepsPerRevolution, 8, 9, 10, 11); // X-axis motor
Stepper stepperY(stepsPerRevolution, 4, 5, 6, 7);   // Y-axis motor
Stepper stepperZ(stepsPerRevolutionOfAxisZ, 12, 13, 14, 15); // Z-axis motor

// Set pin numbers
const int endSwitchX = 2; // Pin for the X-axis limit switch
const int endSwitchY = 3; // Pin for the Y-axis limit switch
const int eStopButtonPin = 4; // Pin connected to the E-stop button
const int homeAxisButtonPin = 5; // Pin for home axis to 0,0
const int testCommandPin = 6; // Pin for test function

// Set up device
void setup() {
  Serial.begin(9600);

  // Set speed in RPM of motors
  stepperX.setSpeed(60); 
  stepperY.setSpeed(60);
  stepperZ.setSpeed(30);

  // Initialize pins
  pinMode(endSwitchX, INPUT_PULLUP); // Initialize the switch pin as an input with internal pull-up
  pinMode(endSwitchY, INPUT_PULLUP);

  pinMode(eStopButtonPin, INPUT_PULLUP); // Initialize the E-stop pin as input with internal pull-up resistor
  pinMode(homeAxisButtonPin, INPUT_PULLUP); // Initialize the home pin
  pinMode(testCommandPin, INPUT_PULLUP); // Initialize the test pin

  // Home all axis
  homeAllAxis();
}

// Main loop
void loop() {
  // Check for emergency stop button pushed
    if (digitalRead(eStopButtonPin) == LOW) { // Check if the E-stop button is pressed
    emergencyStop();
  }

  // Check for home button pushed
    if (digitalRead(homeAxisButtonPin) == LOW) { // Check if the home button is pressed
    homeAllAxis();
  }

  // Check for test button pushed
    if (digitalRead(testCommandPin) == LOW) { // Check if the test button is pressed
    testDevice();
  }

  // Listen for commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any whitespace
    if (command.startsWith("PU")) {
      // Pen Up command. You could use this to disable the motor or to move it to a 'rest' position.
    } else if (command.startsWith("PD")) {
      // Pen Down command. Here, you can move the motor according to the coordinates that follow.
      int x = command.substring(2).toInt();
      // Assuming 'x' represents the number of steps to move. In a real application, you'd have to convert coordinates to steps.
      myStepper.step(x);
    }
  }
}

void homeAllAxis() {
  // Move X-axis to home position
  while (digitalRead(endSwitchX) == HIGH) { // HIGH means the switch is not pressed
    stepperX.step(-1); // Move one step at a time in the negative direction
    delay(5); // Short delay to control the speed
  }

  // Move Y-axis to home position
  while (digitalRead(endSwitchY) == HIGH) { // HIGH means the switch is not pressed
    stepperY.step(-1); // Move one step at a time in the negative direction
    delay(5); // Short delay to control the speed
  }

  // Both axes are now at their home positions
  Serial.println("Axes homed");
}

void emergencyStop() {
}

void testDevice() {
}