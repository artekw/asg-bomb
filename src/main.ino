#include "SevenSegmentTM1637.h"
#include "SevenSegmentFun.h"
#include "SevenSegmentExtended.h"
#include "Keypad.h"
#include <stdlib.h>
#include <Time.h>

// Piny wyswietlacza
#define CLK 16
#define DIO 10

// Klawiaturka
const byte ROWS = 4; //cztery wiersze
const byte COLS = 3; //trzy kolumny
const byte buzzer = 9;

// pozycja wpisywanej liczby
int currentLength = 0;

// zmienne czasu

int s = 0;
int m = 0;

int minutes = 0;
int seconds = 0;

time_t t = 0;

char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

// Piny wierszy
byte rowPins[ROWS] = {8, 7, 6, 5};
// Piny kolumn
byte colPins[COLS] = {4, 3, 2};

SevenSegmentExtended display(CLK, DIO);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* USTAWIENIA */
// Długość hasła
char password[5];
// zegar
char time[5];
// wpisane hasło
char entered[5];
/* KONIEC */

unsigned int sec;
long previousMillis = 0;
long interval = 500;  
int ledState = 0;

//tony
int keyTone = 100;
int boom = 200;
int counter = 1000;
int defused = 100;

void setup()
{
    Serial.begin(9600);
    display.begin();            // initializes the display
    display.setBacklight(100);  // set the brightness to 100 %
    display.print("----");      // display INIT on the display
    delay(100);
    setSyncInterval(1000);

    // buzer
    pinMode(buzzer, OUTPUT);
    // czeka na wprowadzenie hasła i czasu

    // wprowadzanie hasła
    while (currentLength < 4) {
        display.setCursor(0, currentLength);
        char keyP = keypad.getKey();
        Serial.print(keyP);
        keyP == NO_KEY;
        noTone(buzzer);
        if (keyP != NO_KEY) {
            if ((keyP != '*') && (keyP != '#')) {
                tone(buzzer, keyTone);
                display.print(keyP);
                password[currentLength] = keyP;
                currentLength++;
            }
        }
    }
    if (currentLength == 4) {
        noTone(buzzer);
        display.clear();
        display.setCursor(0, 0);
        display.print("Good");
        display.blink();
        delay(1000);
        display.print(password);
        currentLength = 0;
        display.clear();
    }

    // ustawienie czasu
    while (currentLength < 4) {
        display.setColonOn(true);
        display.setCursor(0, currentLength);
        char keyT = keypad.getKey();
        Serial.print(keyT);
        noTone(buzzer);
        keyT == NO_KEY;
        if (keyT != NO_KEY) {
            if ((keyT != '*') && (keyT != '#')) {
                tone(buzzer, keyTone);
                display.print(keyT);
                time[currentLength] = keyT;
                currentLength++;
            }
        }   
    }
    if (currentLength == 4) {
        noTone(buzzer);
        display.clear();
        display.setCursor(0, 0);
        display.setColonOn(false);
        display.print("Good");
        display.blink();
        delay(1000);
        display.print(time);
        currentLength = 0;
    }
    m = (time[0] - '0') * 10;
    m = m + (time[1] - '0');

    s = (time[2] - '0') * 10;
    s = s + (time[3] - '0');
    // zerowanie zegara
    setTime(t);
    tone(buzzer, 1000);
    delay(300);
    noTone(buzzer);
    delay(300);
}


void loop()
{
    unsigned long currentMillis = millis();
    Serial.println("Start time");

    minutes = m-minute();
    seconds = s-second();
    sec = seconds + (60 * minutes);

    // czas minął - wybuch
    if (sec <= 0) {
        display.clear();
        display.setCursor(0, 0);
        display.setColonOn(false);
        display.print("****");
        //digitalWrite(buzzer, HIGH);
        tone(buzzer, boom);
        delay(10000000);
    }

    // odliczanie odtatnich 10 sekund
    if (sec < 10){
        noTone(buzzer);
        if(currentMillis - previousMillis > interval) {
            previousMillis = currentMillis;
            if (counter == 0) {
                counter = 1000;
                
            }
            else {
                counter = 0;
                tone(buzzer, counter);
            }
            
        }
    }
            
    display.printTime(sec/60, sec % 60, false);
    Serial.print(sec % 60);

    // czyta znaki hasła
    char key2 = keypad.getKey(); 
    Serial.print(key2);
    if (key2 == '*') {
        display.setColonOn(false);
        display.clear();
        display.setCursor(0, 0); 
        while (currentLength < 4) {
            display.setCursor(0, currentLength);
            char keyP = keypad.getKey();
            noTone(buzzer);
            Serial.print(keyP);
            keyP == NO_KEY;
            if (keyP != NO_KEY) {
                tone(buzzer, keyTone);
                if ((keyP != '*') && (keyP != '#')) {
                    tone(buzzer, keyTone);
                    display.print(keyP);
                    entered[currentLength] = keyP;
                    currentLength++;
                }
            }
        }
        
        // sprawdzam wpisane hasło
        if (entered[0] == password[0] && entered[1] == password[1] && entered[2] == password[2] && entered[3] == password[3]) {
            // rozbrojona
            noTone(buzzer);
            display.clear();
            display.print("Good");
            display.blink();
            for (int i=0; i<3;i++) {
                tone(buzzer, defused);
                delay(100);
                noTone(buzzer);
                delay(100);
            }
            delay(1000);
            display.off();
            delay(10000000);
        }
        else {
            // wybuch
            noTone(buzzer);
            display.clear();
            display.print("****");
            display.blink();
            delay(1000);
            tone(buzzer, 1000);
            delay(10000000);
        }
    }
    delay(100);
}

