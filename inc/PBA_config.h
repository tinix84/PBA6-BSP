/**
 * @addtogroup PBA_CONFIG
 * @{
 *******************************************************************************
 * @file		PBA_config.h
 * @brief		Konfiguration des Board-Support-Packages f�r PBA4/5/6
 * @author		ICT Berufsbildungscenter AG
 * @version		1.2.0
 * @date		06.12.2013:	Aufbau PBA6-BSP mit Basis PBA5-BSP,
 *							Unterst�tzung von PIC16F1787, Anpassungen
 *							f�r XC8-Compiler, Interrupt-Library
 * @date		17.02.2016: stdint.h hinzugef�gt-->Wechsel auf std-integer Typen
 * @date		04.03.2016: Renamed RS232 to UART
 * @date		06.04.2016: Menu-Bibliothek hinzugef�gt, Events erweitert
 * @date		08.04.2016:	EEPROM-Bibliothek hinzugef�gt	
 * @date		15.04.2016: PBA Init-Funktion umbenennt zu pba_init()-->	
 *							alle Funktionen beginnen mit Name der Peripherie
 * @date        25.05.2016: Umstrukturierung statemachine library. loop-delay-
 *                          library wird immer ben�tigt, Events in eigener
 *                          Bibliothek ohne loop-delay	
 * @date		07.02.2017	Renaming, Fehlermeldungen optimiert
 * 
 * @attention	PBA_Init zur Initialiserung und Konfigurations des PBAs.				
 *				Um die Libraries zu verwenden, kopieren Sie die beiden Ordner 
 *				"Source" und "Include" in Ihren MPLABX-Projektordner. Erstellen 
 *				Sie ein	Main-Sourcefile, speichern Sie dies im "Source"-Ordner 
 *				und f�gen Sie die Dateien in den beiden Ordnern ihrem 
 *				MPLAB-Projekt hinzu.			
 *				F�gen Sie folgende Codezeile ihrem Source-File hinzu:					
 *				@code #include "../inc/PBA_config.h" @endcode										
 *																		
 * @attention	Wenn Sie bestimmte Bibliotheken nicht ben�tigen, kommentieren 
 *				Sie die zugeh�rigen Defines in der Datei @link PBA_config.h @endlink aus.						
 *				Entfernen Sie die nicht ben�tigten Source-Files aus ihrem Projekt.	
 * 								
 *******************************************************************************
 * 
 * @copyright
 * @{
 * 
 * Diese Software kann unter den Bedingungen der MIT-Lizenz verwendet werden.
 * 
 * Copyright &copy; 2016 ICT Berufsbildungscenter AG
 * 
 * #####MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @}
 */
#ifndef _PBA_CONFIG_
#define _PBA_CONFIG_

