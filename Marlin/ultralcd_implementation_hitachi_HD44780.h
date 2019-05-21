#ifndef ULTRA_LCD_IMPLEMENTATION_HITACHI_HD44780_H
#define ULTRA_LCD_IMPLEMENTATION_HITACHI_HD44780_H

/**
* Implementation of the LCD display routines for a Hitachi HD44780 display. These are common LCD character displays.
* When selecting the Russian language, a slightly different LCD implementation is used to handle UTF8 characters.
**/

#ifndef REPRAPWORLD_KEYPAD
extern volatile uint8_t buttons;  //the last checked buttons in a bit array.
#else
extern volatile uint16_t buttons;  //an extended version of the last checked buttons in a bit array.
#endif

////////////////////////////////////
// Setup button and encode mappings for each panel (into 'buttons' variable
//
// This is just to map common functions (across different panels) onto the same 
// macro name. The mapping is independent of whether the button is directly connected or 
// via a shift/i2c register.

#ifdef ULTIPANEL
// All UltiPanels might have an encoder - so this is always be mapped onto first two bits
#define BLEN_B 1
#define BLEN_A 0

#define EN_B (1<<BLEN_B) // The two encoder pins are connected through BTN_EN1 and BTN_EN2
#define EN_A (1<<BLEN_A)

#if defined(BTN_ENC) && BTN_ENC > -1
  // encoder click is directly connected
  #define BLEN_C 2 
  #define EN_C (1<<BLEN_C) 
#endif 
  
//
// Setup other button mappings of each panel
//
#if defined(LCD_I2C_VIKI)
  #define B_I2C_BTN_OFFSET 3 // (the first three bit positions reserved for EN_A, EN_B, EN_C)
  
  // button and encoder bit positions within 'buttons'
  #define B_LE (BUTTON_LEFT<<B_I2C_BTN_OFFSET)    // The remaining normalized buttons are all read via I2C
  #define B_UP (BUTTON_UP<<B_I2C_BTN_OFFSET)
  #define B_MI (BUTTON_SELECT<<B_I2C_BTN_OFFSET)
  #define B_DW (BUTTON_DOWN<<B_I2C_BTN_OFFSET)
  #define B_RI (BUTTON_RIGHT<<B_I2C_BTN_OFFSET)

  #if defined(BTN_ENC) && BTN_ENC > -1 
    // the pause/stop/restart button is connected to BTN_ENC when used
    #define B_ST (EN_C)                            // Map the pause/stop/resume button into its normalized functional name 
    #define LCD_CLICKED (buttons&(B_MI|B_RI|B_ST)) // pause/stop button also acts as click until we implement proper pause/stop.
  #else
    #define LCD_CLICKED (buttons&(B_MI|B_RI))
  #endif  

  // I2C buttons take too long to read inside an interrupt context and so we read them during lcd_update
  #define LCD_HAS_SLOW_BUTTONS

#elif defined(LCD_I2C_PANELOLU2)
  // encoder click can be read through I2C if not directly connected
  #if BTN_ENC <= 0 
    #define B_I2C_BTN_OFFSET 3 // (the first three bit positions reserved for EN_A, EN_B, EN_C)
  
    #define B_MI (PANELOLU2_ENCODER_C<<B_I2C_BTN_OFFSET) // requires LiquidTWI2 library v1.2.3 or later

    #define LCD_CLICKED (buttons&B_MI)

    // I2C buttons take too long to read inside an interrupt context and so we read them during lcd_update
    #define LCD_HAS_SLOW_BUTTONS
  #else
    #define LCD_CLICKED (buttons&EN_C)  
  #endif

#elif defined(REPRAPWORLD_KEYPAD)
    // define register bit values, don't change it
    #define BLEN_REPRAPWORLD_KEYPAD_F3 0
    #define BLEN_REPRAPWORLD_KEYPAD_F2 1
    #define BLEN_REPRAPWORLD_KEYPAD_F1 2
    #define BLEN_REPRAPWORLD_KEYPAD_UP 3
    #define BLEN_REPRAPWORLD_KEYPAD_RIGHT 4
    #define BLEN_REPRAPWORLD_KEYPAD_MIDDLE 5
    #define BLEN_REPRAPWORLD_KEYPAD_DOWN 6
    #define BLEN_REPRAPWORLD_KEYPAD_LEFT 7
    
    #define REPRAPWORLD_BTN_OFFSET 3 // bit offset into buttons for shift register values

    #define EN_REPRAPWORLD_KEYPAD_F3 (1<<(BLEN_REPRAPWORLD_KEYPAD_F3+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_F2 (1<<(BLEN_REPRAPWORLD_KEYPAD_F2+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_F1 (1<<(BLEN_REPRAPWORLD_KEYPAD_F1+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_UP (1<<(BLEN_REPRAPWORLD_KEYPAD_UP+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_RIGHT (1<<(BLEN_REPRAPWORLD_KEYPAD_RIGHT+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_MIDDLE (1<<(BLEN_REPRAPWORLD_KEYPAD_MIDDLE+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_DOWN (1<<(BLEN_REPRAPWORLD_KEYPAD_DOWN+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_LEFT (1<<(BLEN_REPRAPWORLD_KEYPAD_LEFT+REPRAPWORLD_BTN_OFFSET))

    #define LCD_CLICKED ((buttons&EN_C) || (buttons&EN_REPRAPWORLD_KEYPAD_F1))
    #define REPRAPWORLD_KEYPAD_MOVE_Y_DOWN (buttons&EN_REPRAPWORLD_KEYPAD_DOWN)
    #define REPRAPWORLD_KEYPAD_MOVE_Y_UP (buttons&EN_REPRAPWORLD_KEYPAD_UP)
    #define REPRAPWORLD_KEYPAD_MOVE_HOME (buttons&EN_REPRAPWORLD_KEYPAD_MIDDLE)

#elif defined(NEWPANEL)
  #define LCD_CLICKED (buttons&EN_C)
  
#else // old style ULTIPANEL
  //bits in the shift register that carry the buttons for:
  // left up center down right red(stop)
  #define BL_LE 7
  #define BL_UP 6
  #define BL_MI 5
  #define BL_DW 4
  #define BL_RI 3
  #define BL_ST 2

  //automatic, do not change
  #define B_LE (1<<BL_LE)
  #define B_UP (1<<BL_UP)
  #define B_MI (1<<BL_MI)
  #define B_DW (1<<BL_DW)
  #define B_RI (1<<BL_RI)
  #define B_ST (1<<BL_ST)
  
  #define LCD_CLICKED (buttons&(B_MI|B_ST))
