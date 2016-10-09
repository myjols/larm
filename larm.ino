#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
byte xnumberpos = 0;

boolean larm;
byte codestring[5]={0,0,0,0,0}, inputstring[4]={0,0,0,0};
byte pos;
byte addr;
unsigned long starttime;

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 8, 7, 6, 5 };
// Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins.
byte colPins[COLS] = { 12, 11, 10, 9 };
// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define larmonpin 3
#define larmoffpin 4

void setup()
{
  pinMode(larmonpin, OUTPUT);
  pinMode(larmoffpin, OUTPUT);
  larm = false;
  pos = 1;
  addr = 1;

  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  codestring[pos+0] = EEPROM.read(addr+0);
  codestring[pos+1] = EEPROM.read(addr+1);
  codestring[pos+2] = EEPROM.read(addr+2);
  codestring[pos+3] = EEPROM.read(addr+3);

  Serial.print(codestring[pos+0]);
  Serial.print(codestring[pos+1]);
  Serial.print(codestring[pos+2]);
  Serial.print(codestring[pos+3]);
  Serial.print(" pos=");
  Serial.println(pos);
  
  displaywrite();
}

void loop()
{
  char key = kpd.getKey();
  if (key) // Check for a valid key.
  {
    switch (key)
    {
      case '*':
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Testar");
        display.println("Dioderna!");
        display.display();        
        digitalWrite(larmonpin, HIGH);
        digitalWrite(larmoffpin, HIGH); 
        delay(2000);
        displaywrite();        
        break;
      case '#':
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Testar");
        display.println("Displayen!");
        display.display();        
        delay(1000);
        // draw multiple rectangles
        testdrawline();
        display.display();
        delay(2000);
        displaywrite();        
        break;
      case 'A':
        larm = true;
        displaywrite();
        xnumberpos = 0;
        pos = 1;
        break;
      case 'B':
        displaywrite();
        xnumberpos = 0;
        pos = 1;
        break;
      case 'C':
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Ny kod:");
        display.display();
        starttime = millis();
        pos = 1;
        addr = 1;
        xnumberpos = 0;
        while (millis() - starttime < 3000) {
          char key = kpd.getKey();          
          if((key >= '0' or key <= '9') && pos < 5) {
            display.setCursor(xnumberpos, 20);  
            display.println(key);
            display.display();     
            inputstring[pos] = key;
            xnumberpos = xnumberpos + 20;
            pos++;
            delay(1000);
          }
        }
        pos = 1;
        Serial.print(inputstring[pos+0]);
        Serial.print(inputstring[pos+1]);
        Serial.print(inputstring[pos+2]);
        Serial.print(inputstring[pos+3]);
        Serial.print(" pos=");
        Serial.println(pos);
        delay(2000);
        displaywrite();
        break;
      case 'D':
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Fabriksins");
        display.println("A = Ja");
        display.println("B = Nej");
        display.display();
        starttime = millis();
        while (millis() - starttime < 3000) {
          char key = kpd.getKey();
          if (key == 'A') {
            addr = 1;
            pos = 1;
            EEPROM.write(addr+0,1);
            EEPROM.write(addr+1,2);
            EEPROM.write(addr+2,5);
            EEPROM.write(addr+3,1);
           
            codestring[pos+0] = EEPROM.read(addr+0);
            codestring[pos+1] = EEPROM.read(addr+1);
            codestring[pos+2] = EEPROM.read(addr+2);
            codestring[pos+3] = EEPROM.read(addr+3);
          
            Serial.print(codestring[pos+0]);
            Serial.print(codestring[pos+1]);
            Serial.print(codestring[pos+2]);
            Serial.print(codestring[pos+3]);
            Serial.print(" pos=");
            Serial.println(pos);
  
            break;
          }
          if (key == 'B') {
            break;
          }
        }
        displaywrite();
        xnumberpos = 0;
        pos = 1;
        break;    
      default:
        if(pos < 5) {    
          if(pos == 1) {
            starttime = millis();
          }
          display.setCursor(xnumberpos, 20);  
          display.println(key);
          inputstring[pos] = key;
          Serial.print(pos);
          Serial.print(codestring[pos]+'0');
          Serial.println(inputstring[pos]);                   
          display.display();
          if(pos == 4 && larm) {        
            if(codestring[1]+'0' == inputstring[1] &&
               codestring[2]+'0' == inputstring[2] &&
               codestring[3]+'0' == inputstring[3] &&
               codestring[4]+'0' == inputstring[4]) {
              larm = false;
              pos = 1;
              displaywrite();
            }
          }
          xnumberpos = xnumberpos + 20;
          pos++;
        }
        break;
    }
  }
  if(millis() - starttime > 4000) {
    displaywrite();
    xnumberpos = 0;
    pos = 1;
    display.display();
  }
}

void displaywrite()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Larm redo");
  display.setCursor(0, 50);
  if(larm) {
    display.println("Larmat");
    digitalWrite(larmonpin, HIGH);
    digitalWrite(larmoffpin, LOW);
  } else {
    display.println("Avlarmat");
    digitalWrite(larmonpin, LOW);
    digitalWrite(larmoffpin, HIGH);
  }
  display.display();
}

void testdrawline() { 
  display.clearDisplay(); 
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);
  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
    delay(1);
  }
  delay(250);
}
