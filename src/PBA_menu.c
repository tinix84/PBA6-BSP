/**
 * @defgroup PBA_MENU
 * @{
 *******************************************************************************
 * @file			PBA_menu.c
 * @brief			Menu-Library f�r PBA4/5/6
 * @author			ICT Berufsbildungscenter AG
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
#include "../inc/PBA_config.h"
#ifdef USE_MENU_LIBRARY
/*----- LCD-Konfiguration-------------------------------------------------------------------*/

/*Anzahl Zeilen auf dem LCD*/
#define MAXLINES (2u)		/**< Anzahl Zeilen des Displays */

/*Anzahl Zeichen pro Zeile*/
#define MAXCHARS (16u)		/**< Anzahl Zeichen pro Zeile */

/*Defines Tasterrflanken*/
#define POSEDGE_UP		p_events->posEdge.switch2	/**< Tasterflanke Taste UP */
#define POSEDGE_DOWN	p_events->posEdge.switch1	/**< Tasterflanke Taste Down */
#define POSEDGE_ENTER	p_events->posEdge.switch0	/**< Tasterflanke Taste Enter */

/*Zeichen welches angezeigt wird falls Eintr�ge oberhalb des aktuellen Eintrags vorhanden sind*/
/*welche nicht angezeigt werden*/
uint8_t charsetAddrArrowUp		=	0x00;

/*Zeichen welches angezeigt wird falls Eintr�ge unterhalb des aktuellen Eintrags vorhanden sind*/
/*welche nicht angezeigt werden*/
uint8_t charsetAddrArrowDown	=	0x01;



/*----- LCD-Funktionen ---------------------------------------------------------------------*/

/**
 * @brief Ruft die Funktion zum L�schen des LCDs auf
 */
static inline void MENU_LcdClear()		
{
	LCD_Clear();
}

/*Ruft Funktion zum anzeigen eines Strings auf einer bestimmten Zeile auf*/

/**
 * @brief Ruft Funktion zum anzeigen eines Strings auf einer bestimmten Zeile auf*
 * @param row		Displayzeile auf welche geschrieben wird
 * @param p_text	Auszugebender Text
 */
static inline void MENU_LcdDisplayStringLine(uint8_t row,const uint8_t *p_text)	
{
	LCD_GotoXY(1,row);		/*erste Zeile=0*/
	LCD_PutsRom(p_text);
}

/**
 * @brief Ruft Funktion zum Anzeigen eines Zeichens an bestimmter Stelle auf bestimmter Zeile auf
 * @param row	Displayzeile auf welche geschrieben wird
 * @param posX	Position in Zeile
 * @param ch	Zeichen
 */
static inline void MENU_LcdDisplayCharLine(uint8_t row, uint8_t posX,uint8_t ch)
{
	LCD_GotoXY(posX,row);		/*Erste Zeile=0, Erstes Zeichen=0*/
	LCD_Putc(ch);
}






/*----- PBA spezifische LCD-Funktionen ----------------------------------------------------*/
/**
 * @brief F�gt ein selbst definiertes Zeichen dem Charset eines DOGM-162 Displays hinzu
 * @param cgramAddr	CGRAM Adresse an welchem das Zeichen gespreichert werden soll (0..7)
 * @param p_char	Zeichen-Array (8-Byte)
 */
static void LCD_SetChar(unsigned char cgramAddr, unsigned const char *p_char)
{
	unsigned char i;
	LCD_Send(0x40|(cgramAddr<<3));				/*CGRAM-Addresse setzen (Befehl)*/
	LCD_RS=1;									/*LCD umschalten auf Datenempfang*/
	for(i=0;i<=7;i++)							/*8 Bytes CG-Daten senden*/
		LCD_Send(*(p_char+i));					/*Sende 1 Byte Daten aus �bergabe-Array*/
	LCD_RS=0;									/*LCD umschalten auf Befehlsempfang*/
}
/*Speichert Pfeilsymbole im ROM des PBA-LCDs*/
/**
 * @brief Speichert Pfeil und Ok Zeichen im CGRAM des DOGM162-Displays
 */