#endif

////////////////////////
// Setup Rotary Encoder Bit Values (for two pin encoders to indicate movement)
// These values are independent of which pins are used for EN_A and EN_B indications
// The rotary encoder part is also independent to the chipset used for the LCD
#if defined(EN_A) && defined(EN_B)
    #define encrot0 0
    #define encrot1 2
    #define encrot2 3
    #define encrot3 1
#endif 

#endif //ULTIPANEL

////////////////////////////////////
// Create LCD class instance and chipset-specific information
#if defined(LCD_I2C_TYPE_PCF8575)
  // note: these are register mapped pins on the PCF8575 controller not Arduino pins
  #define LCD_I2C_PIN_BL  3
  #define LCD_I2C_PIN_EN  2
  #define LCD_I2C_PIN_RW  1
  #define LCD_I2C_PIN_RS  0
  #define LCD_I2C_PIN_D4  4
  #define LCD_I2C_PIN_D5  5
  #define LCD_I2C_PIN_D6  6
  #define LCD_I2C_PIN_D7  7

  #include <Wire.h>
  #include <LCD.h>
  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C
  LCD_CLASS lcd(LCD_I2C_ADDRESS,LCD_I2C_PIN_EN,LCD_I2C_PIN_RW,LCD_I2C_PIN_RS,LCD_I2C_PIN_D4,LCD_I2C_PIN_D5,LCD_I2C_PIN_D6,LCD_I2C_PIN_D7);
  
#elif defined(LCD_I2C_TYPE_MCP23017)
  //for the LED indicators (which maybe mapped to different things in lcd_implementation_update_indicators())
  #define LED_A 0x04 //100
  #define LED_B 0x02 //010
  #define LED_C 0x01 //001

  #define LCD_HAS_STATUS_INDICATORS

  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2
  #if defined(DETECT_DEVICE)
     LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
     LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif
  
#elif defined(LCD_I2C_TYPE_MCP23008)
  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2
  #if defined(DETECT_DEVICE)
     LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
     LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif

#elif defined(LCD_I2C_TYPE_PCA8574)
    #include <LiquidCrystal_I2C.h>
    #define LCD_CLASS LiquidCrystal_I2C
    LCD_CLASS lcd(LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT);
    
// 2 wire Non-latching LCD SR from:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/schematics#!shiftregister-connection 
#elif defined(SR_LCD_2W_NL)

  extern "C" void __cxa_pure_virtual() { while (1); }
  #include <LCD.h>
  #include <LiquidCrystal_SR.h>
  #define LCD_CLASS LiquidCrystal_SR
  LCD_CLASS lcd(SR_DATA_PIN, SR_CLK_PIN);

#else
  // Standard directly connected LCD implementations
  #ifdef LANGUAGE_RU
    #include "LiquidCrystalRus.h"
    #define LCD_CLASS LiquidCrystalRus
  #else 
    #include <LiquidCrystal.h>
    #define LCD_CLASS LiquidCrystal
  #endif  
  LCD_CLASS lcd(LCD_PINS_RS, LCD_PINS_ENABLE, LCD_PINS_D4, LCD_PINS_D5,LCD_PINS_D6,LCD_PINS_D7);  //RS,Enable,D4,D5,D6,D7
#endif

#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
  static uint16_t progressBarTick = 0;
  #if PROGRESS_MSG_EXPIRE > 0
    static uint16_t messageTick = 0;
  #endif
  #define LCD_STR_PROGRESS  "\x03\x04\x05"
#endif

/* Custom characters defined in the first 8 characters of the LCD */
#define LCD_STR_BEDTEMP     "\x00"
#define LCD_STR_DEGREE      "\x01"
#define LCD_STR_THERMOMETER "\x02"
//-#define LCD_STR_THERMOMETERA "\x03"
#define LCD_STR_THERMOMETERA "\x07"
//#define LCD_STR_UPLEVEL     "\x03"
#define LCD_STR_UPLEVEL     "\x5E"  /* from the default character set */
#define LCD_STR_REFRESH     "\x04"
#define LCD_STR_FOLDER      "\x05"
#define LCD_STR_FEEDRATE    "\x06"
//-#define LCD_STR_CLOCK       "\x07"
#define LCD_STR_CLOCK       "\x03"
#define LCD_STR_ARROW_RIGHT "\x7E"  /* from the default character set */

