#include <PID_v1.h>
#include <U8x8lib.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ezButton.h>
#include <PWM.h>
#include <math.h>
//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#define DECIMAL_PLACES 4
#define kp_STEP 0.0500
#define ki_STEP 0.0001
#define kd_STEP 1.0000

#define THERMISTORNOMINAL 96000  // resistance at 25 degrees C
#define TEMPERATURENOMINAL 25    // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 5             // how many samples to take and average, more takes longer but is more 'smooth'
#define BCOEFFICIENT 3950        // The beta coefficient of the thermistor (usually 3000-4000)
#define SERIESRESISTOR 4700      // the value of the 'other' resistor

#define BUZZER_PIN A0
#define THERMISTOR_PIN A1  // which analog pin to connect
#define RELAY_PIN 9
#define RELAY_FREQ 60  //frequency (in Hz)
#define MOTOR_PIN 5

#define ENCODER_CLK 2
#define ENCODER_DT 3
#define ENCODER_SW 4

#define EEPROM_PID_ADDRESS 0

#define minHeaterPower 0
#define maxHeaterPower 50
#define SAMPlE_TIME 10  //In seconds

#define STATE_IDLE 0
#define STATE_BREW -2
#define KEEP_WARM 2
#define STATE_MIX -1
#define SHOW_PID -3
#define TEST_PID -4

int STATE = STATE_IDLE;
int sub_state = -1;
bool updateLCD = true;

const char *pidItems[] = {
  "Kp: ",
  "ki: ",
  "kd: ",
  "Start Test",
  "Save"
};
const char *menuItems[] = {
  "Tea",    //95
  "Green",  //80
  "Warm",   //70
  "Mix",
  "See PID",
  "Tst PID"
};
const int menuTemperature[] = {
  95,
  80,
  74,
  STATE_MIX,
  SHOW_PID,
  TEST_PID
};

const uint8_t itemCount = sizeof(menuItems) / sizeof(menuItems[0]);
const uint8_t visibleItems = 3;  // Number of menu items visible at a time
int topItem = 0;                 // Topmost visible menu item
int counter = 1;                 // Currently selected item
int prevCounter = 0;

double pidParams[3];
const double DEFAULT_PID[3] = { 1.0000, 0.0005, 32.0000 };
double input, output;
double currentSetpoint = menuTemperature[1];  // Default to Green Tea

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);
ezButton button(ENCODER_SW);  // create ezButton object
PID pid(&input, &output, &currentSetpoint, DEFAULT_PID[0], DEFAULT_PID[1], DEFAULT_PID[2], DIRECT);

void setup() {
  InitTimersSafe();

  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(THERMISTOR_PIN, INPUT);

  // set analog reference to read AREF pin
  analogReference(EXTERNAL);

  button.setDebounceTime(50);  // set debounce time to 50 milliseconds
  initEncoder();
  initDisplay();

  if (SetPinFrequencySafe(RELAY_PIN, RELAY_FREQ)) {
    beep();
  } else {
    while (1) {
      beep();
      delay(1000);
    }
  }

  initPID();
  STATE = STATE_IDLE;
}

void loop() {

  readButton();
  static unsigned long previousMillis_temp = 0;  // will store last time LED was updated
  unsigned long currentMillis_temp = millis();
  float temp = 0;
  int analog = 0;

  switch (STATE) {

    case STATE_IDLE:
      if (currentMillis_temp - previousMillis_temp >= 1000) {
        previousMillis_temp = currentMillis_temp;

        getTemperature(&temp, &analog);
        printMenuTemp(&temp, &analog);
      }

      if (updateLCD == true) {
        if (counter >= topItem + visibleItems) {
          topItem++;
        }
        if (counter < topItem) {
          topItem--;
        }
        counter = constrain(counter, 0, itemCount - 1);
        topItem = constrain(topItem, 0, itemCount - visibleItems);

        detachInt();
        displayMenu();
        attachInt();
        updateLCD = false;
      }
      break;

    case STATE_MIX:
      if (updateLCD == true) {
        mix();
        updateLCD = false;
      }
      break;

    case SHOW_PID:
      break;

    case TEST_PID:
      if (updateLCD == true) {
        const float change = 1;
        if (prevCounter < counter) {
          if (sub_state == 3) {
            initPID2();
            brewTea(1, menuTemperature[1]);
          } else if (sub_state == 4) {
            beep();
            savePID(pidParams[0], pidParams[1], pidParams[2]);
            beep();
          } else {
            printPIDTest(change);
          }

        } else if (prevCounter > counter) {
          printPIDTest(-change);
        } else {
          printPIDTest(0.0);
        }

        updateLCD = false;
      }
      break;

    default:
      beep();
      beep();
      break;
  }
}

void printMenuTemp(float *temperature, int *adc_value) {
  u8x8.setCursor(11, 0);
  u8x8.print(*adc_value);
  u8x8.setCursor(11, 1);
  u8x8.print(*temperature);
}
