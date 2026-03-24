#define BLYNK_TEMPLATE_ID "TMPL39aVITg1e"
#define BLYNK_TEMPLATE_NAME "LEAK"
#define BLYNK_AUTH_TOKEN "k1PRWyVx6Iz-JPTJYB43-Grq2-uHIEZf"

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define BLYNK_PRINT Serial
#define RELAY 5
#define SENSOR1  25
#define SENSOR2  26

char auth[] = "k1PRWyVx6Iz-JPTJYB43-Grq2-uHIEZf";
char ssid[] = "Realme";
char pass[] = "Blablabla";
// ===== GPS GLOBAL OBJECTS =====
TinyGPSPlus gps;
HardwareSerial GPS(1);

bool locationSent = false;

long currentMillis1 = 0;
long previousMillis1 = 0;
int interval1 = 1000;
//boolean ledState = LOW;
float calibrationFactor1 = 6;
volatile byte pulseCount1;
byte pulse1Sec1 = 0;
float flowRate1;
unsigned int flowMilliLitres1;
unsigned long totalMilliLitres1;
void IRAM_ATTR pulseCounter1()
{
  pulseCount1++;
}
long currentMillis2 = 0;
long previousMillis2 = 0;
int interval2 = 1000;
//boolean ledState = LOW;
float calibrationFactor2 = 6;
volatile byte pulseCount2;
byte pulse1Sec2 = 0;
float flowRate2;
unsigned int flowMilliLitres2;
unsigned long totalMilliLitres2;
void IRAM_ATTR pulseCounter2()
{
  pulseCount2++;
}
void setup()
{
  Serial.begin(115200); 
  GPS.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  pinMode(SENSOR1, INPUT_PULLUP);
  pinMode(SENSOR2, INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);
  pinMode(12, OUTPUT);
  pulseCount1 = 0;
  flowRate1 = 0.0;
  flowMilliLitres1 = 0;
  totalMilliLitres1 = 0;
  previousMillis1 = 0;
  pulseCount2 = 0;
  flowRate2 = 0.0;
  flowMilliLitres2 = 0;
  totalMilliLitres2 = 0;
  previousMillis2 = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR1), pulseCounter1, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2), pulseCounter2, FALLING);
  Blynk.begin(auth, ssid, pass);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("IoT Based Water");
  lcd.setCursor(1, 1);
  lcd.print("Leakage monitor");
  delay(2000);
  
}
void loop()
{
   Blynk.run();
   while (GPS.available()) {
  gps.encode(GPS.read());
}

  currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 > interval1) {
    pulse1Sec1 = pulseCount1;
    pulseCount1 = 0;
    flowRate1 = ((1000.0 / (millis() - previousMillis1)) * pulse1Sec1) / calibrationFactor1;
    previousMillis1 = millis();
    flowMilliLitres1 = (flowRate1 / 60) * 1000;
    totalMilliLitres1 += flowMilliLitres1;
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate1: ");
    Serial.print(int(flowRate1));  
    Serial.print("mL/S");
    Serial.print("\t");
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Flow1=");
    lcd.print(int(flowRate1));
    lcd.print(" mL/S");
    Blynk.virtualWrite(V0,int(flowRate1));
    // Print the cumulative total of litres flowed since starting
    Serial.print("Volunm1: ");
    Serial.print(totalMilliLitres1);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres1 / 1000);
    Serial.println("L");
  }
    currentMillis2 = millis();
  if (currentMillis2 - previousMillis2 > interval2) {
    pulse1Sec2 = pulseCount2;
    pulseCount2 = 0;
    flowRate2 = ((1000.0 / (millis() - previousMillis2)) * pulse1Sec2) / calibrationFactor2;
    previousMillis2 = millis();
    flowMilliLitres2 = (flowRate2 / 60) * 1000;
    totalMilliLitres2 += flowMilliLitres2;
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate2: ");
    Serial.print(int(flowRate2));  // Print the integer part of the variable
    Serial.print("mL/S");
    Serial.print("\t");       // Print tab space
    lcd.setCursor(1, 1);
    lcd.print("Flow2=");
    lcd.print(int(flowRate2));
    lcd.print(" mL/S");
    Blynk.virtualWrite(V1,int(flowRate2));
    // Print the cumulative total of litres flowed since starting
    Serial.print("Volunm2: ");
    Serial.print(totalMilliLitres2);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres2 / 1000);
    Serial.println("L");
    Serial.println(" ");
  }
  if (flowRate2 < flowRate1 && flowRate2 < 8)
  {
    lcd.setCursor(1, 0);
    lcd.print("Leakage occured");
    lcd.setCursor(1, 1);
    lcd.print("Flow 1 to 2 ");
    
    if (!locationSent) {
  if (gps.location.isValid()) {

    float lat = gps.location.lat();
    float lon = gps.location.lng();

    String mapLink = "https://maps.google.com/?q=" 
                     + String(lat, 6) + "," 
                     + String(lon, 6);

    Blynk.logEvent(
      "flow_notify",
      "Water Leakage Detected!\nLocation:\n" + mapLink
    );

  } else {
    Blynk.logEvent(
      "flow_notify",
      "Water Leakage Detected!\nLocation: GPS not fixed yet"
    );
  }

  locationSent = true;
}


    digitalWrite(RELAY, HIGH);
    digitalWrite(12, HIGH);
  }
  else
  {
    digitalWrite(RELAY, LOW);
    digitalWrite(12, LOW);
    locationSent = false;

  }
}