static void lcd_restore_symbols_magnum(
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    bool progress_bar_set=true
  #endif
) {
    byte bedTemp[8] =
    {
        B01001,
        B10010,
        B01001,
        B10010,
        B00000,
        B11111,
        B01010,
        B00000
    };
    byte degree[8] =
    {
        B01100,
        B10010,
        B10010,
        B01100,
        B00000,
        B00000,
        B00000,
        B00000
    };
    /*byte thermometer[8] =
    {
        B11111,
        B11011,
        B11011,
        B11111,
        B01010,
        B01010,
        B00100,
        B00000
    }; old */
	byte thermometer[8] =
    {
        B11111,
        B10001,
        B10001,
        B11111,
        B01010,
        B01010,
        B00100,
        B00000
    };
	byte clock [8]={
        B00000,
        B01110,
        B10011,
        B10101,
        B10001,
        B01110,
        B00000,
        B00000
    };
   /* byte uplevel[8]={
        B00100,
        B01110,
        B11111,
        B00100,
        B11100,
        B00000,
        B00000,
        B00000
    }; //thanks joris */
    byte refresh[8]={
        B00000,
        B00110,
        B11001,
        B11000,
        B00011,
        B10011,
        B01100,
        B00000,
    }; //thanks joris
    byte folder [8]={
        B00000,
        B11100,
        B11111,
        B10001,
        B10001,
        B11111,
        B00000,
        B00000
    };
	byte feedrate [8]={
        B00010,
        B00100,
        B01100,
        B11111,
        B00110,
        B00100,
        B01000,
        B10000
    }; //MG
    byte thermometera[8] =
    {
        B11011,
        B11011,
        B11011,
        B11111,
        B01010,
        B01010,
        B00100,
        B00000
    };
	
	#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    static bool char_mode = false;
    byte progress[3][8] = { {
      B00000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B00000
    }, {
      B00000,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B00000
    }, {
      B00000,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B00000
    } };
    if (progress_bar_set != char_mode) {
      char_mode = progress_bar_set;
      lcd.createChar(LCD_STR_BEDTEMP[0], bedTemp);
      lcd.createChar(LCD_STR_DEGREE[0], degree);
      lcd.createChar(LCD_STR_THERMOMETER[0], thermometer);
	  lcd.createChar(LCD_STR_THERMOMETERA[0], thermometera);
	  //lcd.createChar(LCD_STR_CLOCK[0], clock);
      lcd.createChar(LCD_STR_FEEDRATE[0], feedrate);
	  
      
      if (progress_bar_set) {
        // Progress bar characters for info screen
        for (int i=3; i--;) lcd.createChar(LCD_STR_PROGRESS[i], progress[i]);
      }
      else {
        // Custom characters for submenus
        //lcd.createChar(LCD_STR_UPLEVEL[0], uplevel);
		//lcd.createChar(LCD_STR_THERMOMETERA[0], thermometera);
		lcd.createChar(LCD_STR_CLOCK[0], clock);
        lcd.createChar(LCD_STR_REFRESH[0], refresh);
        lcd.createChar(LCD_STR_FOLDER[0], folder);
      }
    }
  #else
    lcd.createChar(LCD_STR_BEDTEMP[0], bedTemp);
    lcd.createChar(LCD_STR_DEGREE[0], degree);
    lcd.createChar(LCD_STR_THERMOMETER[0], thermometer);
	lcd.createChar(LCD_STR_CLOCK[0], clock);
    //lcd.createChar(LCD_STR_UPLEVEL[0], uplevel);
    lcd.createChar(LCD_STR_REFRESH[0], refresh);
    lcd.createChar(LCD_STR_FOLDER[0], folder);
    lcd.createChar(LCD_STR_FEEDRATE[0], feedrate);
	lcd.createChar(LCD_STR_THERMOMETERA[0], thermometera);
  #endif
	
	// ORIGINAL
	/*
	lcd.createChar(LCD_STR_BEDTEMP[0], bedTemp);
    lcd.createChar(LCD_STR_DEGREE[0], degree);
    lcd.createChar(LCD_STR_THERMOMETER[0], thermometer);
	lcd.createChar(LCD_STR_THERMOMETERA[0], thermometera);
    //lcd.createChar(LCD_STR_UPLEVEL[0], uplevel);
    lcd.createChar(LCD_STR_REFRESH[0], refresh);
    lcd.createChar(LCD_STR_FOLDER[0], folder);
    lcd.createChar(LCD_STR_FEEDRATE[0], feedrate);
    lcd.createChar(LCD_STR_CLOCK[0], clock);
	*/
    lcd.clear();
	
}
/*
static void lcd_set_custom_characters( //удалить! 
  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    bool progress_bar_set=true
  #endif
) {
   //MG icons
   byte bedTemp[8] =
    {
        B01001,
        B10010,
        B01001,
        B10010,
        B00000,
        B11111,
        B01010,
        B00000
    };
    byte degree[8] =
    {
        B01100,
        B10010,
        B10010,
        B01100,
        B00000,
        B00000,
        B00000,
        B00000
    };
    byte thermometer[8] =
    {
        B11111,
        B11011,
        B11011,
        B11111,
        B01010,
        B01010,
        B00100,
        B00000
    };
	byte thermometera[8] =
    {
        B11011,
        B11011,
        B11011,
        B11111,
        B01010,
        B01010,
        B00100,
        B00000
    };
    /*byte uplevel[8]={
        B00100,
        B01110,
        B11111,
        B00100,
        B11100,
        B00000,
        B00000,
        B00000
    };
    byte refresh[8]={
        B00000,
        B00110,
        B11001,
        B11000,
        B00011,
        B10011,
        B01100,
        B00000,
    };
    byte folder [8]={
        B00000,
        B11100,
        B11111,
        B10001,
        B10001,
        B11111,
        B00000,
        B00000
    };
	byte feedrate [8]={
        B00010,
        B00100,
        B01100,
        B11111,
        B00110,
        B00100,
        B01000,
        B10000
    };
    byte clock [8]={
        B00000,
        B01110,
        B10011,
        B10101,
        B10001,
        B01110,
        B00000,
        B00000
    };

  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    static bool char_mode = false;
    byte progress[3][8] = { {
      B00000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B00000
    }, {
      B00000,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B00000
    }, {
      B00000,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B00000
    } };
    if (progress_bar_set != char_mode) {
      char_mode = progress_bar_set;
      lcd.createChar(LCD_STR_BEDTEMP[0], bedTemp);
      lcd.createChar(LCD_STR_DEGREE[0], degree);
      lcd.createChar(LCD_STR_THERMOMETER[0], thermometer);
	  lcd.createChar(LCD_STR_THERMOMETERA[0], thermometera);
      lcd.createChar(LCD_STR_FEEDRATE[0], feedrate);
      lcd.createChar(LCD_STR_CLOCK[0], clock);
      if (progress_bar_set) {
        // Progress bar characters for info screen
        for (int i=3; i--;) lcd.createChar(LCD_STR_PROGRESS[i], progress[i]);
      }
      else {
        // Custom characters for submenus
        //lcd.createChar(LCD_STR_UPLEVEL[0], uplevel);
		lcd.createChar(LCD_STR_THERMOMETERA[0], thermometera);
        lcd.createChar(LCD_STR_REFRESH[0], refresh);
        lcd.createChar(LCD_STR_FOLDER[0], folder);
      }
    }
  #else
    lcd.createChar(LCD_STR_BEDTEMP[0], bedTemp);
    lcd.createChar(LCD_STR_DEGREE[0], degree);
    lcd.createChar(LCD_STR_THERMOMETER[0], thermometer);
	lcd.createChar(LCD_STR_THERMOMETERA[0], thermometera);
   // lcd.createChar(LCD_STR_UPLEVEL[0], uplevel);
    lcd.createChar(LCD_STR_REFRESH[0], refresh);
    lcd.createChar(LCD_STR_FOLDER[0], folder);
    lcd.createChar(LCD_STR_FEEDRATE[0], feedrate);
    lcd.createChar(LCD_STR_CLOCK[0], clock);
  #endif
}
*/
int initnum = 0; // MG INIT LOGO SHOW TIME -1 for disable
static void lcd_implementation_init(
  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    bool progress_bar_set=true
  #endif
) {

#if defined(LCD_I2C_TYPE_PCF8575)
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  #ifdef LCD_I2C_PIN_BL
    lcd.setBacklightPin(LCD_I2C_PIN_BL,POSITIVE);
    lcd.setBacklight(HIGH);
  #endif
  
#elif defined(LCD_I2C_TYPE_MCP23017)
    lcd.setMCPType(LTI_TYPE_MCP23017);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    lcd.setBacklight(0); //set all the LEDs off to begin with
    
#elif defined(LCD_I2C_TYPE_MCP23008)
    lcd.setMCPType(LTI_TYPE_MCP23008);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);

