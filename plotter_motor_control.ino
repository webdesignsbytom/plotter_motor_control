#include <Stepper.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>

// Set LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust

// Steps in rotation of motors
const int stepsPerRevolution = 200; 
const int stepsPerRevolutionOfAxisZ = 100; // Z axis smaller motor

// Set stepper motors for all axis
// Need more input pins
Stepper stepperX(stepsPerRevolution, 8, 9, 10, 11); // X-axis motor
Stepper stepperY(stepsPerRevolution, 4, 5, 6, 7);   // Y-axis motor
Stepper stepperZ(stepsPerRevolutionOfAxisZ, 12, 13, 14, 15); // Z-axis motor

// Set pin numbers
const int upButtonPin = 1;
const int downButtonPin = 2;
const int leftButtonPin = 3;
const int rightButtonPin = 4;
const int chipSelect = 5;
const int endSwitchX = 6; // Pin for the X-axis limit switch
const int endSwitchY = 7; // Pin for the Y-axis limit switch
const int eStopButtonPin = 8; // Pin connected to the E-stop button
const int homeAxisButtonPin = 9; // Pin for home axis to 0,0
const int testCommandPin = 10; // Pin for test function
const int selectButtonPin = 11; // Select from menu

// SD card and selection data
int fileIndex = 0;
int lastButtonStateUp = HIGH;
int lastButtonStateDown = HIGH;
int lastButtonStateLeft = HIGH;
int lastButtonStateRight = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

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
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);

  // Initialize LCD display
  lcd.init();
  lcd.backlight();
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Update LCD dislay
  updateLCDDisplay();

  // Home all axis
  homeAllAxis();
}

// Main loop
void loop() {
  // Navigate the menu on the LCD screen
  navigateMenu()

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

void navigateMenu() {
  int upButtonState = digitalRead(upButtonPin);
  int downButtonState = digitalRead(downButtonPin);
  int leftButtonState = digitalRead(leftButtonPin);
  int rightButtonState = digitalRead(rightButtonPin);

  if ((upButtonState == LOW && lastButtonStateUp == HIGH) || (downButtonState == LOW && lastButtonStateDown == HIGH)) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Check Up Button
    if (upButtonState == LOW && lastButtonStateUp == HIGH) {
      fileIndex++;
      updateDisplay();
    }
    // Check Down Button
    if (downButtonState == LOW && lastButtonStateDown == HIGH) {
      fileIndex--;
      updateDisplay();
    }
  }

  lastButtonStateUp = upButtonState;
  lastButtonStateDown = downButtonState;
  lastButtonStateLeft = leftButtonState;
  lastButtonStateRight = rightButtonState;
}

void updateLCDDisplay() {
  // Optional: Display the current file index or file name
  lcd.clear();
  lcd.print("File: ");
  lcd.print(fileIndex); // Or use a function to get the file name based on index
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