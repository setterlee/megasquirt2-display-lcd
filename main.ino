#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- Pines ----------
#define BTN_PAGE 2
#define BTN_PEAK 3

#define MAP_PIN  A0
#define OILP_PIN A1
#define OILT_PIN A2
#define BAT_PIN  A3
#define AIRT_PIN A6

// ---------- Timing ----------
const unsigned long UPDATE_INTERVAL   = 250;
const unsigned long TEST_INTERVAL     = 120;
const unsigned long PEAK_VIEW_TIME    = 3000;
const unsigned long LONG_PRESS_TIME   = 1500;
const unsigned long UNIT_MSG_TIME     = 3000;

// ---------- Estado ----------
bool testMode = true;
bool showPeaks = false;
bool usePSI = true;
bool showUnitChange = false;

unsigned long lastUpdate = 0;
unsigned long lastTestStep = 0;
unsigned long peakViewStart = 0;
unsigned long unitChangeStart = 0;

// ---------- Páginas ----------
enum Page {
  PAGE_MAIN,
  PAGE_BAT,
  PAGE_COUNT
};

Page currentPage = PAGE_MAIN;

// ---------- Valores ----------
float boostBar = 0;
float oilPressure = 0;
float oilTemp = 0;
float airTemp = 0;
float batteryVolt = 0;

// ---------- Min / Max ----------
float boostMin, boostMax;
float oilPMin,  oilPMax;
float oilTMin,  oilTMax;
float airTMin,  airTMax;
float batMin,   batMax;

// ---------- Test ----------
float testStep = 0;
bool testUp = true;

void resetAllPeaks() {
  boostMin = oilPMin = oilTMin = airTMin = batMin =  999;
  boostMax = oilPMax = oilTMax = airTMax = batMax = -999;
}

// ---------- SETUP ----------
void setup() {
  pinMode(BTN_PAGE, INPUT_PULLUP);
  pinMode(BTN_PEAK, INPUT_PULLUP);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  resetAllPeaks();

  lcd.setCursor(0, 0);
  lcd.print("Hi Setterlee!");
  lcd.setCursor(0, 1);
  lcd.print("Welcome back :)");
  delay(1200);
  lcd.clear();
}

// ---------- LOOP ----------
void loop() {
  handlePageButton();
  handlePeakButton();

  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();

    if (testMode) updateTestValues();
    else          readRealSensors();

    updatePeaks();

    if (showPeaks && millis() - peakViewStart > PEAK_VIEW_TIME) {
      showPeaks = false;
      lcd.clear();
    }

    if (showUnitChange && millis() - unitChangeStart > UNIT_MSG_TIME) {
      showUnitChange = false;
      lcd.clear();
    }

    if (!showUnitChange) {
      drawScreen();
    }
  }
}

// ---------- BOTON PAGE ----------
void handlePageButton() {
  static bool lastState = HIGH;
  static unsigned long pressStart = 0;
  static bool longHandled = false;
  bool current = digitalRead(BTN_PAGE);

  if (lastState == HIGH && current == LOW) {
    pressStart = millis();
    longHandled = false;
  }

  // Long press: cambiar unidad
  if (current == LOW && !longHandled &&
      millis() - pressStart >= LONG_PRESS_TIME) {
    usePSI = !usePSI;
    showUnitChange = true;
    unitChangeStart = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UNIT CHANGE:");
    lcd.setCursor(0, 1);
    lcd.print(usePSI ? "BAR -> PSI" : "PSI -> BAR");
    longHandled = true;
  }

  // Short press: cambiar página
  if (lastState == LOW && current == HIGH && !longHandled) {
    currentPage = (Page)((currentPage + 1) % PAGE_COUNT);
    lcd.clear();
  }

  lastState = current;
}

// ---------- BOTON PEAK ----------
void handlePeakButton() {
  static bool lastState = HIGH;
  static unsigned long pressStart = 0;
  static bool longHandled = false;

  bool current = digitalRead(BTN_PEAK);

  if (lastState == HIGH && current == LOW) {
    pressStart = millis();
    longHandled = false;
  }

  if (current == LOW && !longHandled &&
      millis() - pressStart >= LONG_PRESS_TIME) {
    resetAllPeaks();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PEAK RESET");
    lcd.setCursor(0, 1);
    lcd.print("DONE");
    delay(1000);
    lcd.clear();
    longHandled = true;
  }

  if (lastState == LOW && current == HIGH && !longHandled) {
    showPeaks = true;
    peakViewStart = millis();
    lcd.clear();
  }

  lastState = current;
}