#elif defined(LCD_I2C_TYPE_PCA8574)
      lcd.init();
      lcd.backlight();
    
#else
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
#endif

//MG custom logo +
    
	/*lcd_set_custom_characters(
        #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            progress_bar_set
        #endif
    );*/
	/*
	lcd_restore_symbols_magnum(
		#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            progress_bar_set
        #endif
	);
	*/
	if (initnum >= 0) {
		byte magnum[8][8] = {
			{
				B00100,
				B01111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111
			},
			{
				B00000,
				B00000,
				B10001,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111
			},
			{
				B00110,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111
			},
			{
				B00000,
				B00000,
				B00000,
				B00000,
				B00000,
				B00000,
				B00000,
				B00000
			},
			{
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B01110
			},
			{
				B01110,
				B00100,
				B00000,
				B00000,
				B00000,
				B00000,
				B00000,
				B00000
			},
			{
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B11111,
				B01111
			},
			{
				B01110,
				B11011,
				B11011,
				B11000,
				B10111,
				B10111,
				B11011,
				B01111
			}
		};
		
		lcd.createChar(LCD_STR_BEDTEMP[0], magnum[0]);
		lcd.createChar(LCD_STR_DEGREE[0], magnum[1]);
		lcd.createChar(LCD_STR_THERMOMETER[0], magnum[2]);
		lcd.createChar(LCD_STR_THERMOMETERA[0], magnum[3]);
		//lcd.createChar(LCD_STR_UPLEVEL[0], magnum[3]);
		lcd.createChar(LCD_STR_REFRESH[0], magnum[4]);
		lcd.createChar(LCD_STR_FOLDER[0], magnum[5]);
		lcd.createChar(LCD_STR_FEEDRATE[0], magnum[6]);
		lcd.createChar(LCD_STR_CLOCK[0], magnum[7]);
		lcd.clear();	
		
		lcd.setCursor(8, 1);
		lcd.print(LCD_STR_BEDTEMP[0]);
		lcd.print(LCD_STR_DEGREE[0]);
		lcd.print(LCD_STR_THERMOMETER[0]);
		lcd.print(LCD_STR_THERMOMETERA[0]);
		//lcd.print(LCD_STR_UPLEVEL[0]);	
		
		lcd.setCursor(8, 2);
		lcd.print(LCD_STR_REFRESH[0]);
		lcd.print(LCD_STR_FOLDER[0]);
		lcd.print(LCD_STR_FEEDRATE[0]);
		lcd.print(LCD_STR_CLOCK[0]);
	} else {
		/*lcd_restore_symbols_magnum();
		lcd_set_custom_characters(
        #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            progress_bar_set
        #endif
		);*/
		
		lcd_restore_symbols_magnum(
		#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            progress_bar_set
        #endif
		);
		
		/*lcd_set_custom_characters(
        #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            progress_bar_set
        #endif
		);*/
	}
