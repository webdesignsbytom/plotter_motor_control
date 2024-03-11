#include <Stepper.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include "esp_camera.h"
#include <Wire.h>
#include "setup.ino"

// Set LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust

// Steps in rotation of motors
const int stepsPerRevolution = 200;
const int stepsPerRevolutionOfAxisZ = 100; // Z axis smaller motor

// Set stepper motors for all axis
// Need more input pins
Stepper stepperX(stepsPerRevolution, 8, 9, 10, 11);          // X-axis motor
Stepper stepperY(stepsPerRevolution, 4, 5, 6, 7);            // Y-axis motor
Stepper stepperZ(stepsPerRevolutionOfAxisZ, 12, 13, 14, 15); // Z-axis motor

// Set pin numbers from board
// Navigation buttons pins
const int upButtonPin = 1;
const int downButtonPin = 2;
// Select button pin
const int selectButtonPin = 3; // Select from menu
// SD card pin
const int chipSelect = 4;
// Stop switch pins
const int endSwitchX = 5; // Pin for the X-axis limit switch
const int endSwitchY = 6; // Pin for the Y-axis limit switch

// Menu state
int mainMenuIndex = 0;
const int mainMenuItems = 3; // Adjust based on the number of items in your menu

// SD card and selection data
int fileIndex = 0;
int lastButtonStateUp = HIGH;
int lastButtonStateDown = HIGH;

// Button delay settings
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Main setup loop
// Set up device functions and pins
void setup()
{
  runSetupFunctions();
}

// Functions to start device
void runSetupFunctions()
{
  Serial.begin(9600);

  // Initialize motors
  setupStepperMotors();

  // Initialize pins
  setupPins();

  // Initialize LCD display
  setupLCDFunctions();

  // Initialize camera
  setupCamera();

  // Initialize SD card module
  setupSdCardModule();

  // Home all axis
  homeAllAxis();

  // Display welcome menu
  displayMenu(mainMenuIndex);
}

// Setup LCD screen
void setupLCDFunctions()
{
  lcd.init();
  lcd.backlight();
}

// Initialize the switch pins as inputs with internal pull-up
void setupPins()
{
  pinMode(endSwitchX, INPUT_PULLUP); 
  pinMode(endSwitchY, INPUT_PULLUP);
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
}

// Stepper motor setup 
void setupStepperMotors()
{
  // Set speed in RPM of motors
  stepperX.setSpeed(60);
  stepperY.setSpeed(60);
  stepperZ.setSpeed(30);
}

void setupCamera()
{
  Serial.begin(115200);

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.println("Camera init failed with error 0x%x");
    return;
  }
}

// Setup the SD card
void setupSdCardModule() {
    // Initialize SD card
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lcd.print("SD Fail");
    delay(2000); // Show error for 2 seconds
  } else {
    lcd.clear();
    lcd.print("SD OK");
    delay(2000); // Show success for 2 seconds
  }
}

// Main loop code block
void loop()
{
  // Navigate the menu on the LCD screen
  navigateMenu()


  // Listen for commands
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any whitespace

    if (command.startsWith("PU"))
    {
      // Pen Up command. You could use this to disable the motor or to move it to a 'rest' position.
    }
    else if (command.startsWith("PD"))
    {
      // Pen Down command. Here, you can move the motor according to the coordinates that follow.
      int x = command.substring(2).toInt();
      // Assuming 'x' represents the number of steps to move. In a real application, you'd have to convert coordinates to steps.
      myStepper.step(x);
    }
  }
}

void navigateMenu()
{
  // Check button inputs to navigate the menu or select an option
  if (digitalRead(downButtonPin) == LOW) {
    // Increment menu index
    mainMenuIndex = (mainMenuIndex + 1) % mainMenuItems;
    displayMenu(mainMenuIndex);
    delay(200); // Debounce delay
  } else if (digitalRead(upButtonPin) == LOW) {
    // Decrement menu index
    mainMenuIndex--;
    if (mainMenuIndex < 0) mainMenuIndex = mainMenuItems - 1;
    displayMenu(mainMenuIndex);
    delay(200); // Debounce delay
  } else if (digitalRead(selectButtonPin) == LOW) {
    // Select current menu item
    executeMenuItem(mainMenuIndex);
    delay(200); // Debounce delay
  }
}

void updateLcdWithSelectedFile()
{
  // Optional: Display the current file index or file name
  lcd.clear();
  lcd.print("File: ");
  lcd.print(fileIndex); // Or use a function to get the file name based on index
}

void homeAllAxis()
{
  // Move X-axis to home position
  while (digitalRead(endSwitchX) == HIGH)
  {                    // HIGH means the switch is not pressed
    stepperX.step(-1); // Move one step at a time in the negative direction
    delay(5);          // Short delay to control the speed
  }

  // Move Y-axis to home position
  while (digitalRead(endSwitchY) == HIGH)
  {                    // HIGH means the switch is not pressed
    stepperY.step(-1); // Move one step at a time in the negative direction
    delay(5);          // Short delay to control the speed
  }

  // Both axes are now at their home positions
  Serial.println("Axes homed");
}

void displayMenu(int index) {
  lcd.clear();
  lcd.setCursor(0, 0); // Set cursor to the first row
  
  switch (index) {
    case 0:
      lcd.print(">SD Card");
      break;
    case 1:
      lcd.print(">Motor Control");
      break;
    case 2:
      lcd.print(">Test");
      break;
    default:
      lcd.print("Invalid");
      break;
  }
}

void executeMenuItem(int index) {
  lcd.clear();
  switch (index) {
    case 0:
      // Display SD card contents or submenu
      break;
    case 1:
      // Display motor control options or submenu
      break;
    case 2:
      // Run tests or display test options
      break;
  }
}

void emergencyStop()
{
}

void testDevice()
{
}