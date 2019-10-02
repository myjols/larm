#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// Display
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
byte xnumberpos = 0;

// Pin Code
boolean binputstr = false;
boolean larm;
byte codestring[5]={0,0,0,0,0};
byte inputstring[4]={0,0,0,0};
byte address;
byte pos;
unsigned long starttime;

// Keypad
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
byte rowPins[ROWS] = { 28, 26, 24, 22 };
// Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins.
byte colPins[COLS] = { 36, 34, 32, 30 };
// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// LED diodes
#define larmonpin 38
#define larmoffpin 40

void setup()
{
  pinMode(larmonpin, OUTPUT);
  pinMode(larmoffpin, OUTPUT);
  larm = false;
  pos = 1;
  address = 0;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  codestring[pos+0] = EEPROM.read(address+0);
  codestring[pos+1] = EEPROM.read(address+1);
  codestring[pos+2] = EEPROM.read(address+2);
  codestring[pos+3] = EEPROM.read(address+3);
  displaywrite();

  Serial.begin(9600);
}

void loop()
{
  char key = kpd.getKey();
  if (key) // Check for a valid key.
  {
    switch (key)
    {
      // Test LED
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
      // Test Display
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
      // Activate Larm
      case 'A':
        larm = true;
        displaywrite();
        xnumberpos = 0;
        pos = 1;
        break;
      // Clear Display
      case 'B':
        displaywrite();
        xnumberpos = 0;
        pos = 1;
        break;
      // Change Code
      case 'C':
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Gammal kod");
        display.display();
        binputstr = false;
        pos = 1;
        address = 0;
        xnumberpos = 0;
        starttime = millis();
        // Input old code
        while (millis() - starttime < 4000) {
          char key = kpd.getKey();         
          if((key=='0' or key=='1' or key=='2' or key=='3' or key=='4' or key=='5' or key=='6' or key=='7' or key=='8' or key=='9') && (pos < 5)) {
            display.setCursor(xnumberpos, 20);  
            display.println(key);
            inputstring[pos] = key;
            display.display();     
            xnumberpos = xnumberpos + 20;
            pos++;
          }
        }
        // Check old code valid
        if(inputstring[1] == EEPROM.read(address+0)+'0' &&
           inputstring[2] == EEPROM.read(address+1)+'0' &&
           inputstring[3] == EEPROM.read(address+2)+'0' &&
           inputstring[4] == EEPROM.read(address+3)+'0') {
          binputstr = true;
        }
        // Continue if old code valid       
        if(binputstr) {
          binputstr = false;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("Ny kod");
          display.display();
          pos = 1;
          xnumberpos = 0;
          starttime = millis();
          // Input new code
          while (millis() - starttime < 4000) {
            char key = kpd.getKey();          
             if((key=='0' or key=='1' or key=='2' or key=='3' or key=='4' or key=='5' or key=='6' or key=='7' or key=='8' or key=='9') && (pos < 5)) {
              display.setCursor(xnumberpos, 20);  
              display.println(key);
              inputstring[pos] = key;
              display.display();     
              xnumberpos = xnumberpos + 20;
              pos++;
            }         
          }
          // Write new code to EEPROM if valid
          if (pos == 5) {
            EEPROM.write(address+0,inputstring[1]-'0');
            EEPROM.write(address+1,inputstring[2]-'0');
            EEPROM.write(address+2,inputstring[3]-'0');
            EEPROM.write(address+3,inputstring[4]-'0');
            codestring[1] = inputstring[1]-'0';
            codestring[2] = inputstring[2]-'0';
            codestring[3] = inputstring[3]-'0';
            codestring[4] = inputstring[4]-'0';
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Ny kod");
            display.println("lagrad");
            display.display();
            delay(2000);
            displaywrite();
          } else {
            // Restore old code if new code invalid
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Gammal kod");
            display.println("kvar");
            display.display();
            delay(2000);   
            displaywrite(); 
          }  
        }
        pos = 1;
        displaywrite();
        break;
      // Factory Reset
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
            address = 0;
            pos = 1;
            EEPROM.write(address+0,1);
            EEPROM.write(address+1,2);
            EEPROM.write(address+2,5);
            EEPROM.write(address+3,1);
           
            codestring[pos+0] = EEPROM.read(address+0);
            codestring[pos+1] = EEPROM.read(address+1);
            codestring[pos+2] = EEPROM.read(address+2);
            codestring[pos+3] = EEPROM.read(address+3);
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Fabrikskod");
            display.println("giltig");
            display.display();
            delay(2000);
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
      // Input code
      default:
        if(pos < 5) {    
          if(pos == 1) {
            starttime = millis();
          }
          display.setCursor(xnumberpos, 20);  
          display.println('*');
          inputstring[pos] = key;
          display.display();
          if(pos == 4 && larm) {        
            if(codestring[1]+'0' == inputstring[1] &&
               codestring[2]+'0' == inputstring[2] &&
               codestring[3]+'0' == inputstring[3] &&
               codestring[4]+'0' == inputstring[4]) {
              larm = false;
              pos = 0;
              xnumberpos = -20;
              displaywrite();
            }
          }
          xnumberpos = xnumberpos + 20;
          pos++;
        }
        break;
    }
  }
  // Clear display if input code not entered within specified time
  if(millis() - starttime > 4000) {
    displaywrite();
    xnumberpos = 0;
    pos = 1;
    display.display();
  }
}

// Write default display text
void displaywrite()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Larm redo");
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