//MG custom logo -

}
static void lcd_implementation_clear()
{
    lcd.clear();
}
/* Arduino < 1.0.0 is missing a function to print PROGMEM strings, so we need to implement our own */
static void lcd_printPGM(const char* str)
{
    char c;
    while((c = pgm_read_byte(str++)) != '\0')
    {
        lcd.write(c);
    }
}
/*
Possible status screens:
16x2   |0123456789012345|
       |000/000 B000/000|
       |Status line.....|

16x4   |0123456789012345|
       |000/000 B000/000|
       |SD100%    Z000.0|
       |F100%     T--:--|
       |Status line.....|

20x2   |01234567890123456789|
       |T000/000D B000/000D |
       |Status line.........|

20x4   |01234567890123456789|
       |T000/000D B000/000D |
       |X+000.0 Y+000.0 Z+000.0|
       |F100%  SD100% T--:--|
       |Status line.........|

20x4   |01234567890123456789|
       |T000/000D B000/000D |
       |T000/000D     Z000.0|
       |F100%  SD100% T--:--|
       |Status line.........|
*/
static void lcd_implementation_status_screen()
{
	// MG ++ additional time to display init screen
	if (initnum > 0) {
		initnum --;
		return;
	} else
	if (initnum == 0) {
		lcd_restore_symbols_magnum();
		initnum --;
	}
	// загрузка прутка
	if (load_filament_now > 0) {
	//lcd.clear();
	//lcd.setCursor(0, 0);
		if (load_filament_now == 1) {
		lcd.setCursor(0,0);
		#if defined(MAGNUM_PRO)
		lcd_printPGM(PSTR("< Замена прутка "));
		if (active_extruder == 0) {
			lcd_printPGM(PSTR("1"));
		} else {
			lcd_printPGM(PSTR("2"));
		}
		lcd.print(LCD_STR_THERMOMETERA[0]);
		lcd_printPGM(PSTR(" >"));
		#else
			lcd_printPGM(PSTR("<   Замена прутка  >"));
		#endif
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR(" нагрев экструдера  "));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR(" температура: "));
		lcd.print(itostr3(int(degHotend(active_extruder)+0.5)));
		lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Пожалуйста подождите"));
		} else if (load_filament_now == 2) {
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("<=  Замена прутка =>"));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("  Загрузите новый   "));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR(" пруток в экструдер "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("  ОК для завершения "));
		} else if (load_filament_now == 3) {
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("<=  Замена прутка =>"));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("  Загрузите пруток  "));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR("в экструдер до упора"));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("ОК для начала подачи"));
		} else if (load_filament_now == 4) {
		/*lcd.setCursor(0,0);
		lcd_printPGM(PSTR("<   Замена прутка  >"));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR(" Дождитесь загрузки "));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR("пластика в экструдер"));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("ОК - чтобы завершить"));
		*/
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("<=  Замена прутка =>"));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("<Пруток загружается>"));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR("Потяните вытекающий "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("пластик из сопла ..."));
		
		}
	return; // не обновляем экран
	}
	
	
	
	
	
	
	
	// SW and PRO calibrate
	#if defined MAGNUM_PRO || defined SW_EXTRUDER
	if (sw_calibrate_now > 0) {
	//lcd.clear();
	lcd.setCursor(0, 0);
		if (sw_calibrate_now == 1) {
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("< Калибровка сопел >"));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR(" = расстояние XY = "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Пожалуйста подождите"));
		} else if (sw_calibrate_now == 2) {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("  Совместите метку  "));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("  с первым соплом  "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Далее ... (нажмите)"));
		} else if (sw_calibrate_now == 3) {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("  Вращая ручку <->  "));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("совместите с меткой "));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR("Смещение по X:"));
		lcd.print(ftostr32sp(current_position[X_AXIS]));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Далее ... (нажмите)"));
		} else if (sw_calibrate_now == 4) {
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("  Вращая ручку <^>  "));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("совместите с меткой "));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR("Смещение по Y:"));
		lcd.print(ftostr32sp(current_position[Y_AXIS]));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Готово .. (нажмите)"));
		}
	return; // не обновляем экран
	}
	#if defined SW_EXTRUDER
	if (sw_calibrate_z_now > 0) {
	//lcd.clear();
	lcd.setCursor(0, 0);
		if (sw_calibrate_z_now == 1) {
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("< Калибровка сопел >"));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR(" = высота сопел =  "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Пожалуйста подождите"));
		} else if (sw_calibrate_z_now == 2) {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("   Вращая ручку ^o  "));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("Отрегулируйте высоту"));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR("  1-го экструдера   "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Далее ... (нажмите)"));
		} else if (sw_calibrate_z_now == 3) {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("   Вращая ручку ^o  "));
		lcd.setCursor(0,1);
		lcd_printPGM(PSTR("Отрегулируйте высоту"));
		lcd.setCursor(0,2);
		lcd_printPGM(PSTR("  2-го экструдера   "));
		lcd.setCursor(0,3);
		lcd_printPGM(PSTR("Готово ... (нажмите)"));
		}
	return; // не обновляем экран
	}
	#endif
	#endif
	//MG+ Filament Monitor и пауза печати
	if (Filament_change_now > 0) {
		lcd.setCursor(0,0);
		lcd_printPGM(PSTR("<   Замена прутка  >"));
		#if defined FILAMENT_MONITOR
		if (load_failed) {
			lcd.setCursor(0, 0);
			if (READ(LASER_PIN) == 1) {
				lcd_printPGM(PSTR("!Пруток не загружен!"));
			} else {
				lcd_printPGM(PSTR("   Пруток загружен  "));
			}
		}
		#endif
		if (Filament_change_now == 1 ) { // сменить пруток или суперпауза
			lcd.setCursor(0, 1);
			lcd_printPGM(PSTR("  ----------------  "));
			lcd.setCursor(0, 2);
			lcd_printPGM(PSTR("   < Пожалуйста >   "));
			lcd.setCursor(0, 3);
			lcd_printPGM(PSTR("   < подождите! >   "));
		} else if (Filament_change_now == 2 ) { // пришли в процедуру
			if (jam_detected == 1) {
				lcd.setCursor(0, 1);
				lcd_printPGM(PSTR("<  Монитор прутка  >"));
				lcd.setCursor(0, 2);
				lcd_printPGM(PSTR("   Нажмите OK для   "));
				lcd.setCursor(0, 3);
				lcd_printPGM(PSTR(" включения нагрева! "));
			} else if (jam_detected == 2) { // вызов компандой M600 S
				lcd.setCursor(0, 1);
				lcd_printPGM(PSTR("< Расширенная Пауза>"));
				lcd.setCursor(0, 2);
				lcd_printPGM(PSTR("   Нажмите OK для   "));
				lcd.setCursor(0, 3);
				lcd_printPGM(PSTR(" включения нагрева! "));
			} else if (jam_detected == 3) {
				lcd.setCursor(0, 1);
				lcd_printPGM(PSTR("1.Дождитесь нагрева "));
				lcd.setCursor(0, 2);
				lcd_printPGM(PSTR("2.Загрузите пруток  "));
				lcd.setCursor(0, 3);
				lcd_printPGM(PSTR("3.Нажмите для печати"));
			} else if (jam_detected == 4) {
				lcd.setCursor(0, 1);
				lcd_printPGM(PSTR("<Пруток не загружен>"));
				lcd.setCursor(0, 2);
				lcd_printPGM(PSTR("  Загрузите пруток! "));
				lcd.setCursor(0, 3);
				lcd_printPGM(PSTR(" Нажмите для печати "));
			//} else if (Filament_change_now == 2 ) { 
			} else { 
				// Просто принята команда M600
				#if defined SW_EXTRUDER
					if (reload_filament_sw == 1 ) { 
						lcd.setCursor(0,0);
						lcd_printPGM(PSTR("<   Замена прутка  >"));
						lcd.setCursor(0,1);
						lcd_printPGM(PSTR(" Подготовьте пруток "));
						lcd.setCursor(0,2);
						lcd_printPGM(PSTR("    для загрузки    "));
						lcd.setCursor(0,3);
						lcd_printPGM(PSTR("ОК для начала подачи"));
					} else if (reload_filament_sw == 2 ) { // загружаем
						lcd.setCursor(0,0);
						lcd_printPGM(PSTR("<   Замена прутка  >"));
						lcd.setCursor(0,1);
						lcd_printPGM(PSTR("  Загрузите новый   "));
						lcd.setCursor(0,2);
						lcd_printPGM(PSTR(" пруток в экструдер "));
						lcd.setCursor(0,3);
						lcd_printPGM(PSTR("ОК - чтобы завершить"));
					} 
					
				
				#else
					lcd.setCursor(0, 1);
					lcd_printPGM(PSTR("< Замените пруток  >"));
					lcd.setCursor(0, 2);
					lcd_printPGM(PSTR("   Нажмите OK для   "));
					lcd.setCursor(0, 3);
					lcd_printPGM(PSTR(" продолжения печати "));
				#endif
			}
		}
	return; // не обновляем экран
	}
	//MG- Filament Monitor и пауза печати
	
    int tHotend=int(degHotend(0) + 0.5);
    int tTarget=int(degTargetHotend(0) + 0.5);

#if LCD_WIDTH < 20
    lcd.setCursor(0, 0);
    lcd.print(itostr3(tHotend));
    lcd.print('/');
    lcd.print(itostr3left(tTarget));

# if EXTRUDERS > 1 || TEMP_SENSOR_BED != 0
    //If we have an 2nd extruder or heated bed, show that in the top right corner
    lcd.setCursor(8, 0);