static void LCD_CreateArrows(void)
{
	const uint8_t arrow_up[]=  {0b0000100,
								0b0001110,
								0b0010101,
								0b0000100,
								0b0000100,
								0b0000100,
								0b0000100,
								0b0000000};
	
	const uint8_t arrow_down[]={0b0000100,
								0b0000100,
								0b0000100,
								0b0000100,
								0b0010101,
								0b0001110,
								0b0000100,
								0b0000000};
	
	const uint8_t ok[]=		   {0b0000001,
								0b0000001,
								0b0000010,
								0b0000010,
								0b0000010,
								0b0010100,
								0b0001100,
								0b0000000};
	LCD_SetChar(0,arrow_up);					/*arrow_up in CGRAM (resp. CGROM) laden*/
	LCD_SetChar(1,arrow_down);					/*arrow_up in CGRAM (resp. CGROM) laden*/
	LCD_SetChar(2,ok);							/*ok in CGRAM (resp. CGROM) laden*/
}
/*----- Pointer auf ersten Menueintrag -----------------------------------------------------*/
static menuEntry_t *p_menu;	/**< Pointer auf ersten Menueintrag*/

/*----- Pointer auf Events-Struktur --------------------------------------------------------*/

static events_t *p_events; /**< Pointer auf Events-Struktur*/

/*----- Datentyp welcher die Scrollrichtung speichert --------------------------------------*/

/**
 *@brief Aktuelle Scrollrichtung.
 */
typedef enum
{
	DIR_UP,		/**< Aufw�rts */
	DIR_DOWN	/**< Abw�rts */
}menuScrollDirection_t;

/*----- Initialisierung des Men�s ----------------------------------------------------------*/

/**
 * @brief Initialisierung des Menus
 * @param p_menuStart	Erster Menueintrag der auf dem Display dargestellt werden soll
 * @param p_event		Im Hauptprogramm verwendete Eventstruktur
 */
void MENU_Init(menuEntry_t *p_menuStart ,events_t *p_event)
{
	p_menu=p_menuStart;
    p_events=p_event;
	LCD_CreateArrows();
}


/*----- Zeichnen des Men�s auf das Display -------------------------------------------------*/

/**
 * @brief Zeichet das Menu auf das Display.
 * @param p_firstRowMenuEntry	Menueintrag der zuoberst auf dem Display angezeigt wird
 * @param p_cursorRowMenuEntry	Menueintrag der aktuell angew�hlt ist
 * @param direction				Scrollrichtung
 */
