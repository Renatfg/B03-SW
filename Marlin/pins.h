#ifndef PINS_H
#define PINS_H

#include "boards.h"

#if !MB(5DPRINT)
#define X_MS1_PIN -1
#define X_MS2_PIN -1
#define Y_MS1_PIN -1
#define Y_MS2_PIN -1
#define Z_MS1_PIN -1
#define Z_MS2_PIN -1
#define E0_MS1_PIN -1
#define E0_MS2_PIN -1
#define E1_MS1_PIN -1
#define E1_MS2_PIN -1
#define DIGIPOTSS_PIN -1
#endif

#if MBOARD == 2 // MKS GEN_L v2.0/1
#define KNOWN_BOARD 1

#ifndef __AVR_ATmega2560__
 #error Oops!  Make sure you have 'Arduino Mega 2560' selected from the 'Tools -> Boards' menu.
#endif

#define LARGE_FLASH        true
#define DISABLE_JTAG       true

#if defined(MGLASER) || defined(FILAMENT_MONITOR)
 #define LASER_PIN	33 // вывод 5 внешнего разъема
#endif


 #define X_STEP_PIN          54
 #define X_DIR_PIN           55
 #define X_ENABLE_PIN        38

 #define Y_STEP_PIN          60
 #define Y_DIR_PIN           61
 #define Y_ENABLE_PIN        56

 #define Z_STEP_PIN          46
 #define Z_DIR_PIN           48
 #define Z_ENABLE_PIN        62   


#define E0_STEP_PIN         26
#define E0_DIR_PIN          28
#define E0_ENABLE_PIN       24

#define E1_STEP_PIN        26
#define E1_DIR_PIN         28
#define E1_ENABLE_PIN      24

#undef  INVERT_E1_DIR
#define INVERT_E1_DIR 		false

//Pasta
//extern bool pasta_enabled;
//extern bool pasta_dir_enabled;
/*
#define E3_STEP_PIN         46
#define E3_DIR_PIN          37
#define E3_ENABLE_PIN       34
#define INVERT_E3_DIR 		true
*/
// изза пасты этот блок нужен, см. WRITE(E3_ENABLE_PIN, !E_ENABLE_ON);
#define E3_STEP_PIN         4 //SERVO2 разъем!
#define E3_DIR_PIN          4
#define E3_ENABLE_PIN       4
#define INVERT_E3_DIR 		true
//-

// Temperature Sensors
//
#define TEMP_0_PIN         13 
#if defined(MAGNUM_PRO)
	#define TEMP_1_PIN     15 
#else
	#define TEMP_1_PIN     -1 
#endif  
#define TEMP_2_PIN         -1
#define TEMP_BED_PIN       14  

//
// Heaters / Fans
//

#define HEATER_0_PIN       10
#if defined(MAGNUM_PRO)
	#define HEATER_1_PIN   7
#else
	#define HEATER_1_PIN   -1 
#endif
#define HEATER_BED_PIN     8   // 6 если на переходной плате мосфет

#define FAN_PIN        	   57 // А3
//#define FAN1_PIN       MOSFET_D_PIN

#define STAT_LED_RED 	47
#define STAT_LED_BLUE 	45


#define X_STOP_PIN         4
#define Y_STOP_PIN         58 // А4 
#define Z_STOP_PIN         5

#define SDSS               53
#define SDCARDDETECT       49

#define SDPOWER            -1
#define LED_PIN            -1
#define PS_ON_PIN          -1
#define KILL_PIN           -1
#define ALARM_PIN          -1

#define DAC_STEPPER_CURRENT
#define DAC_STEPPER_ADDRESS	0
#define DAC_STEPPER_ORDER 	{3,2,1,0}
#define DAC_STEPPER_MAX 	3500
#define DAC_STEPPER_VREF 	1 //internal Vref, gain 1x = 2.048V
#define DAC_STEPPER_GAIN	0

#if defined(SW_EXTRUDER)
	#define SERVICE_PIN 		31
	#define SW_DIR_PIN			32
	#define SW_EN_PIN			39
	#define SW_T0_PIN			19 // Hall 2
	#define SW_T1_PIN			18 // Hall 1