#  if EXTRUDERS > 1
    tHotend = int(degHotend(1) + 0.5);
    tTarget = int(degTargetHotend(1) + 0.5);
    lcd.print(LCD_STR_THERMOMETER[0]);
#  else//Heated bed
    tHotend=int(degBed() + 0.5);
    tTarget=int(degTargetBed() + 0.5);
    lcd.print(LCD_STR_BEDTEMP[0]);
#  endif
    lcd.print(itostr3(tHotend));
    lcd.print('/');
    lcd.print(itostr3left(tTarget));
# endif//EXTRUDERS > 1 || TEMP_SENSOR_BED != 0

#else//LCD_WIDTH > 19
    lcd.setCursor(0, 0);
	#if defined(MAGNUM_PRO)
	 lcd.print('1');
	 if (active_extruder == 0) {
		lcd.print(LCD_STR_THERMOMETERA[0]);
	 } else {
		lcd.print(LCD_STR_THERMOMETER[0]);
	 }
	#else
		lcd.print(LCD_STR_THERMOMETER[0]);	
	#endif
	#if defined(MGLASER)
	if (laser_enable) {
		lcd_printPGM(PSTR(" Лазер"));
		if (READ(LASER_PIN)) {
			lcd_printPGM(PSTR(" *"));
		} else {
			lcd_printPGM(PSTR("  ")); 
		}
	} else {
		if (pasta_enabled || pasta_dir_enabled) {
			lcd_printPGM(PSTR(" Паста"));
		} else {
			lcd.print(itostr3(tHotend));
			lcd.print('/');
			lcd.print(itostr3left(tTarget));
			lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
		}
	}
	#else
	if (pasta_enabled || pasta_dir_enabled) {
		lcd_printPGM(PSTR(" Паста"));
	} else {
		lcd.print(itostr3(tHotend));
		lcd.print('/');
		lcd.print(itostr3left(tTarget));
		lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
	}
	#endif //defined(MGLASER)
    if (tTarget < 10) lcd.print(' ');

	# if EXTRUDERS > 1 || TEMP_SENSOR_BED != 0
    //If we have an 2nd extruder or heated bed, show that in the top right corner
    //MG
	//lcd.setCursor(10, 0);
	lcd.setCursor(11, 0);
#  if EXTRUDERS > 1 && !defined(MAGNUM_PRO) //MG - ext1 to second line
    tHotend = int(degHotend(1) + 0.5);
    tTarget = int(degTargetHotend(1) + 0.5);
    lcd.print(LCD_STR_THERMOMETER[0]);
#  else//Heated bed
    tHotend=int(degBed() + 0.5);
    tTarget=int(degTargetBed() + 0.5);
    lcd.print(LCD_STR_BEDTEMP[0]);
#  endif
    lcd.print(itostr3(tHotend));
    lcd.print('/');
    lcd.print(itostr3left(tTarget));
    lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
    if (tTarget < 10)
        lcd.print(' ');
# endif//EXTRUDERS > 1 || TEMP_SENSOR_BED != 0
#endif//LCD_WIDTH > 19

#if LCD_HEIGHT > 2
//Lines 2 for 4 line LCD
# if LCD_WIDTH < 20
#  ifdef SDSUPPORT
    lcd.setCursor(0, 2);
    lcd_printPGM(PSTR("SD"));
    if (IS_SD_PRINTING)
        lcd.print(itostr3(card.percentDone()));
    else
        lcd_printPGM(PSTR("---"));
    lcd.print('%');
#  endif//SDSUPPORT
# else//LCD_WIDTH > 19
#  if EXTRUDERS > 1 && TEMP_SENSOR_BED != 0
    //If we both have a 2nd extruder and a heated bed, show the heated bed temp on the 2nd line on the left, as the first line is filled with extruder temps
    #if defined(MAGNUM_PRO)
	//MG - ext1 to second line
	lcd.setCursor(0, 1);
	lcd.print('2');
	 if (active_extruder == 1) {
		lcd.print(LCD_STR_THERMOMETERA[0]);
	 } else {
		lcd.print(LCD_STR_THERMOMETER[0]);
	 }
	tHotend = int(degHotend(1) + 0.5);
    tTarget = int(degTargetHotend(1) + 0.5);
	lcd.print(itostr3(tHotend));
    lcd.print('/');
    lcd.print(itostr3left(tTarget));
	lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
	if (tTarget < 10) lcd.print(' ');
	#else
	tHotend=int(degBed() + 0.5);
    tTarget=int(degTargetBed() + 0.5);
    lcd.setCursor(0, 1);
    lcd.print(LCD_STR_BEDTEMP[0]);
    lcd.print(itostr3(tHotend));
    lcd.print('/');
    lcd.print(itostr3left(tTarget));
    lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
    if (tTarget < 10)
        lcd.print(' ');
	#endif //MG
#  else
    lcd.setCursor(0,1);
    lcd.print('X');
    lcd.print(ftostr3(current_position[X_AXIS]));
    lcd_printPGM(PSTR(" Y"));
    lcd.print(ftostr3(current_position[Y_AXIS]));
#  endif//EXTRUDERS > 1 || TEMP_SENSOR_BED != 0
# endif//LCD_WIDTH > 19
    lcd.setCursor(LCD_WIDTH - 8, 1);
    lcd.print('Z');
    lcd.print(ftostr32sp(current_position[Z_AXIS] + 0.00001));
#endif//LCD_HEIGHT > 2

#if LCD_HEIGHT > 3
    lcd.setCursor(0, 2);
    lcd.print(LCD_STR_FEEDRATE[0]);
    lcd.print(itostr3(feedmultiply));
    lcd.print('%');
	lcd_printPGM(PSTR("  "));
# if LCD_WIDTH > 19
#  ifdef SDSUPPORT
    lcd.setCursor(7, 2);
    lcd_printPGM(PSTR("SD"));
    if (IS_SD_PRINTING)
        lcd.print(itostr3(card.percentDone()));
    else
        lcd_printPGM(PSTR("---"));
    lcd.print('%');
#  endif//SDSUPPORT
# endif//LCD_WIDTH > 19
    //lcd.setCursor(LCD_WIDTH - 6, 2);
	lcd.setCursor(LCD_WIDTH - 5, 2);
    //lcd.print(LCD_STR_CLOCK[0]);
    if(starttime != 0)
    {
        uint16_t time = millis()/60000 - starttime/60000;
        lcd.print(itostr2(time/60));
        lcd.print(':');
        lcd.print(itostr2(time%60));
    }else{
        lcd_printPGM(PSTR("--:--"));
    }