static void MENU_draw(menuEntry_t *p_firstRowMenuEntry, menuEntry_t *p_cursorRowMenuEntry,menuScrollDirection_t direction)
{	uint8_t drawRowNmbr=0;		/*1. Zeile auf die geschrieben wird*/
	menuEntry_t *p_currentRowMenuEntry;
	MENU_LcdClear();
	/* nach unten scrollen in menu, aktuelle gew�hlte ID gr�sser als maximal anzeigbare Zeilen in Display*/
	if(((direction==DIR_DOWN)&&((p_cursorRowMenuEntry->id)>(MAXLINES-1)))||((p_cursorRowMenuEntry->next==p_firstRowMenuEntry)&&(direction==DIR_UP)))
	{
		p_currentRowMenuEntry=p_cursorRowMenuEntry; 
		drawRowNmbr=(MAXLINES);
		do
		{
			MENU_LcdDisplayStringLine(drawRowNmbr-1,p_currentRowMenuEntry->name);	/*Name des Menueintrags aufs Display zeichen*/
			if(p_currentRowMenuEntry==p_cursorRowMenuEntry)									/*Wenn aktuelle Zeile=angew�hlte Zeile*/
			MENU_LcdDisplayCharLine(drawRowNmbr-1,0,'*' );					/*Markierung aufs Display schreiben*/
			p_currentRowMenuEntry=p_currentRowMenuEntry->prev;								/*Danach vorherige Zeile anw�hlen*/
			drawRowNmbr-=1;													/*Zeilennummer verkleinern*/
		}
		while((p_currentRowMenuEntry!=p_firstRowMenuEntry)&&(drawRowNmbr>0));					/*Dies wird wiederholt solange sich die Eintr�ge nicht wiederholen*/
		
		if(p_cursorRowMenuEntry->next!=p_firstRowMenuEntry)									/*Weitere Menueintr�ge unterhalb*/
		{
			MENU_LcdDisplayCharLine((MAXLINES-1),(MAXCHARS-1),charsetAddrArrowDown );	/*Pfeil nach unten anzeigen*/
		}
		if(p_cursorRowMenuEntry->id-(MAXLINES-1)>0)								/*Weitere Eintr�ge oberhalb*/
		{
			MENU_LcdDisplayCharLine((0),15,charsetAddrArrowUp );						/*Pfeil nach oben anzeigen*/
		}
	}

	else
	{
		/*nach oben scrollen, aktuell gew�hlte ID gr�sser als maximal anzeigbare Zeilen in Display*/
		if((direction==DIR_UP)&&((p_cursorRowMenuEntry->id)>(MAXLINES-1)))
		{
			p_currentRowMenuEntry=p_cursorRowMenuEntry;  
			do
			{
				MENU_LcdDisplayStringLine(drawRowNmbr,p_currentRowMenuEntry->name);	/*Name des Menueintrags aufs Display zeichen*/
				if(p_currentRowMenuEntry==p_cursorRowMenuEntry)								/*Wenn aktuelle Zeile=angew�hlte Zeile*/
				MENU_LcdDisplayCharLine(drawRowNmbr,0,'*' );					/*Markierung aufs Display schreiben*/
				p_currentRowMenuEntry=p_currentRowMenuEntry->next;							/*Danach n�chste Zeile anw�hlen*/
				drawRowNmbr+=1;											/*Zeilennummer erh�hen*/
			}
			while((p_currentRowMenuEntry!=p_firstRowMenuEntry)&&(drawRowNmbr<MAXLINES));		/*Dies wird wiederholt solange sich die Eintr�ge nicht wiederholen*/
			
			if(p_currentRowMenuEntry!=p_firstRowMenuEntry)									/*Weitere Eintr�ge unerhalb*/
			{
				MENU_LcdDisplayCharLine((MAXLINES-1),15,charsetAddrArrowDown );			/*Pfeil nach unten anzeigen*/
			}
			if(p_cursorRowMenuEntry->id-(MAXLINES-1)>0)								/*Weitere Eintr�ge oberhalb*/
			{
				MENU_LcdDisplayCharLine((0),15,charsetAddrArrowUp );						/*Pfeil nach oben anzeigen*/
			}
		}
		/*Aktuelle ID des Menueintrags kleiner als maximal anzeigbare Zeilen auf Display*/
		else
		{
			p_currentRowMenuEntry=p_firstRowMenuEntry;  
			do
			{
				MENU_LcdDisplayStringLine(drawRowNmbr,p_currentRowMenuEntry->name);	/*Name des Menueintrags aufs Display zeichen*/
				if(p_currentRowMenuEntry==p_cursorRowMenuEntry)								/*Wenn aktuelle Zeile=angew�hlte Zeile*/
				MENU_LcdDisplayCharLine(drawRowNmbr,0,'*' );					/*Markierung aufs Display schreiben*/
				p_currentRowMenuEntry=p_currentRowMenuEntry->next;							/*Danach n�chste Zeile anw�hlen*/
				drawRowNmbr+=1;											/*Zeilennummer erh�hen*/
			}
			while((p_currentRowMenuEntry!=p_firstRowMenuEntry)&&(drawRowNmbr<(MAXLINES)));	/*Dies wird wiederholt solange sich die Eintr�ge nicht wiederholen oder Display voll*/
			if(p_currentRowMenuEntry!=p_firstRowMenuEntry)									/*Weitere Eintr�ge unterhalb*/
			{
				MENU_LcdDisplayCharLine((MAXLINES-1),(MAXCHARS-1),charsetAddrArrowDown );	/*Pfeil nach unten anzeigen*/
			}
		}
	}
}
/*----- Aktualisieren des Cursorsymbols auf dem Display -------------------------*/

/**
 * @brief Manuelles Setzen der Cursors auf eine bestimmte Zeile
 * @param p_firstRowMenuEntry		Erster Menueintrag
 * @param p_cursorRowMenuEntryNew	Menueintrag bei dem Cursor gesetzt werden soll
 * @param p_cursorRowMenuEntryOld	Menueintrag bei dem der Cursor aktuell ist
 */
static void MENU_SetCursor(menuEntry_t *p_firstRowMenuEntry, menuEntry_t *p_cursorRowMenuEntryNew, menuEntry_t *p_cursorRowMenuEntryOld)
{  uint8_t drawRowNmbr=0;
   menuEntry_t *p_currentRowMenuEntry;

   p_currentRowMenuEntry=p_firstRowMenuEntry;
   do
   {
		if(p_currentRowMenuEntry==p_cursorRowMenuEntryNew)	/*Entspricht aktuelle Zeile, der neuen angew�hlten Zeile*/
		{
			MENU_LcdDisplayCharLine(drawRowNmbr,0,'*' );	/*Markierung schreiben*/
		}
		if(p_currentRowMenuEntry==p_cursorRowMenuEntryOld)	/*Entspricht aktuelle Zeile der alten angew�hlten Zeile*/
		{
			MENU_LcdDisplayCharLine(drawRowNmbr,0,' ' );	/*Markierung l�schen*/
		}
       p_currentRowMenuEntry=p_currentRowMenuEntry->next;	/*N�chste Zeile anw�hlen*/
       drawRowNmbr+=1;									/*Zeilennummer erh�hen*/
	   
   }
   while(p_currentRowMenuEntry!=p_firstRowMenuEntry);
}