#endif

#define BEEPER -1

   #define LCD_PINS_RS 27
   #define LCD_PINS_ENABLE 29
   #define LCD_PINS_D4 23
   #define LCD_PINS_D5 25
   #define LCD_PINS_D6 17
   #define LCD_PINS_D7 16

  #define BTN_EN1   37
  #define BTN_EN2   35
  #define BTN_ENC   2 //the click

 /* #define BLEN_C 2
  #define BLEN_B 1
  #define BLEN_A 0*/
  
  //encoder rotation values
  #define encrot0 0
  #define encrot1 2
  #define encrot2 3
  #define encrot3 1

/*
#ifndef SDSUPPORT
// these pins are defined in the SD library if building with SD support
  #define SCK_PIN          52
  #define MISO_PIN         50
  #define MOSI_PIN         51
#endif
*/

#endif  // MBOARD == 2 B03c

#if MBOARD == 1 //B03c
/****************************************************************************************
* Magnum 3D
// AT90USBxx_TEENSYPP_ASSIGNMENTS -- Use Teensyduino Teensy++2.0 assignments.

    /**

    AT90USB  51 50 49 48 47 46 45 44 10 11 12 13 14 15 16 17 35 36 37 38 39 40 41 42 25 26 27 28 29 30 31 32 33 34 43 09 18 19 01 02 61 60 59 58 57 56 55 54
    Port     A0 A1 A2 A3 A4 A5 A6 A7 B0 B1 B2 B3 B4 B5 B6 B7 C0 C1 C2 C3 C4 C5 C6 C7 D0 D1 D2 D3 D4 D5 D6 D7 E0 E1 E2 E3 E4 E5 E6 E7 F0 F1 F2 F3 F4 F5 F6 F7
    Marlin   00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
    Teensy   28 29 30 31 32 33 34 35 20 21 22 23 24 25 26 27 10 11 12 13 14 15 16 17 00 01 02 03 04 05 06 07 08 09(46*47)36 37 18 19 38 39 40 41 42 43 44 45
             The pins 46 and 47 are not supported by Teensyduino, but are supported below.
    */
/****************************************************************************************/
#define KNOWN_BOARD 1
#define AT90USB 1286  // Disable MarlinSerial etc.
#define AT90USBxx_TEENSYPP_ASSIGNMENTS

#ifndef __AVR_AT90USB1286__
#error Oops!  Make sure you have 'Teensy++ 2.0' selected from the 'Tools -> Boards' menu.
#endif

#define LARGE_FLASH        true

// X, Y, Z, E hysteresis in mm. These are the extra distances that are performed when an axis changes direction to compensate for any mechanical hysteresis your printer has.
//#define AXIS_HYSTERESIS

#define DISABLE_JTAG       true

#if defined(MGLASER) || defined(FILAMENT_MONITOR)
 #define LASER_PIN	42 // EX-TERM (вывод 5 внешнего разъема)
#endif

//#define Z_MIN_PROBE_PIN 34 // E3_ENABLE_PIN  -- нельзя т.к. boot и не включается принтер!

#if defined(M2000)
 #define X_STEP_PIN          29
 #define X_DIR_PIN           31 
 #define X_ENABLE_PIN        32

 #define Y_STEP_PIN          28
 #define Y_DIR_PIN           30
 #define Y_ENABLE_PIN        33

 #define Z_STEP_PIN          46
 #define Z_DIR_PIN           37
 #define Z_ENABLE_PIN        44 
#else
 #define X_STEP_PIN          0
 #define X_DIR_PIN           1
 #define X_ENABLE_PIN       39

 #define Y_STEP_PIN          2
 #define Y_DIR_PIN           3
 #define Y_ENABLE_PIN       38

 #define Z_STEP_PIN          4
 #define Z_DIR_PIN           5
 #define Z_ENABLE_PIN       23   
#endif

#define E0_STEP_PIN         6
#define E0_DIR_PIN          7
#define E0_ENABLE_PIN      19