/******************** BSP-Konfiguration ***************************************/

	
	/**
	 * @addtogroup BSP-KONFIGURATION BSP-Konfiguration
	 * Nicht ben�tigte Bibliotheken und Konfigurationen auskommentieren
	 * @{
	 */
	#define PBA6/**<Auskommentieren, falls nicht PBA6 verwendet wird*/

	#define	USE_ADC_LIBRARY         /**< ADC-Support Funktionen*/

	#define	USE_LCD_LIBRARY         /**< LCD DOGM162 Funktionen*/

	#define	USE_UART_LIBRARY        /**< UART Funktionen*/

	#define	USE_I2C_LIBRARY         /**< I2C Funktionen*/

	#define	USE_LM75_LIBRARY        /**< Ansteuerung I2C-Temperatursensor*/

	#define USE_EEPROM_LIBRARY      /**< Verwendung internes EEPROM*/

	#define USE_HELPERS_LIBRARY     /**< N�tzliche Makros und Funktionen*/
	
	#define USE_INTERRUPT_LIBRARY	/**< Globaler Interrupt-Handler*/

	#define USE_LOOPDELAY_LIBRARY   /**< Timerbasiertes Delay*/

    #define USE_EVENTS_LIBRARY      /**< Zeit- und Taster-Events*/
	
	#define USE_MENU_LIBRARY        /**< Funktionen Erstellung und Darstellung eines Menus auf LCD*/

	/*Auswahl Standard-Output f�r printf-Funktion*/
	#define STDOUT_LCD				/**< Ausgabe printf auf LCD*/
    //#define STDOUT_UART			/**< Ausgabe printf auf UART-Schnittstelle*/
	
	/**
	 * @}
     */

	/******************** Compiler-Includes, Clock Definiton **********************************/
	#include <xc.h>
	
	#if defined (_16F1787)
	/**
	 * @addtogroup DEF_DELAY Taktkonfiguration f�r Delay
	 * Zur Verwendung der Delays muss die Taktfrequenz bekannt sein
	 * @{
	 */
		#define _XTAL_FREQ 32000000			/**< PIC16F1787--> 8MHz & 4xPLL*/
	/**
	 * @}
	*/
	#else
		#define _XTAL_FREQ 20000000			/* Alle Anderen 20MHz*/
	#endif
	#pragma jis								/* JIS character handling aktivieren (�,�,�), f�r XC-Compiler*/
	
	
	/******************** Schalter/Taster-, LEDs- und Peripherie-Ports **************************/
	/**
	* @addtogroup DEF_TASTER Taster-Bits
	* Erm�glichen Zugriff auf die Taster
	* @{
	*/
	#define		TASTER0		RB0
	#define		TASTER1		RB1
	#define		TASTER2 	RB2
	#define		TASTER3		RB3
	#define		TASTER4		RB4
	#define		TASTER5		RB5
	#define		TASTER6		RB6
	#define		TASTER7		RB7
	/**
	 * @}
	 */
#if defined (_16F1787)
	/**
	* @addtogroup DEF_LED_SUMMER Definition LED-und Summer-Bits
	* Erm�glichen Zugriff auf LEDs und Summer.
	* PIC16F1787 verf�gt �ber Latch-Register
	* Ausg�nge �ber Latch-Register ansprechen
	* um Fehler bei Bitmanipulationen zu vermeiden
	* @{
	*/
	#define		LED0		LATD0
	#define		LED1		LATD1
	#define		LED2 		LATD2
	#define		LED3		LATD3
	#define		LED4		LATD4
	#define		LED5		LATD5
	#define		LED6		LATD6
	#define		LED7		LATD7
	#define		LED8		LATE0
	#define		LED9		LATE1

	#define		LEDS_D		LATD
	#define		LEDS_E		LATE

	#define		SUMMER		LATC5
	/**
	 * @}
	 */
#else
	#define		LED0		RD0
	#define		LED1		RD1
	#define		LED2 		RD2
	#define		LED3		RD3
	#define		LED4		RD4
	#define		LED5		RD5
	#define		LED6		RD6
	#define		LED7		RD7
	#define		LED8		RE0
	#define		LED9		RE1

	#define		LEDS_D		PORTD
	#define		LEDS_E		PORTE

	#define		SUMMER		RC5

#endif
	
