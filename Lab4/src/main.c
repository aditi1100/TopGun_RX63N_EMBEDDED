#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "platform.h" 
#include "r_switches.h"
#include "cmt_periodic_multi.h"
#include "timer_adc.h"
#include "flash_led.h"

#define TIMER_COUNT_600MS 56250  /* 600mS with 48MHz pclk/512 */
#define MCU_NAME "   RX63N    "
#define TIMER_COUNT_100MS   9375 /* 0.1 second with 48MHz pclk / 512. */ 
#define CHANNEL_0   0
#define TIMER_COUNT_400MS   37500 /* 400MS with 48MHz pclk / 512. */ 
#define TIMER_COUNT_200MS   18750 /* 400MS with 48MHz pclk / 512. */ 

/* Statics test replacement variable */
uint8_t g_working_string[13] = "   STATIC   "; /* 12 characters plus NULL terminator. */

/* Statics test const variable */
const uint8_t g_replacement_str[] = "TEST TEST"; /* Must be 12 chars or less. */

volatile bool g_sw1_press = false;
volatile bool g_sw2_press = false;
volatile bool g_sw3_press = false;


void BLINK_RED_LEDs(void);
void RESET_ALL_LEDs(void);
void ALL_RED_LEDs_ON(void);
void BLOCK_UNTIL_SWITCH_PRESS(void);
void NEW_PATTERN(void);
void Create_MyObstacle(void);
int LED_STATE = 0; //0 is off and 1 is on
int PAUSE = 1; //0 is off and 1 is on
int valx_1;
int valy_1;
int x_jet = 0;
int y_jet = 0;
bool enemy_flag=0; //variable for creating enemy plane
bool hit_flag = 0; //variable for tracking hits and hit animation

/*******************************************************************************
Private function prototypes and global variables
*******************************************************************************/
static void statics_test(void);
static void timer_callback(void);

volatile static bool g_delay_complete = false;


/******************************************************************************
* Function name: main
* Description  : Main program function. The function first initialises the debug
*                LCD, and displays the splash screen on the LCD. The function
*                then calls the flash_led, timer_adc and statics_test functions. 
*                These start by flashing the LEDs at a fixed rate until a switch is
*                pressed, then the flash rate varies with the position of the   
*                pot, RV1. Finally, statics_test displays " STATICS " on the    
*                debug LCD, which is replaced with "TEST TEST" a letter at a 
*                time.
* Arguments    : none
* Return value : none
******************************************************************************/
void main(void)
{
	R_SWITCHES_Init(); /* Prepare the board switches for use. */
	
    /* Initialize the debug LCD on the RSPI bus. */
    lcd_initialize();
    
    /* Clear LCD */
    lcd_clear();
    
    /* Display message on LCD */
    lcd_display(LCD_LINE1, "Hello World");
    lcd_display(LCD_LINE2, MCU_NAME);

    cmt_init(); /* Initialize the CMT unit for application timing tasks. */ 
	
	cmt_callback_set(CHANNEL_0, &BLINK_RED_LEDs);
	cmt_start(CHANNEL_0, TIMER_COUNT_400MS);
	
	Set_Font_Bitmap();
	Set_LCD_Pos(0,0);
	Set_LCD_Char(0);
	
	BLOCK_UNTIL_SWITCH_PRESS();

	lcd_clear();
	Set_Font_8_by_8();
	lcd_display(LCD_LINE1, "Deekshith");
	lcd_display(LCD_LINE2, "Shyaam");
	cmt_stop(CHANNEL_0);
    
	cmt_callback_set(CHANNEL_0, &NEW_PATTERN);
	cmt_start(CHANNEL_0, TIMER_COUNT_200MS);
	
	BLOCK_UNTIL_SWITCH_PRESS();
	lcd_clear();

	cmt_stop(CHANNEL_0);
	
	
   timer_adc();
	
    while (1)
    {
		
		if(g_sw3_press)
		{
			g_sw3_press = false;
			
			if(PAUSE ==0) PAUSE =1;
			else PAUSE =0;
		
		}
			
	} 
}



/******************************************************************************
* Function name: timer_callback
* Description  : This function is called from the cmt_timer compare-match ISR.
*              : It sets a global flag that is polled for delay loop control.
* Arguments    : None
* Return value : None
******************************************************************************/
void timer_callback(void)
{
    cmt_stop(CHANNEL_0);
    g_delay_complete = true;
    
} /* End of function timer_callback() */