//Pasta
//extern bool pasta_enabled;
//extern bool pasta_dir_enabled;

// старая настройка пасты, не работает STEP со свичем (DIR и ENABLE работают)
//#define E3_STEP_PIN         46 // EX-1 STEP
//#define E3_DIR_PIN          37
//#define E3_ENABLE_PIN       34

//Новая настройка чтобы работало с переходной платой
// для переходной нужно будет ENABLE поставить кнопку - масса включено, +5 вольт отключен двигатель
// т.е. можно вообще не подключать контакт и будет двигатель всегда включен
#define E3_STEP_PIN         34 // вывод 2 - EX_ENABLE (Service)
#define E3_DIR_PIN          42 // вывод 5 - EX_TERM (laser) 
#define E3_ENABLE_PIN       37 // вывод 4 - EX-1 DIR  - SW_DIR_PIN

#define INVERT_E3_DIR 		true

// изза пасты этот блок нужен, см. WRITE(E3_ENABLE_PIN, !E_ENABLE_ON);
/*#define E3_STEP_PIN         34
#define E3_DIR_PIN          34
#define E3_ENABLE_PIN       34
#define INVERT_E3_DIR 		true
*/
//-

#define HEATER_0_PIN       21  // Extruder 0
#define TEMP_0_PIN          1  // Extruder / Analog pin numbering

#if defined(MAGNUM_PRO)
  #define TEMP_1_PIN         3
  #if defined(SW_EXTRUDER)
	
	#define SERVICE_PIN 34 // boot (E3_ENABLE_PIN) вход в серсное меню
	
	#define HEATER_1_PIN       14
	#define HEATER_BED_PIN     20  // Bed
	
	#define E1_STEP_PIN        6
	#define E1_DIR_PIN         7
	#define E1_ENABLE_PIN      19
	
	#undef  INVERT_E1_DIR
	#define INVERT_E1_DIR 		false
	
	#define SW_DIR_PIN			37  // EX-1 DIR
	#define SW_EN_PIN			44  // E1-Enable
	#define SW_T0_PIN			46  // EX-1 STEP
	#define SW_T1_PIN			13  
  #else
	#define HEATER_1_PIN       14
	#define HEATER_BED_PIN     20  // Bed
	#define E1_STEP_PIN        46
	#define E1_DIR_PIN         37
	#define E1_ENABLE_PIN      44
  #endif
#else 
	//#define HEATER_1_PIN       14
	#define HEATER_BED_PIN     20  // Bed
#endif

#if !defined(HEATER_1_PIN)
 #define HEATER_1_PIN       -1
 #define TEMP_1_PIN         -1
#endif
#if !defined(HEATER_2_PIN)
 #define HEATER_2_PIN       -1
 #define TEMP_2_PIN         -1
#endif


#define TEMP_BED_PIN        0  // Bed / Analog pin numbering

#define STAT_LED_RED 3
#define STAT_LED_BLUE 2

// If soft or fast PWM is off then use Teensyduino pin numbering, Marlin
// fastio pin numbering otherwise
#ifdef FAN_SOFT_PWM || FAST_PWM_FAN
	#define FAN_PIN        22  // Fan
#else
	#define FAN_PIN        16  // Fan
#endif

#define X_STOP_PIN         35
#define Y_STOP_PIN         12 //8 for B01

#define Z_STOP_PIN         36 // стандартный микрик
/*#if defined(AUTO_MI_PROBE)
	#define Z_STOP_PIN         37 // E3_DIR_PIN (вывод 4 внешнего разъема)
#else
	#define Z_STOP_PIN         36 // стандартный микрик
#endif*/

#define SDSS               20
#define SDCARDDETECT       15//27

#define SDPOWER            -1
#define LED_PIN            -1
#define PS_ON_PIN          -1
#define KILL_PIN           -1
#define ALARM_PIN          -1

#define DAC_STEPPER_CURRENT
#define DAC_STEPPER_ADDRESS	0
#define DAC_STEPPER_ORDER 	{3,2,1,0}
#define DAC_STEPPER_MAX 	3500
#define DAC_STEPPER_VREF 	1 //internal Vref, gain 1x = 2.048V
#define DAC_STEPPER_GAIN	0