/******************** Einbinden der Bibliotheken*******************************/
	#include <stdio.h>				/**<IO-functions (printf, scanf,...)*/
	#include <string.h>				/**<String-functions*/
	#include <stdint.h>				/**<standard integer types*/

	/**
	* @name Libraries einbinden
	* Auskommentieren, falls Library nicht ben�tigt
	* @{
	*/
	#ifdef	USE_HELPERS_LIBRARY
		#include "PBA_helpers.h"
	#endif
	#ifdef	USE_I2C_LIBRARY
		#include "PBA_driver_i2c.h"
	#endif
	#ifdef	USE_LCD_LIBRARY
		#include "PBA_driver_lcd.h"
	#endif
	#ifdef	USE_ADC_LIBRARY
		#include "PBA_driver_adc.h"
	#endif
	#ifdef	USE_LM75_LIBRARY
		#include "PBA_driver_lm75.h"
	#endif
	#ifdef	USE_UART_LIBRARY
		#include "PBA_driver_uart.h"
	#endif
	#ifdef	USE_LOOPDELAY_LIBRARY
		#include "PBA_loopdelay.h"
	#endif
	#ifdef	USE_EVENTS_LIBRARY
		#include "PBA_events.h"
	#endif
	#ifdef USE_INTERRUPT_LIBRARY
	#include "PBA_interrupts.h";
	#endif
	#ifdef USE_MENU_LIBRARY
	#include "PBA_menu.h"
	#endif
	#ifdef USE_EEPROM_LIBRARY
	#include "PBA_driver_eeprom.h"
	#endif
	/**
	 * @}
	 */
	/******************** Prototypen f�r Board-support Library Funktionen ***********************/
	
	void PBA_Init(void);
	
	
	void putch(uint8_t c);
	
	/************** �berpr�fen der Bibliothek-Abh�ngigkeiten ************************************/
	#if defined(USE_LM75_LIBRARY) && !defined(USE_I2C_LIBRARY)
		#error Um die LM75-Library zu nutzen, muss die I2C-Library eingebunden sein (PBA_config.h)
	#endif
	#if defined(STDOUT_LCD) && !defined(USE_LCD_LIBRARY)
		#error Um das LCD als Standard-Output (printf) zu nutzen muss die LCD-Library eingebunden sein (PBA_config.h)
	#endif
	#if defined(STDOUT_UART) && !defined(USE_UART_LIBRARY)
		#error Um UART als Standard-Output (printf) zu nutzen muss die UART-Library eingebunden sein (PBA_config.h)
	#endif
	#if defined(USE_EVENTS_LIBRARY) && !defined(USE_LOOPDELAY_LIBRARY)
		#error Zur Verwedung der Events-Bibliothek muss die loopdelay_Bibliothek eingebunden sein (PBA_config.h)
	#endif
	#if defined(USE_MENU_LIBRARY)&&!defined(USE_LCD_LIBRARY)
	#error Zur Verwendung der Menubibliothek muss die LCD-Bibliothek eingebunden sein (PBA_config.h)
	#endif
	#if defined(USE_MENU_LIBRARY)&&!defined(USE_EVENTS_LIBRARY)
	#error Zur Verwendung der Menubibliothek muss die Events-Library eingebunden sein (PBA_config.h)
	#endif
	#if defined (USE_LOOPDELAY_LIBRARY) && !defined (USE_INTERRUPT_LIBRARY)
	#error Um die loopdelay-Library zu verwenden muss die Interrupt-Library eingebunden sein (PBA_config.h)
	#endif
	#if defined (USE_LCD_LIBRARY) && !defined (USE_HELPERS_LIBRARY)
	#error Zur Verwendung der LCD-Library muss die Helpers-Library eingebunden sein (PBA_config.h
	#endif
	/************** �berpr�fung ob der ausgew�hlte PIC unterst�tzt wird vom Board-support Package **********/
	#if !defined(_16F1787) && !defined(_16F874A) && !defined(_16F877A) && !defined(_16F887) && !defined(_16F884)
		/*Kommentieren sie die Fehlermeldung aus, um das Board-Support Package mit einem nicht unterst�tzten*/
		/*Mikrocontroller zu nutzen.*/
		/*Vorsicht: Einige Bibliotheken funktionieren unter Umst�nden nicht!!*/
		#error Der ausgew�hlte PIC wird nicht unterst�tzt vom PBA Board-support Package!
	#endif
	
	/************** Deaktivieren der stack overflow Warnungen, Problem von Compiler gel�st **************/
	#pragma warning disable 1393,1090

#endif
/**
 *@}
 */