// ---------- TEST MODE ----------
void updateTestValues() {
  if (millis() - lastTestStep >= TEST_INTERVAL) {
    lastTestStep = millis();

    testStep += testUp ? 0.05 : -0.05;
    if (testStep >= 1) testUp = false;
    if (testStep <= 0) testUp = true;

    boostBar    = -0.6 + testStep * 1.8;
    oilPressure =  1.0 + testStep * 4.0;
    oilTemp     = 60  + testStep * 50;
    airTemp     = 10  + testStep * 60;
    batteryVolt = 12.0 + testStep * 1.8;
  }
}

// ---------- REAL MODE ----------
void readRealSensors() {
  boostBar    = map(analogRead(MAP_PIN),  0, 1023, -100, 120) / 100.0;
  oilPressure = map(analogRead(OILP_PIN), 0, 1023, 0, 600) / 100.0;
  oilTemp     = map(analogRead(OILT_PIN), 0, 1023, 20, 140);
  airTemp     = map(analogRead(AIRT_PIN), 0, 1023, -10, 80);
  batteryVolt = map(analogRead(BAT_PIN),  0, 1023, 0, 200) / 10.0;
}

// ---------- PEAK UPDATE ----------
void updatePeaks() {
  boostMin = min(boostMin, boostBar);
  boostMax = max(boostMax, boostBar);

  oilPMin  = min(oilPMin, oilPressure);
  oilPMax  = max(oilPMax, oilPressure);

  oilTMin  = min(oilTMin, oilTemp);
  oilTMax  = max(oilTMax, oilTemp);

  airTMin  = min(airTMin, airTemp);
  airTMax  = max(airTMax, airTemp);

  batMin   = min(batMin, batteryVolt);
  batMax   = max(batMax, batteryVolt);
}

// ---------- DISPLAY ----------
void drawScreen() {
  lcd.setCursor(0, 0);

  if (showPeaks) {
    if (currentPage == PAGE_MAIN) {
      lcd.print("MAP ");
      float mapMinDisplay = usePSI ? boostMin * 14.5038 : boostMin;
      float mapMaxDisplay = usePSI ? boostMax * 14.5038 : boostMax;
      lcd.print(mapMinDisplay, 1);
      lcd.print("/");
      lcd.print(mapMaxDisplay, 1);
      lcd.setCursor(0, 1);
      lcd.print("OIL ");
      float oilMinDisplay = usePSI ? oilPMin * 14.5038 : oilPMin;
      float oilMaxDisplay = usePSI ? oilPMax * 14.5038 : oilPMax;
      lcd.print(oilMinDisplay, 1);
      lcd.print("/");
      lcd.print(oilMaxDisplay, 1);
      lcd.print(usePSI ? "p" : "b");
    }
    return;
  }

  if (currentPage == PAGE_MAIN) {
    lcd.print("MAP:");
    float mapDisplay = usePSI ? boostBar * 14.5038 : boostBar;
    if (boostBar >= 0) lcd.print("+");
    lcd.print(mapDisplay, 1);
    lcd.print(usePSI ? "p " : "b ");
    lcd.print((int)airTemp);
    lcd.print("C ");

    lcd.setCursor(0, 1);
    lcd.print("OIL:");
    float oilDisplay = usePSI ? oilPressure * 14.5038 : oilPressure;
    lcd.print(oilDisplay, 1);
    lcd.print(usePSI ? "p " : "b ");
    lcd.print((int)oilTemp);
    lcd.print("C ");
  }

  if (currentPage == PAGE_BAT) {
    lcd.print("BAT:");
    lcd.print(batteryVolt, 1);
    lcd.print("V   ");

    lcd.setCursor(0, 1);
    lcd.print("MODE:");
    lcd.print(testMode ? "TEST" : "REAL");
  }
}