#ifdef ULTRA_LCD
  #define BEEPER -1

  #if defined(MG_LCD_I2C)
   #define LCD_PINS_RS -1
   #define LCD_PINS_ENABLE -1
   #define LCD_PINS_D4 -1
   #define LCD_PINS_D5 -1
   #define LCD_PINS_D6 -1
   #define LCD_PINS_D7 -1  
  #else
   #define LCD_PINS_RS 8
   #define LCD_PINS_ENABLE 9
   #define LCD_PINS_D4 4
   #define LCD_PINS_D5 5
   #define LCD_PINS_D6 6
   #define LCD_PINS_D7 7
  #endif

  #define BTN_EN1   16
  #define BTN_EN2   17
  #define BTN_ENC   18//the click

  #define BLEN_C 2
  #define BLEN_B 1
  #define BLEN_A 0
  
  //encoder rotation values
  #define encrot0 0
  #define encrot1 2
  #define encrot2 3
  #define encrot3 1
#endif

#ifndef SDSUPPORT
// these pins are defined in the SD library if building with SD support
  #define SCK_PIN           9
  #define MISO_PIN         11
  #define MOSI_PIN         10
#endif

#endif  // MBOARD == 1 B03c


#ifndef KNOWN_BOARD
#error Unknown MOTHERBOARD value in configuration.h
#endif

//List of pins which to ignore when asked to change by gcode, 0 and 1 are RX and TX, do not mess with those!
#define _E0_PINS E0_STEP_PIN, E0_DIR_PIN, E0_ENABLE_PIN, HEATER_0_PIN,
#if EXTRUDERS > 1
  #define _E1_PINS E1_STEP_PIN, E1_DIR_PIN, E1_ENABLE_PIN, HEATER_1_PIN,
#else
  #define _E1_PINS
#endif
#if EXTRUDERS > 2
  #define _E2_PINS E2_STEP_PIN, E2_DIR_PIN, E2_ENABLE_PIN, HEATER_2_PIN,
#else
  #define _E2_PINS
#endif

#ifdef X_STOP_PIN
  #if X_HOME_DIR < 0
    #define X_MIN_PIN X_STOP_PIN
    #define X_MAX_PIN -1
  #else
    #define X_MIN_PIN -1
    #define X_MAX_PIN X_STOP_PIN
  #endif
#endif

#ifdef Y_STOP_PIN
  #if Y_HOME_DIR < 0
    #define Y_MIN_PIN Y_STOP_PIN
    #define Y_MAX_PIN -1
  #else
    #define Y_MIN_PIN -1
    #define Y_MAX_PIN Y_STOP_PIN
  #endif
#endif

#ifdef Z_STOP_PIN
  #if Z_HOME_DIR < 0
    #define Z_MIN_PIN Z_STOP_PIN
    #define Z_MAX_PIN -1
  #else
    #define Z_MIN_PIN -1
    #define Z_MAX_PIN Z_STOP_PIN
  #endif
#endif

#ifdef DISABLE_MAX_ENDSTOPS
#define X_MAX_PIN          -1
#define Y_MAX_PIN          -1
#define Z_MAX_PIN          -1
#endif

#ifdef DISABLE_MIN_ENDSTOPS
#define X_MIN_PIN          -1
#define Y_MIN_PIN          -1
#define Z_MIN_PIN          -1
#endif

#define SENSITIVE_PINS {0, 1, X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, X_MIN_PIN, X_MAX_PIN, Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, Y_MIN_PIN, Y_MAX_PIN, Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, Z_MIN_PIN, Z_MAX_PIN, PS_ON_PIN, \
                        HEATER_BED_PIN, FAN_PIN,                  \
                        _E0_PINS _E1_PINS _E2_PINS             \
                        analogInputToDigitalPin(TEMP_0_PIN), analogInputToDigitalPin(TEMP_1_PIN), analogInputToDigitalPin(TEMP_2_PIN), analogInputToDigitalPin(TEMP_BED_PIN) }

#endif //__PINS_H