#endif

  //MG
  /*if (pasta_enabled) {
	lcd.setCursor(LCD_WIDTH - 1, 0);
	lcd.print('P');
  } else {*/
  	lcd.setCursor(LCD_WIDTH - 1, 0);
	lcd.print(" ");  
  
  //MG SW для отладки SW
  #ifdef SW_EXTRUDER
	if (sw_test) {
		extern unsigned long sw_on_timer_show;
		lcd.setCursor(0, 3);
		lcd_printPGM(PSTR("                    "));
		lcd.setCursor(0, 3);
		lcd_printPGM(PSTR("Вр: "));
		lcd.print(sw_on_timer_show);
		//lcd_printPGM(PSTR(" mc"));
		lcd_printPGM(PSTR(" Cnt: "));
		lcd.print(sw_test);
	return;
	}
	if (READ(SERVICE_PIN) == 0) {
		extern unsigned long sw_on_timer_show;
		lcd.setCursor(0, 3);
		lcd_printPGM(PSTR("                    "));
		lcd.setCursor(0, 3);
		lcd_printPGM(PSTR("Время: "));
		lcd.print(sw_on_timer_show);
		lcd_printPGM(PSTR(" mc"));
		if (sw_timeout) {
			lcd_printPGM(PSTR(" TIMER!"));	
		} else {lcd_printPGM(PSTR("       "));}
	return;
	}
  #endif
  // Status message line at the bottom
  lcd.setCursor(0, LCD_HEIGHT - 1);
  
    
  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    if (card.isFileOpen()) {
      uint16_t mil = millis(), diff = mil - progressBarTick;
      if (diff >= PROGRESS_BAR_MSG_TIME || !lcd_status_message[0]) {
        // draw the progress bar
        int tix = (int)(card.percentDone() * LCD_WIDTH * 3) / 100,
          cel = tix / 3, rem = tix % 3, i = LCD_WIDTH;
        char msg[LCD_WIDTH+1], b = ' ';
        msg[i] = '\0';
        while (i--) {
          if (i == cel - 1)
            b = LCD_STR_PROGRESS[2];
          else if (i == cel && rem != 0)
            b = LCD_STR_PROGRESS[rem-1];
          msg[i] = b;
        }
        lcd.print(msg);
        return;
      }
    } //card.isFileOpen
	#endif //LCD_PROGRESS_BAR

  //Display both Status message line and Filament display on the last line
  #ifdef FILAMENT_LCD_DISPLAY
    if (message_millis + 5000 <= millis()) {  //display any status for the first 5 sec after screen is initiated
      lcd_printPGM(PSTR("Dia "));
      lcd.print(ftostr12ns(filament_width_meas));
      lcd_printPGM(PSTR(" V"));
      lcd.print(itostr3(100.0*volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM]));
  	  lcd.print('%');
  	  return;
    }
  #endif //FILAMENT_LCD_DISPLAY
 // END if jam_detected
	/*
	#ifdef SW_EXTRUDER
 	if (sw_test) {
		lcd_printPGM(PSTR("                    "));
		lcd.setCursor(0, LCD_HEIGHT - 1);
		lcd_printPGM(PSTR("Test Count: "));
		lcd.print(sw_test);
	} else {
		lcd.print(lcd_status_message);
    }
	#endif
	#ifndef SW_EXTRUDER
		lcd.print(lcd_status_message);
	#endif
	*/
	lcd.print(lcd_status_message);
	//MG пруток для отладки
	/*
	lcd_printPGM(PSTR("                    "));
	lcd.setCursor(0, LCD_HEIGHT - 1);
	lcd_printPGM(PSTR("LASER_PIN "));
	lcd.print(READ(LASER_PIN));
	*/
	
	//MG SW для отладки
	/*
	lcd_printPGM(PSTR("                    "));
	lcd.setCursor(0, LCD_HEIGHT - 1);
	lcd_printPGM(PSTR("T1 "));
	lcd.print(READ(SW_T0_PIN));
	lcd_printPGM(PSTR(" T2 "));
	lcd.print(READ(SW_T1_PIN));
	if (sw_timeout) {
		lcd_printPGM(PSTR(" TIMER!"));	
	} else {lcd_printPGM(PSTR("       "));}
	*/
	
	
	/*lcd_printPGM(PSTR("DIR"));
	lcd.print(READ(SW_DIR_PIN));
	lcd_printPGM(PSTR(" EN"));
	lcd.print(READ(SW_EN_PIN));
	lcd_printPGM(PSTR(" CS"));
	lcd.print(sw_current);
	lcd.setCursor(17, LCD_HEIGHT - 1);
	lcd.print(sw_current_max);*/
	//lcd.print(analogRead(SW_CS_PIN));
  
}