/* ----- Hauptfunktion des Men�s -------------------------------------------------*/

/**
 * @brief Anzeige und Aktualisierung des Menus.
 * Muss zyklisch aufgerufen werden
 * @return 1:Menu beendet, 0:Menu nicht beendet
 */
uint8_t MENU_Call (void)
{
	static menuEntry_t *p_cursorRowMenuEntry=0,*p_firstRowMenuEntry=0;

	static uint8_t isFirstCall=1;
	static uint8_t doRepeatFunction=0;

	if(isFirstCall)		/*Wird die Funktion das erste Mal aufgerufen->Startwerte setzen*/
	{
		p_cursorRowMenuEntry=p_firstRowMenuEntry=p_menu;	/*Startpunkt Hauptmen�*/
		MENU_LcdClear();
		MENU_draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_UP);
		isFirstCall=0;
	}
	if(doRepeatFunction==0)			/*Wenn im vorherigen Aufruf keine Funktion ausgw�hlt wurde die wiederholt werden muss*/
	{								/*�berpr�fung der Tasten und Men�handling*/

		if( POSEDGE_UP )			/* Taste "Rauf" gedr�ckt ?*/
		{
			p_cursorRowMenuEntry=p_cursorRowMenuEntry->prev;						/*Position speichern*/
			MENU_draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_UP);
			 
			 return 0;

		}
		if( POSEDGE_DOWN )			/* Taste "Runter" gedr�ckt?*/
		{
			p_cursorRowMenuEntry=p_cursorRowMenuEntry->next;						/*Position speichern*/
			MENU_draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_DOWN);
			
			return 0;
		}
		else
		{
			if( POSEDGE_ENTER )									/*Taste "Enter" gedr�ckt?*/
			{
				if(p_cursorRowMenuEntry->submenu)						/*Wenn ein Untermen� vorhanden*/
				{
					p_firstRowMenuEntry=p_cursorRowMenuEntry=p_cursorRowMenuEntry->submenu;	/*Cursor neu setzen*/
					MENU_LcdClear();
					MENU_draw(p_firstRowMenuEntry, p_cursorRowMenuEntry,DIR_UP);		/*Untermenu zeichnen*/
					return 0;
				}
				else
				{
					if(p_cursorRowMenuEntry->function)				/*Wenn eine Funktion vorhanden*/
					{
						MENU_LcdClear();
						p_cursorRowMenuEntry->function(1);			/*Funktion ausf�hren, �bergabeparameter=1->1. Aufruf*/
						doRepeatFunction=1;					/*Merkvariable setzen, beim n�chsten Aufruf von menu() wird wieder die Funktion ausgef�hrt*/
						return 0;
					}
					else
					{
						if(p_cursorRowMenuEntry->ret)				/*Wenn ein R�ckkehrmenu vorhanden*/
						{
							MENU_LcdClear();
							p_cursorRowMenuEntry=p_firstRowMenuEntry=p_cursorRowMenuEntry->ret;	/*Cursor auf R�ckkehrmenu setzen*/
							MENU_draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_UP);	/*Menu zeichnen*/
							return 0;
						}
						else							/*Falls weder Untermenu, noch Funktion, noch R�ckkehrmenu vorhanden*/
						{								/*Kann nur bei Exit im Hauptmenu vorkommen*/
							MENU_LcdClear();				/*Display l�schen*/
							isFirstCall=1;				/*Startvariable wieder setzen*/
							return 1;					/*Menu beendet*/
						}
					}
				}
			}
			return 0;
		}
	}
	else/*Falls repeatfunction gesetzt (Funktion soll noch einmal ausgef�hrt werden)*/
	{
		if(p_cursorRowMenuEntry->function(0))	/*Falls die ausgef�hrt Funktion 1 als R�ckgabe liefert*/
		{										/*Funktion muss nicht wieder ausgef�hrt werden*/
			MENU_LcdClear();
			p_firstRowMenuEntry=p_cursorRowMenuEntry=p_cursorRowMenuEntry->ret;	/*Cursor auf R�ckkehrmenu setzen*/
			MENU_draw(p_firstRowMenuEntry, p_cursorRowMenuEntry,DIR_UP);	/*Menu zeigen*/
			doRepeatFunction=0;						/*Speichervariabel l�schen*/
		}
	return 0;
	}
}
#endif

/**
 * @}
 */