/* USB LCD */

//#define DEBUG
#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI - comment this out to use with standard arduinos
#include <LiquidCrystal_I2C.h>          // for LCD w/ GPIO MODIFIED for the ATtiny85
#include <DigiUSB.h>

#define GPIO_ADDR     0x27             // (PCA8574A A0-A2 @5V) typ. A0-A3 Gnd 0x20 / 0x38 for A - 0x27 is the address of the Digispark LCD modules.
#define CHARS 20
#define LINES 4

int currentLine = 0;
boolean clear_next = 0;
boolean clear_screen = 0;
boolean backlight = 1;

LiquidCrystal_I2C lcd(GPIO_ADDR, CHARS, LINES);  // set address & 16 chars / 2 lines

void setup(){
  DigiUSB.begin();
  TinyWireM.begin();                    // initialize I2C lib - comment this out to use with standard arduinos
  lcd.init();                           // initialize the lcd 
  lcd.backlight();                      
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Initialized!");
  lcd.setCursor(0, 1);
  lcd.print(CHARS);
  lcd.print("x");
  lcd.print(LINES);  
  lcd.setCursor(0, 0);
  
  clear_screen = true;  // clear the whole screen when we get proper input
}

void get_input() {
  int lastRead;
  // when there are no characters to read, or the character isn't a newline
  while (1==1) {
    if(DigiUSB.available()){
      if(clear_screen) clear_all();
      // something to read
      lastRead = DigiUSB.read();
      // Next line on newline
      if(lastRead == '\n'){
        // cycle back to first line if on last line
        if(currentLine >= (LINES - 1)) {
          currentLine = 0;
          clear_next = true;
        } else {
          currentLine = currentLine + 1;
          clear_next = true;
        }  
        // move to the first column on the next line
        lcd.setCursor(0, currentLine);
      }
      else if(lastRead == 172){ // not sign "¬" send it with the send program to toggle the backlight
        if(backlight){
          lcd.noBacklight();
          backlight = 0;
        }
        else{
          lcd.backlight(); 
          backlight = 1;
        }
        DigiUSB.read(); // read to nothing to get rid of newline that should come after it
      }
      else{
        // clear a new line before writing to it
        if(clear_next) {
          clear_line(currentLine);
          clear_next = 0;
        }
        lcd.print(char(lastRead));
      }
    }
    // refresh the usb port
    DigiUSB.refresh();
    delay(10);
  }
}

// clear a single line
void clear_line(int line) {
  lcd.setCursor(0, line);
  for(int j = 0; j < CHARS; j++) {
    lcd.print(" ");
  }  
  lcd.setCursor(0, line);
}  

// clear the whole screen and move cursor to top left
void clear_all() {
  for(int i = 0; i < LINES; i++) {
    lcd.setCursor(0, i);
    for(int j = 0; j < CHARS; j++) {
      lcd.print(" ");
    }  
  }
  lcd.setCursor(0, 0);
  clear_screen = 0;
}

void loop(){
  get_input();
}
