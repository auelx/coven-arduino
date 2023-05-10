#include <dht.h>
#include <LiquidCrystal_I2C_Hangul.h>
#define dataPin 8 // Defines pin number to which the sensor is connected

dht DHT; // Creats a DHT object
LiquidCrystal_I2C_Hangul lcd(0x27,16,2);

#define BUTTON_START 7
#define RELAY_FAN 4
#define RELAY_HEAT 5
#define BUZZER 6

boolean isStarted = false;
const long TIMER = 28800000L;
const float MAINTAINING_TEMP = 60.0;
long timerMillis = 0L;

void setup() 
{
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_HEAT, OUTPUT);
  pinMode(BUZZER, OUTPUT);
	Serial.begin(9600);

  lcd.init();
  lcd.clear();         
  lcd.backlight(); 
}

void loop() 
{

  if (isPressed(BUTTON_START)) {
    Serial.println("Start Button is pressed");
    isStarted = true;
    timerMillis = TIMER;
    delay(1000);
  }

  while(isStarted) {

      //Uncomment whatever type you're using!
    int readData = DHT.read22(dataPin); // DHT22/AM2302
    int readData = DHT.read11(dataPin); // DHT11

    float t = DHT.temperature; // Gets the values of the temperature
    float h = DHT.humidity; // Gets the values of the humidity

    digitalWrite(RELAY_FAN, HIGH);

    lcd.setCursor(0,0);
    lcd.print("Temp: " + String(t) + " C   ");
    lcd.setCursor(0,1);

    if (timerMillis > 60000) {
      int hours = millisToHours(timerMillis);
      int minutes = millisToMinutes(timerMillis);
      lcd.print("Timer: 0" + String(hours) + ":" + String(minutes));
    } else {
      int seconds = millisToSeconds(timerMillis);
      lcd.print("Timer:   :" + String(seconds)+ "    ");
    }
    Serial.println("Current temp: " + (String)t + " MT: " + (String)MAINTAINING_TEMP);
    if (t > MAINTAINING_TEMP) {
      Serial.println("The heater will now turn off");
      digitalWrite(RELAY_HEAT, LOW);
    } else if (t < MAINTAINING_TEMP)  {
      Serial.println("The heater will now turn on");
      digitalWrite(RELAY_HEAT, HIGH);
    }
    
    //lcd.print("Timer: " + String(hours) + ":" + String(minutes));
    // Printing the results on the serial monitor
    Serial.print("Temperature = ");
    Serial.print(t);
    Serial.print(" ");
    Serial.print((char)176);//shows degrees character
    Serial.print("C | ");
    Serial.print((t * 9.0) / 5.0 + 32.0);//print the temperature in Fahrenheit
    Serial.print(" ");
    Serial.print((char)176);//shows degrees character
    Serial.println("F ");
    Serial.print("Humidity = ");
    Serial.print(h);
    Serial.println(" % ");
    Serial.println("");

    if (isPressed(BUTTON_START)) {
      Serial.println("Start Button is pressed");
      isStarted = false;
    }

    timerMillis-=1000;
    delay(1000); // Delays 2 seconds

    if (timerMillis <= 0) {
      digitalWrite(RELAY_FAN, LOW);
      digitalWrite(RELAY_HEAT, LOW);
      Serial.println("Process Completed.");
      lcd.setCursor(0,0);
      lcd.print("Process Complete");
      lcd.setCursor(0,1);
      lcd.print("----------------");
      isStarted = false;
      digitalWrite(BUZZER, HIGH);
      delay(5000);
      digitalWrite(BUZZER, LOW);
    }
  }
  
  lcd.setCursor(0,0);
  lcd.print("Press the button");
  lcd.setCursor(0,1);
  lcd.print("to start........");
  
}

boolean isPressed(uint8_t pin) {
  return digitalRead(pin) == LOW;
}

long millisToHours(long ml) {
  return ml/3600000;
}

long millisToMinutes(long ml) {
  return (ml%3600000)/60000;
}

long millisToSeconds(long ml) {
  return ((ml%3600000)%60000)/1000;
}