static void lcd_implementation_drawmenu_generic(uint8_t row, const char* pstr, char pre_char, char post_char)
{
    uint8_t c;
    //Use all characters in narrow LCDs
  #if LCD_WIDTH < 20
      uint8_t n = LCD_WIDTH - 1 - 1;
    #else
      uint8_t n = LCD_WIDTH - 1 - 2;
  #endif
  	uint8_t u = 0;
    lcd.setCursor(0, row);
    lcd.write(pre_char);
	while(n > 0)
    {
		c = pgm_read_byte(pstr);
		if (c == 0x0) {
			break;
		}
		if (c == 0xd0 || c == 0xd1) { // utf8 high part
			lcd.write(c);
			pstr++;			
			c = pgm_read_byte(pstr);
			lcd.write(c);
			pstr++;
			n--;
		} else {
			lcd.write(c);
			pstr++;
			n--;
		}
    }
    while(n--) lcd.print(' ');
    lcd.write(post_char);
    lcd.write(' ');
}
static void lcd_implementation_drawmenu_setting_edit_generic(uint8_t row, const char* pstr, char pre_char, char* data)
{
    uint8_t c;
    //Use all characters in narrow LCDs
  #if LCD_WIDTH < 20
      uint8_t n = LCD_WIDTH - 1 - 1 - strlen(data);
    #else
      uint8_t n = LCD_WIDTH - 1 - 2 - strlen(data);
  #endif
	uint8_t u = 0;
    lcd.setCursor(0, row);
    lcd.write(pre_char);
	while(n > 0) 
	{
		c = pgm_read_byte(pstr);
		if (c == 0x0) {
			break;
		}
		if (c == 0xd0 || c == 0xd1) { // utf8 high part
			lcd.write(c);
			pstr++;			
			c = pgm_read_byte(pstr);
			lcd.write(c);
			pstr++;
			n--;
		} else {
			lcd.write(c);
			pstr++;
			n--;
		}
    }
	lcd.print(':');
    while(n--) 
        lcd.print(' ');
    lcd.print(data);
}
static void lcd_implementation_drawmenu_setting_edit_generic_P(uint8_t row, const char* pstr, char pre_char, const char* data)
{
    uint8_t c;
    //Use all characters in narrow LCDs
  #if LCD_WIDTH < 20
      uint8_t n = LCD_WIDTH - 1 - 1 - strlen_P(data);
    #else
	  //MG 
	  uint8_t n = LCD_WIDTH + 4 - 1 - 2 - strlen_P(data);
      //uint8_t n = LCD_WIDTH - 1 - 2 - strlen_P(data);
  #endif
   	uint8_t u = 0;
    lcd.setCursor(0, row);
    lcd.write(pre_char);
   	while(n > 0)
    {
		c = pgm_read_byte(pstr);
		if (c == 0x0) {
			break;
		}
	
		if (c == 0xd0 || c == 0xd1) { // utf8 high part
			lcd.write(c);
			pstr++;			
			c = pgm_read_byte(pstr);
			lcd.write(c);
			pstr++;
			n--;
		} else {
			lcd.write(c);
			pstr++;
			n--;
		}
    }
    lcd.print(':');
    while(n--)
        lcd.print(' ');
    lcd_printPGM(data);
}
#define lcd_implementation_drawmenu_setting_edit_int3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_int3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float43_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float43(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_bool_selected(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_bool(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))

//Add version for callback functions
#define lcd_implementation_drawmenu_setting_edit_callback_int3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_int3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float43_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float43(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_bool_selected(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_callback_bool(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))


void lcd_implementation_drawedit(const char* pstr, char* value)
{
    lcd.setCursor(1, 1);
    lcd_printPGM(pstr);
    lcd.print(':');
   #if LCD_WIDTH < 20
      lcd.setCursor(LCD_WIDTH - strlen(value), 1);
    #else
      lcd.setCursor(LCD_WIDTH -1 - strlen(value), 1);
   #endif
    lcd.print(value);
}
static void lcd_implementation_drawmenu_sdfile_selected(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 1;
    lcd.setCursor(0, row);
    lcd.print('>');
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-1] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd.print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd.print(' ');
}
static void lcd_implementation_drawmenu_sdfile(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 1;
    lcd.setCursor(0, row);
    lcd.print(' ');
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-1] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd.print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd.print(' ');
}
static void lcd_implementation_drawmenu_sddirectory_selected(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd.setCursor(0, row);
    lcd.print('>');
    lcd.print(LCD_STR_FOLDER[0]);
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-2] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd.print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd.print(' ');
}
static void lcd_implementation_drawmenu_sddirectory(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd.setCursor(0, row);
    lcd.print(' ');
    lcd.print(LCD_STR_FOLDER[0]);
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-2] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd.print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd.print(' ');
}
#define lcd_implementation_drawmenu_back_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, LCD_STR_UPLEVEL[0], LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_back(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_submenu_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_submenu(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_gcode_selected(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_gcode(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')
#define lcd_implementation_drawmenu_function_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_function(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')

static void lcd_implementation_quick_feedback()
{
#ifdef LCD_USE_I2C_BUZZER
	#if !defined(LCD_FEEDBACK_FREQUENCY_HZ) || !defined(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
	  lcd_buzz(1000/6,100);
	#else
	  lcd_buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS,LCD_FEEDBACK_FREQUENCY_HZ);
	#endif
#elif defined(BEEPER) && BEEPER > -1
    SET_OUTPUT(BEEPER);
	#if !defined(LCD_FEEDBACK_FREQUENCY_HZ) || !defined(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
    for(int8_t i=0;i<10;i++)
    {
      WRITE(BEEPER,HIGH);
      delayMicroseconds(100);
      WRITE(BEEPER,LOW);
      delayMicroseconds(100);
    }
    #else
    for(int8_t i=0;i<(LCD_FEEDBACK_FREQUENCY_DURATION_MS / (1000 / LCD_FEEDBACK_FREQUENCY_HZ));i++)
    {
      WRITE(BEEPER,HIGH);
      delayMicroseconds(1000000 / LCD_FEEDBACK_FREQUENCY_HZ / 2);
      WRITE(BEEPER,LOW);
      delayMicroseconds(1000000 / LCD_FEEDBACK_FREQUENCY_HZ / 2);
    }
    #endif
#endif
}

#ifdef LCD_HAS_STATUS_INDICATORS
static void lcd_implementation_update_indicators()
{
  #if defined(LCD_I2C_PANELOLU2) || defined(LCD_I2C_VIKI)
    //set the LEDS - referred to as backlights by the LiquidTWI2 library 
    static uint8_t ledsprev = 0;
    uint8_t leds = 0;
    if (target_temperature_bed > 0) leds |= LED_A;
    if (target_temperature[0] > 0) leds |= LED_B;
    if (fanSpeed) leds |= LED_C;
    #if EXTRUDERS > 1  
      if (target_temperature[1] > 0) leds |= LED_C;
    #endif
    if (leds != ledsprev) {
      lcd.setBacklight(leds);
      ledsprev = leds;
    }
  #endif
}
#endif

#ifdef LCD_HAS_SLOW_BUTTONS
extern uint32_t blocking_enc;

static uint8_t lcd_implementation_read_slow_buttons()
{
  #ifdef LCD_I2C_TYPE_MCP23017
  uint8_t slow_buttons;
    // Reading these buttons this is likely to be too slow to call inside interrupt context
    // so they are called during normal lcd_update
    slow_buttons = lcd.readButtons() << B_I2C_BTN_OFFSET; 
    #if defined(LCD_I2C_VIKI)
    if(slow_buttons & (B_MI|B_RI)) { //LCD clicked
       if(blocking_enc > millis()) {
         slow_buttons &= ~(B_MI|B_RI); // Disable LCD clicked buttons if screen is updated
       }
    }
    #endif
    return slow_buttons; 
  #endif
}
#endif

#endif//ULTRA_LCD_IMPLEMENTATION_HITACHI_HD44780_H