/******************************************************************************
* Function name: sw1_callback
* Description  : Callback function that is executed when SW1 is pressed.
*                Called by sw1_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw1_callback(void)
{
    g_sw1_press = true; /* The switch was pressed. */
} /* End of function sw1_callback() */


/******************************************************************************
* Function name: sw2_callback
* Description  : Callback function that is executed when SW2 is pressed.
*                Called by sw2_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw2_callback(void)
{
    g_sw2_press = true; /* The switch was pressed. */
} /* End of function sw2_callback() */


/******************************************************************************
* Function name: sw3_callback
* Description  : Callback function that is executed when SW3 is pressed.
*                Called by sw3_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw3_callback(void)
{
    g_sw3_press = true; /* The switch was pressed. */
} /* End of function sw3_callback() */



void RESET_ALL_LEDs(void)
{
	PORTD.PODR.BYTE = 0xFF;       /* Turn off all LEDs on port D.*/
    PORTE.PODR.BYTE |= 0x0F;      /* Turn off all LEDs on port E.*/
}

void ALL_RED_LEDs_ON(void)
{
	RESET_ALL_LEDs();
	
	PORTD.PODR.BIT.B2 = 0;
	PORTE.PODR.BIT.B0 = 0;
	PORTD.PODR.BIT.B1 = 0;
	PORTD.PODR.BIT.B7 = 0;
	PORTD.PODR.BIT.B0 = 0;
	PORTD.PODR.BIT.B6 = 0;
}

void BLOCK_UNTIL_SWITCH_PRESS(void)
{
	while(1)
	{
		if((g_sw1_press) || (g_sw2_press) || (g_sw3_press))
		{
			RESET_ALL_LEDs();
			LED_STATE = 0;
			g_sw1_press = false;
			g_sw2_press = false;
			g_sw3_press = false;
			
			break;
		}
	}
}

void BLINK_RED_LEDs()
{
		if(LED_STATE == 0)
		{
			RESET_ALL_LEDs();
			LED_STATE = 1;			
		} else
		{
			ALL_RED_LEDs_ON();
			LED_STATE = 0;	
		}
}

void NEW_PATTERN(void)
{
	PORTD.PODR.BYTE ^= 0x26;
	PORTE.PODR.BYTE ^= 0x07;
}

void Create_MyObstacle(void)
{
	
	if(PAUSE ==0)
	{
		Set_Font_8_by_8();
		lcd_clear();
		lcd_display(LCD_LINE1, "PAUSED");
		 
		RESET_ALL_LEDs();	
	}
	
	else if(PAUSE ==1)
	{
		lcd_clear();	
		ALL_RED_LEDs_ON();
		 Set_Font_Bitmap();
		 Set_LCD_Pos(x_jet,y_jet);
			Set_LCD_Char(3);
		if(g_sw1_press & PAUSE ==1)
		{
			y_jet = y_jet - 8;
			if(y_jet<0)	y_jet=0;
			g_sw1_press = false;
		}
	
		if(g_sw2_press & PAUSE ==1)
		{
			y_jet = y_jet+ 8;
			if(y_jet>0x32)	y_jet=0x32;	
			g_sw2_press = false;
		}
	
	
	if(!enemy_flag)
	{ //create enemy plane
		valx_1 = 80; //spawn on the other side of the screen
		valy_1 = 8*(rand() % 8);
		enemy_flag = 1;
		Set_LCD_Pos(valx_1, valy_1);
		Set_LCD_Char(7);
	}
	else
	{
		if(valx_1 <= 23) //check for collision 
		{
			if((valy_1 <= (y_jet + 16)) && (valy_1 >= y_jet))
			{
				enemy_flag = 0;//respawn enemy
				hit_flag = 1;
			}
		}
		if(valx_1 == 0)
			enemy_flag = 0;
		else
		{
			valx_1 = valx_1 - 5;
			if(hit_flag == 0)
			{
				Set_LCD_Pos(valx_1, valy_1);
				Set_LCD_Char(7); //draw jet
			}
			else
			{
				Set_LCD_Pos(valx_1,valy_1);
				Set_LCD_Char(7);
				Set_LCD_Pos(x_jet, y_jet);
				Set_LCD_Char(1);
				hit_flag = 0;
			}
		}
	}
	}
}
