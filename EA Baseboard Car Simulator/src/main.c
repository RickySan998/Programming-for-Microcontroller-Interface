/*****************************************************************************
 *   A demo example using several of the peripherals on the base board
 *
 *   Copyright(C) 2011, EE2024
 *   All rights reserved.
 *
 ******************************************************************************/

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_timer.h"
#include "lpc17xx.h"

#include "joystick.h"
#include "pca9532.h"
#include "acc.h"
#include "oled.h"
#include "rgb.h"
#include "led7seg.h"
#include "temp.h"
#include "light.h"
#include "pca9532.h"
#include "string.h"
#include "math.h"

#define I2CDEV LPC_I2C2
#define ACC_I2C_ADDR    (0x1D)
#define ACC_ADDR_CTL1   0x18
#define ACC_ADDR_CTL2   0x19
#define ACC_ADDR_LDTH   0x1A
#define ACC_ADDR_PDTH   0x1B
#define ACC_ADDR_PW     0x1C
#define ACC_ADDR_LT     0x1D
#define ACC_ADDR_TW     0x1E
#define ACC_ADDR_DETSRC 0x0A
#define ACC_ADDR_INTRST 0x17
volatile int readacc=0;
volatile int jrec=0;
volatile int jsend=0;
volatile int scol=0;
volatile int stemp=0;
volatile int emsentcol=0;
volatile int emsenttemp=0;
volatile int sendingtemp=0;
volatile int sendingcol=0;
volatile int supermove=2;
volatile uint32_t msTicks;
volatile int colstat = 0;
volatile int tempstat = 0;
int carstat = 0;
volatile uint32_t buttonstart = 0;
volatile int buttonpressed = 0;
uint32_t buttonstartTime;
int buttoncounter;
volatile int oledy=8;
volatile int prevoledy=8;
volatile int menuselect=0;
volatile int xpos = 0;
volatile int finishmeasure=-1;
volatile int numperiods=0;
volatile int t1=0;
volatile int t2=0;
volatile int superobsflag=0;

static void init_ssp(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SPI pin connect
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P2.2 - SSEL - used as GPIO
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

}

static void init_i2c(void)
{
	PINSEL_CFG_Type PinCfg;

	/* Initialize I2C2 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000);
	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);
}

static void init_GPIO(void)
{
	// Initialize button

  PINSEL_CFG_Type PinCfg;

  PinCfg.Portnum = 2;
  PinCfg.Pinnum = 0;
  PinCfg.Funcnum = 0;
  PINSEL_ConfigPin(&PinCfg);
  PinCfg.Portnum = 0;
  PinCfg.Pinnum = 26;
  PinCfg.Funcnum = 0;
  PINSEL_ConfigPin(&PinCfg);
  PinCfg.Portnum = 2;
  PinCfg.Pinnum = 1;
  PinCfg.Funcnum = 0;
  PINSEL_ConfigPin(&PinCfg);
  PinCfg.Funcnum = 1;
  PinCfg.Pinnum = 10;			// sw3
  PINSEL_ConfigPin(&PinCfg);
  PinCfg.Portnum = 0;
  PinCfg.Pinnum = 2;
  PinCfg.Funcnum = 0;			//temp
  PINSEL_ConfigPin(&PinCfg);


}
static int I2CRead(uint8_t addr, uint8_t* buf, uint32_t len)
{
	I2C_M_SETUP_Type rxsetup;

	rxsetup.sl_addr7bit = addr;
	rxsetup.tx_data = NULL;	// Get address to read at writing address
	rxsetup.tx_length = 0;
	rxsetup.rx_data = buf;
	rxsetup.rx_length = len;
	rxsetup.retransmissions_max = 3;

	if (I2C_MasterTransferData(I2CDEV, &rxsetup, I2C_TRANSFER_POLLING) == SUCCESS){
		return (0);
	} else {
		return (-1);
	}
}
static int I2CWrite(uint8_t addr, uint8_t* buf, uint32_t len)
{
	I2C_M_SETUP_Type txsetup;

	txsetup.sl_addr7bit = addr;
	txsetup.tx_data = buf;
	txsetup.tx_length = len;
	txsetup.rx_data = NULL;
	txsetup.rx_length = 0;
	txsetup.retransmissions_max = 3;

	if (I2C_MasterTransferData(I2CDEV, &txsetup, I2C_TRANSFER_POLLING) == SUCCESS){
		return (0);
	} else {
		return (-1);
	}
}
uint32_t getTicks(void) {
	return msTicks;
}
void setcolstat(void) {
	colstat = 1;
	return;
}
void resetcolstat (void) {
	colstat = 0;
}
int getcolstat (void) {
	return colstat;
}
void settempstat (void) {
	tempstat = 1;
	return;
}
void resettempstat (void) {
	tempstat = 0;
	return;
}
int gettempstat (void) {
	return tempstat;
}
uint32_t getButtonStartTime(void) {
	return buttonstart;
}
int getButtonCount (void) {
	return buttonpressed;
}
void resetbuttonpressed (void){
	buttonpressed = 0;
	return;
}
void oled_clearLine(int yposition) {
	oled_putString(0,yposition,"               ",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	return ;
}


void SysTick_Handler(void){
	static int colstartTime=-1;
	static int tempstartTime = -1;
	static int bothstartTime = -1;
	uint32_t tempv;
	int rgbstate;
	uint32_t currentTime;
	msTicks++;
	if((colstat==1)&&(tempstat == 0)) {
		if(colstartTime == -1) {
			tempv = getTicks();
			colstartTime = (int)tempv;
		}
		currentTime = getTicks();
		rgbstate = ((currentTime - colstartTime)/333) % 2;
		if(rgbstate == 0) {
			rgb_setLeds(RGB_BLUE | RGB_GREEN);
		}else {
			rgb_setLeds(RGB_GREEN);
		}
	}
	if((colstat == 0)&&(tempstat == 1)) {
		if(tempstartTime == -1) {
			tempv = getTicks();
			tempstartTime = (int)tempv;
		}
		currentTime = getTicks();
		rgbstate = ((currentTime - tempstartTime)/333) % 2;
		if(rgbstate == 0) {
			rgb_setLeds (RGB_RED | RGB_GREEN);
		}else {
			rgb_setLeds(RGB_GREEN);
		}
	}
	if((colstat == 1) && (tempstat == 1)) {
		if(bothstartTime == -1) {
			tempv = getTicks();
			bothstartTime = (int)tempv;
		}
		currentTime = getTicks();
		rgbstate = ((currentTime - bothstartTime)/333)% 2 ;
		if(rgbstate == 0) {
			rgb_setLeds (RGB_RED | RGB_BLUE | RGB_GREEN);
		}else {
			rgb_setLeds(RGB_GREEN);
		}
	}
	if((colstat == 0) && (tempstat == 0)) {
		tempstartTime = -1;
		colstartTime = -1;
		bothstartTime = -1;
		rgb_setLeds(RGB_GREEN);
	}
	return ;
}
void EINT3_IRQHandler(void) {
	if(LPC_GPIOINT->IO2IntStatF >> 5 & 0x01) {
		LPC_GPIOINT->IO2IntClr |= 1<<5;
		superobsflag=1;
		tempstat = 1;
	}
	if(LPC_GPIOINT->IO0IntStatR >> 3 & 0x01) {
		LPC_GPIOINT->IO0IntClr |= 1<<3;
		colstat=1;
		if(emsentcol==0){
			/*UART_SendData(LPC_UART3,'C');
			UART_IntConfig(LPC_UART3,UART_INTCFG_THRE,ENABLE);*/
			emsentcol=1;
		}
	}
	if(LPC_GPIOINT->IO0IntStatR >> 2 & 0x01) {
			LPC_GPIOINT->IO0IntClr |= 1<<2;
			if(numperiods < 170) {
				if(numperiods==0) {
					t1 = getTicks();
					numperiods ++;
				}else{
					numperiods ++;
				}
			}else{
				t2 = getTicks();
				finishmeasure=1;
				LPC_GPIOINT->IO0IntEnR &= 0x1111111B;
				numperiods=0;
			}
		}
	if((LPC_GPIOINT->IO0IntStatF >>15)&0x01 ) {
		LPC_GPIOINT->IO0IntClr |= 1<<15;
		if(supermove==0){
			if(oledy<=8) {
				oledy=oledy+8;
			}
		}else{
		if(oledy<=16){
		oledy= oledy+8;
		}
		}
	}
	if((LPC_GPIOINT->IO0IntStatF >>16)&0x01 ) {
		LPC_GPIOINT->IO0IntClr |= 1<<16;
		menuselect = 1;
	}
	if((LPC_GPIOINT->IO2IntStatF >>3)&0x01 ) {
		LPC_GPIOINT->IO2IntClr |= 1<<3;
		if(oledy>=16) {
		oledy = oledy - 8;
		}
	}

}
void EINT0_IRQHandler(void)
{
        if(buttonpressed == 0){
        	buttonpressed++;
        	buttonstart = getTicks();
        }
        else {
        if(buttonpressed == 1){
        	buttonpressed++;

        }else {
        	if(buttonpressed == 2) {
        		buttonpressed++;
        	}
        }
        }

	LPC_SC ->EXTINT |= 1<<0;
	return;
}
void TIMER0_IRQHandler (void) {
	if(LPC_TIM0->IR & 0x01) {
	TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
	xpos=xpos+6;
	if(xpos >= 90) {
		xpos = 0;
	}
	}
}
void TIMER1_IRQHandler(void) {
	if(LPC_TIM1->IR & 0x01) {
		TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);
		readacc=1;
	}
}
void startmeasuretemp(void) {
	if(finishmeasure==-1){
	finishmeasure = 0;
	LPC_GPIOINT->IO0IntClr |= 1<<2;
	LPC_GPIOINT->IO0IntEnR |= 1<<2;
	}
	return;
}
void StationaryMode(void){
	uint32_t currentTime;
	char words[100] = "Stationary";
	char progress[200]="****************";
	resetcolstat();
	resettempstat();
	led7seg_setChar('z',0,1);
	pca9532_setLeds(0x0000, 0xffff);
	oled_clearScreen(OLED_COLOR_WHITE);
	oled_putString(0,0,words,OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	TIM_Cmd(LPC_TIM0,DISABLE);
	TIM_Cmd(LPC_TIM1,DISABLE);
    LPC_GPIOINT->IO0IntEnF &= ~(1 <<16);
    LPC_GPIOINT->IO0IntEnF &= ~(1 <<15);
    LPC_GPIOINT->IO2IntEnF &= ~(1 <<3);
    LPC_GPIOINT->IO2IntEnF &= ~(1<<5);
	while(carstat == 0){
		buttoncounter = getButtonCount();
		if(buttoncounter != 0){
			buttonstartTime = getButtonStartTime();
			currentTime = getTicks();
			if((currentTime - buttonstartTime)/1000 >= 1){
				carstat = buttoncounter;
				buttoncounter = 0;
				resetbuttonpressed();
			}
		}
	}
	return;
}
void ForwardMode(void){
	char letters[17]={'0','1','2','3','4','5','6','7','8','9','A','8','C','0','E','F'};
	char words[4][100]= {"Forward","Air bag","Released","Temp. too high"};
	char arrow[20]="->";
	char reading[2][100];
	int j=0;
	int prevj;
	int scaleG;
	int joy;
	int neg=0;

	uint32_t currTime = 0;
	uint32_t nextTime = 0;
	int8_t x = 0;
	int8_t y = 0;
	int8_t z = 0;
	int32_t xoff = 0;
	int32_t yoff = 0;
	int32_t zoff = 0;
	int32_t temp = 0;
	int colstart;
	int tempstart;


	currTime = getTicks();
	oled_clearScreen(OLED_COLOR_WHITE);
	oled_putString(0,0,words[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	pca9532_setLeds(0x0000, 0xffff);
	acc_read(&x, &y, &z);
	    xoff = 0-x;
	    yoff = 0-y;
	    zoff = 64-z;
	while(carstat ==1){
		colstart = getcolstat();
		tempstart = gettempstat();
		nextTime = getTicks();
		prevj = j;
		j= ((nextTime - currTime)/1000)%16;
		led7seg_setChar(letters[j],0,1);
		if (j!=prevj) {
			prevj = j;
			temp = temp_read();
			acc_read(&x, &y, &z);
			x = x+xoff;
			y = y+yoff;
			z = z+zoff;
			if(x<0){
				x = -x;
				neg = 1;
			}else{
				neg = 0;
			}
			scaleG = x * 1000 / 64;
			if (j==5 || j == 10 || j==15){
			 if(neg==0) {
				if(scaleG%10 >= 5){
					sprintf(reading[0], "ACC:%d.%d%d g",scaleG/1000, scaleG/100, 1+(scaleG/10)%10);
				}else{
					sprintf(reading[0], "ACC:%d.%d%d g",scaleG/1000, scaleG/100, (scaleG/10)%10);
				}
			 }else{
					if(scaleG%10 >= 5){
						sprintf(reading[0], "ACC:-%d.%d%d g",scaleG/1000, scaleG/100, 1+(scaleG/10)%10);
					}else{
						sprintf(reading[0], "ACC:-%d.%d%d g",scaleG/1000, scaleG/100, (scaleG/10)%10);
					}
			 }
			sprintf(reading[1],"temp:%.2f C",temp*1.0/10);
			oled_clearScreen(OLED_COLOR_WHITE);
			oled_putString(0,0,words[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			oled_putString(0,10,reading[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			oled_putString(0,20,reading[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			if(colstart == 1){
				oled_putString(0,30,words[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
				oled_putString(45,30,words[2],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			}
			if(tempstart == 1) {
				oled_putString(0,40,words[3],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			}
			}
		}
		if(x>=26 || x<= -26){
			if(colstart == 0){
				setcolstat();
			}
			oled_putString(0,30,words[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			oled_putString(45,30,words[2],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
		}
		if(temp>280) {
			if(tempstart == 0) {
				settempstat();
			}
			oled_putString(0,40,words[3],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
		}
		buttoncounter = getButtonCount();
		if(buttoncounter != 0 ){
			carstat = 0;
			buttoncounter = 0;
			resetbuttonpressed();
		}
	}
}

void ReverseMode(void){
	uint32_t light_value = 0;
	int j=0;
	int prevj;
	uint16_t ledN;
	int n;
	int x;
	uint32_t currTime = 0;
	uint32_t nextTime = 0;
	light_setRange(LIGHT_RANGE_4000);
	char words[2][100]= {"Reverse","Obstacle near"};
	char readings[100];
	led7seg_setChar('z',0,1);
	currTime = getTicks();
	oled_clearScreen(OLED_COLOR_WHITE);
	oled_putString(0,0,words[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,30,"***************",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	pca9532_setBlink1Period(75);
	pca9532_setBlink1Duty(50);
	pca9532_setBlink1Leds(0xFFF0);
	while(carstat == 2){
		nextTime = getTicks();
		prevj = j;
		j = (nextTime - currTime)/1000;
		if(j!= prevj){
			prevj = j;
			x=(j%15)*6;
			light_value = light_read();
			oled_putString(0,0,words[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			if(x==0) {
				oled_putChar(x,30,'*',OLED_COLOR_WHITE,OLED_COLOR_WHITE);
				oled_putChar(84,30,'*',OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			}else{
				oled_putChar(x,30,'*',OLED_COLOR_WHITE,OLED_COLOR_WHITE);
				oled_putChar(x-6,30,'*',OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			}

		}
		n = light_value/187.5;
		ledN = 0xffff << n;
		if(light_value>3000){
			oled_putString(0,10,words[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);

		}

		buttoncounter = getButtonCount();
		if(buttoncounter != 0 ){
				carstat = 0;
				buttoncounter = 0;
				resetbuttonpressed();
		}
	}


}
void SuperMode(void) {
	char menu[5][200]={"Speed++","Speed--","Disp. Temp.","Disp. light"};
	char movement[3][100]={"Stationary","Moving Forward","Moving Backward"};
	char progress[3][100]={"***************",">>>>>>>>>>>>>>>","<<<<<<<<<<<<<<<"};
	char currentmenu[100];
	char reading[2][100];
	int speedlevel=1;
	uint32_t lightval;
	TIM_TIMERCFG_Type timerconfig;
	TIM_MATCHCFG_Type matchconfig;
	timerconfig.PrescaleOption =TIM_PRESCALE_USVAL;
	timerconfig.PrescaleValue = 1000;
	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,&timerconfig);
	TIM_Init(LPC_TIM1,TIM_TIMER_MODE,&timerconfig);
	matchconfig.MatchChannel = 0;
	matchconfig.IntOnMatch=ENABLE;
	matchconfig.ResetOnMatch = ENABLE;
	matchconfig.ExtMatchOutputType=0;
	matchconfig.StopOnMatch=DISABLE;
	matchconfig.MatchValue=1000;
	TIM_ConfigMatch(LPC_TIM0,&matchconfig);
	TIM_ConfigMatch(LPC_TIM1,&matchconfig);
	TIM_ResetCounter(LPC_TIM0);
	TIM_ResetCounter(LPC_TIM1);
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
	TIM_Cmd(LPC_TIM0,ENABLE);
	TIM_Cmd(LPC_TIM1,ENABLE);
	NVIC_ClearPendingIRQ(EINT3_IRQn);
    LPC_GPIOINT->IO0IntClr |= 1 <<16;
    LPC_GPIOINT->IO0IntClr |= 1 <<15;
    LPC_GPIOINT->IO2IntClr |= 1 <<3;
    LPC_GPIOINT->IO0IntEnF |= 1 <<16;
    LPC_GPIOINT->IO0IntEnF |= 1 <<15;
    LPC_GPIOINT->IO2IntEnF |= 1 <<3;
    xpos=0;
    oledy=8;
    prevoledy=8;
    light_setRange(LIGHT_RANGE_4000);
    //condition for movements start here
	while(carstat==3) {
		if((supermove==0) &&(carstat==3)) {
			superstationary();
		}
		if((supermove==1) &&(carstat==3)) {
			superforward();
		}
		if((supermove==2) &&(carstat==3)) {
			superreverse();
		}
	}
}
void superstationary(void) {
	char menu[2][200]={"Disp. Temp.","Disp. light"};
	char movement[100]="Stationary";
	char progress[100]= "***************";
	char reading[2][100];
	int speedlevel = 1;
	uint32_t lightval;
	uint32_t t;
	int temp;
	xpos = 0;
	oledy=8;
	prevoledy=8;
	speedlevel = 1;
	LPC_GPIOINT->IO2IntEnF &= ~(1<<5);
	acc_init();
	oled_clearScreen(OLED_COLOR_WHITE);
	oled_putString(0,0,movement,OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,8,"->",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(12,8,menu[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,16,menu[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,24,progress,OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	while((supermove==0)&&(carstat==3)){
			tempstat=0;
			colstat=0;
			lightval=light_read();
			startmeasuretemp();
	        if(finishmeasure == 1) {
	        	t = t2-t1;
	        	t2 = 0;
	        	t1 = 0;
	        	finishmeasure = -1;
	        	temp = (2*t*10000/(340*1))-27315;
	        }
			// scol = 0, stemp=0, slgiht=0;
			if(prevoledy != oledy) {
				oled_clearLine(oledy);
				oled_clearLine(prevoledy);
				oled_putString(0,prevoledy,menu[prevoledy/8-1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
				oled_putString(0,oledy,"->",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
				oled_putString(12,oledy,menu[oledy/8-1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
				prevoledy=oledy;
			}
			if(menuselect == 1) {
					menuselect = 0;
					if(oledy==8) {
						//update temp reading at line 48
						oled_clearLine(32);
						sprintf(reading[1],"temp:%.2f C",temp*1.0/100);
						oled_putString(0,32,reading[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
					}
					if(oledy==16) {
						oled_clearLine(40);
						sprintf(reading[0],"light:%d lux",lightval);
						oled_putString(0,40,reading[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
					}
			}
			//set of leds that blink due to supermode
			pca9532_setBlink1Period(151);
			pca9532_setBlink1Duty(50);
			pca9532_setBlink1Leds(0xFF00);
			//set of leds that blink due to light reading
			pca9532_setBlink0Period(255-(lightval*255/4000));
			pca9532_setBlink0Duty(50);
			pca9532_setBlink0Leds(0x00FF);
			buttoncounter = getButtonCount();
			if(buttoncounter != 0 ){
					carstat = 0;
					buttoncounter = 0;
					resetbuttonpressed();
				}
		}
}
void superforward(void) {
	char menu[3][200]={"Speed++","Speed--","Disp. Temp."};
	char movement[100]="Moving Forward";
	char progress[100]= ">>>>>>>>>>>>>>>";
	char reading[2][100];
	int speedlevel = 1;
	int barspeed=1;
	uint32_t t;
	uint8_t buf[2];
	int32_t xoff = 0;
	int32_t yoff = 0;
	int32_t zoff = 0;
	int8_t x = 0;
	int8_t y = 0;
	int8_t z = 0;
	int temp;
	TIM_TIMERCFG_Type timerconfig;
	TIM_MATCHCFG_Type matchconfig;
	matchconfig.MatchChannel = 0;
	matchconfig.IntOnMatch=ENABLE;
	matchconfig.ResetOnMatch = ENABLE;
	matchconfig.ExtMatchOutputType=0;
	matchconfig.StopOnMatch=DISABLE;
	matchconfig.MatchValue=1000;
	xpos = 0;
	oledy=8;
	prevoledy=8;
	speedlevel = 1;
	LPC_GPIOINT->IO2IntEnF &= ~(1<<5);
	oled_clearScreen(OLED_COLOR_WHITE);
	oled_putString(0,0,movement,OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,8,"->",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(12,8,menu[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,16,menu[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,24,menu[2],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,32,progress,OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	acc_init();
	acc_read(&x,&y,&z);
	xoff= 0-x;
	yoff= 0-y;
	zoff= 64-z;

	acc_setMode(ACC_MODE_LEVEL);
	buf[0]=ACC_ADDR_INTRST;
    buf[1]=0x03;
    I2CWrite(ACC_I2C_ADDR, buf, 2);
    buf[0]=ACC_ADDR_INTRST;
    buf[1]=0x01;
    I2CWrite(ACC_I2C_ADDR, buf, 2);
    buf[0]= ACC_ADDR_CTL1;
    buf[1]=0x32;
    I2CWrite(ACC_I2C_ADDR, buf, 2);
    buf[0]= ACC_ADDR_CTL2;
    buf[1]= 0x00;
    I2CWrite(ACC_I2C_ADDR, buf, 2);
    buf[0]= ACC_ADDR_LDTH;
    buf[1]= 0x08;
    I2CWrite(ACC_I2C_ADDR, buf, 2);
    LPC_GPIOINT->IO0IntClr |= 1<<3;
    LPC_GPIOINT->IO0IntEnR |= 1<<3;
	while((supermove==1)&&(carstat==3)) {
		startmeasuretemp();
			if(finishmeasure == 1) {
			    t = t2-t1;
			     t2 = 0;
			     t1 = 0;
			     finishmeasure = -1;
			     temp = (2*t*10000/(340*1))-27315;
			     if(temp>2800) {
			        	tempstat=1;
			     }
			   }
			if(readacc==1) {
				readacc=0;
				acc_init();
				acc_read(&x,&y,&z);
				acc_setMode(ACC_MODE_LEVEL);
				x=x+xoff;
				speedlevel = speedlevel + x*2.0/64;
				if(speedlevel>=5) {
					speedlevel = 5;
				}
				if(speedlevel <=1) {
					speedlevel = 1;
				}
			}
		if(prevoledy != oledy) {
	  				oled_clearLine(oledy);
	  				oled_clearLine(prevoledy);
			   		oled_putString(0,prevoledy,menu[prevoledy/8-1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			   		oled_putString(0,oledy,"->",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			   		oled_putString(12,oledy,menu[oledy/8-1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			   		prevoledy=oledy;
			   	}
		if(menuselect == 1) {
					menuselect = 0;
					if(oledy==24) {
						//update temp reading at line 48
						oled_clearLine(40);
						sprintf(reading[1],"temp:%.2f C",temp*1.0/100);
						oled_putString(0,40,reading[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
					}
					if(oledy==8) {
						if(speedlevel<5) {
						speedlevel++;
						}
						if(barspeed<5) {
							barspeed++;
						}
						matchconfig.MatchValue=1000/barspeed;
						TIM_ResetCounter(LPC_TIM0);
						TIM_ConfigMatch(LPC_TIM0,&matchconfig);

					}
					if(oledy==16) {
						if(speedlevel>1) {
							speedlevel--;
						}
						if(barspeed>1) {
							barspeed--;
						}
						matchconfig.MatchValue=1000/barspeed;
						TIM_ResetCounter(LPC_TIM0);
						TIM_ConfigMatch(LPC_TIM0,&matchconfig);
					}
			}
		if(xpos==0) {
			oled_putChar(xpos,32,'>',OLED_COLOR_WHITE,OLED_COLOR_WHITE);
			oled_putChar(84,32,'>',OLED_COLOR_BLACK,OLED_COLOR_WHITE);
		}else{
			oled_putChar(xpos,32,'>',OLED_COLOR_WHITE,OLED_COLOR_WHITE);
			oled_putChar(xpos-6,32,'>',OLED_COLOR_BLACK,OLED_COLOR_WHITE);
		}
		pca9532_setBlink1Period(151);
		pca9532_setBlink1Duty(50);
		pca9532_setBlink1Leds(0xFF00);
		//set of leds that blink due to speed
		pca9532_setBlink0Period(255-(speedlevel*255/6));
		pca9532_setBlink0Duty(50);
		pca9532_setBlink0Leds(0x00FF);
		buttoncounter = getButtonCount();
		if(buttoncounter != 0 ){
				carstat = 0;
				buttoncounter = 0;
				resetbuttonpressed();
			}
	}
	return;
}
void superreverse(void) {
	char menu[3][200]={"Speed++","Speed--","Disp. Light"};
	char movement[100]="Moving Back";
	char progress[100]= "<<<<<<<<<<<<<<<";
	char reading[2][100];
	int speedlevel = 1;
	int barspeed=1;
	uint32_t lightval;
	int32_t xoff = 0;
	int32_t yoff = 0;
	int32_t zoff = 0;
	int8_t x = 0;
	int8_t y = 0;
	int8_t z = 0;
	TIM_TIMERCFG_Type timerconfig;
	TIM_MATCHCFG_Type matchconfig;
	matchconfig.MatchChannel = 0;
	matchconfig.IntOnMatch=ENABLE;
	matchconfig.ResetOnMatch = ENABLE;
	matchconfig.ExtMatchOutputType=0;
	matchconfig.StopOnMatch=DISABLE;
	matchconfig.MatchValue=1000;
	acc_init();
	xpos = 0;
	oledy=8;
	prevoledy=8;
	speedlevel = 1;
	oled_clearScreen(OLED_COLOR_WHITE);
	oled_putString(0,0,movement,OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,8,"->",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(12,8,menu[0],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,16,menu[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,24,menu[2],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	oled_putString(0,32,progress,OLED_COLOR_BLACK,OLED_COLOR_WHITE);
	acc_read(&x,&y,&z);
	xoff= 0-x;
	yoff= 0-y;
	zoff= 64-z;
	light_clearIrqStatus();
	light_setHiThreshold(3000);
	LPC_GPIOINT->IO2IntClr |= 1<<5;
	LPC_GPIOINT->IO2IntEnF |= 1<<5;
	while((supermove==2)&&(carstat==3)) {
		lightval= light_read();
			if(readacc==1) {
				readacc=0;
				acc_read(&x,&y,&z);
				x=x+xoff;
				speedlevel = speedlevel - x*2.0/64;
				if(speedlevel>=5) {
					speedlevel = 5;
				}
				if(speedlevel <=1) {
					speedlevel = 1;
				}
			}
		if(prevoledy != oledy) {
	  				oled_clearLine(oledy);
	  				oled_clearLine(prevoledy);
			   		oled_putString(0,prevoledy,menu[prevoledy/8-1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			   		oled_putString(0,oledy,"->",OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			   		oled_putString(12,oledy,menu[oledy/8-1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
			   		prevoledy=oledy;
			   	}
		if(menuselect == 1) {
					menuselect = 0;
					if(oledy==24) {
						//update light reading at line 40
						oled_clearLine(40);
						sprintf(reading[1],"light:%d lux",lightval);
						oled_putString(0,40,reading[1],OLED_COLOR_BLACK,OLED_COLOR_WHITE);
					}
					if(oledy==8) {
						if(speedlevel<5) {
						speedlevel++;
						}
						if(barspeed<5) {
							barspeed++;
						}
						matchconfig.MatchValue=1000/barspeed;
						TIM_ResetCounter(LPC_TIM0);
						TIM_ConfigMatch(LPC_TIM0,&matchconfig);

					}
					if(oledy==16) {
						if(speedlevel>1) {
							speedlevel--;
						}
						if(barspeed>1) {
							barspeed--;
						}
						matchconfig.MatchValue=1000/barspeed;
						TIM_ResetCounter(LPC_TIM0);
						TIM_ConfigMatch(LPC_TIM0,&matchconfig);
					}
			}
		if(xpos==0) {
			oled_putChar(84-xpos,32,'<',OLED_COLOR_WHITE,OLED_COLOR_WHITE);
			oled_putChar(0,32,'<',OLED_COLOR_BLACK,OLED_COLOR_WHITE);
		}else{
			oled_putChar(84-xpos,32,'<',OLED_COLOR_WHITE,OLED_COLOR_WHITE);
			oled_putChar(90-xpos,32,'<',OLED_COLOR_BLACK,OLED_COLOR_WHITE);
		}
		//set of leds that blink due to light value
		pca9532_setBlink1Period(255-(lightval*255/4000));
		pca9532_setBlink1Duty(50);
		pca9532_setBlink1Leds(0xFF00);
		//set of leds that blink due to speed
		pca9532_setBlink0Period(255-(speedlevel*255/6));
		pca9532_setBlink0Duty(50);
		pca9532_setBlink0Leds(0x00FF);
		buttoncounter = getButtonCount();
		if(buttoncounter != 0 ){
				carstat = 0;
				buttoncounter = 0;
				resetbuttonpressed();
			}
	}
	return;
}
int main (void) {
	//uint32_t k;
	//char letters[17]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    //int32_t xoff = 0;
    //int32_t yoff = 0;
    //int32_t zoff = 0;
    //uint32_t my_light_value = 0;
    //uint32_t currTime = 0;
    //uint32_t nexTime =0;
    //int timeseconds;
    uint32_t ans;
    //char letter;

    //int8_t x = 0;

    //int8_t y = 0;
    //int8_t z = 0;
   // uint8_t dir = 1;
  //uint8_t wait = 0;

    //uint8_t state    = 0;

    //uint8_t btn1 = 1;
    init_i2c();
    init_ssp();
    init_GPIO();

    pca9532_init();
    light_init();
    light_enable();
    joystick_init();
    acc_init();
    oled_init();
    rgb_init();
    led7seg_init();

    SysTick_Config(SystemCoreClock/1000);
    temp_init(getTicks);
    ans = NVIC_EncodePriority(0,15,0);
    NVIC_SetPriority(EINT0_IRQn,ans);
    LPC_SC ->EXTINT |= 1<<0; //clear all EINT0 interrupts
    LPC_SC ->EXTMODE |= 1<<0; //set EINT0 interrupts to be triggered upon edge
    LPC_SC ->EXTPOLAR |= 0<<0;// set EINT0 interrupts to be triggered upon falling edge
    NVIC_ClearPendingIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT0_IRQn);
    ans = NVIC_EncodePriority(0,20,0);
    NVIC_SetPriority(EINT3_IRQn,ans);
    NVIC_ClearPendingIRQ(EINT3_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);
    ans = NVIC_EncodePriority(0,2,0);
    NVIC_SetPriority(TIMER0_IRQn,ans);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    ans = NVIC_EncodePriority(0,3,0);
    NVIC_SetPriority(TIMER1_IRQn,ans);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    while (0) {
    	if(carstat == 0) {
    		StationaryMode();
    	}
    	if(carstat == 1) {
    		ForwardMode();
    	}
    	if(carstat == 2){
    		ReverseMode();
    	}
    	if(carstat == 3) {
    		SuperMode();
    	}
    }
    //int32_t my_temp_value;

    /*while (1)
    {
    	nexTime = getTicks();
    	timeseconds=(nexTime-currTime)/1000;
    	j = timeseconds % 16;
    	letter = letters[j];

        btn1 = ((GPIO_ReadValue (1) >> 31) & 0x01);
        btn2 = ((GPIO_ReadValue (2) >> 10) & 0x01);
        if (btn1 == 0 && btn2 == 0)
        {
        	rgb_setLeds(1);
        }
        if (btn1 == 0 && btn2 == 1) {
        	rgb_setLeds(2);
        }
        if (btn1 == 1 && btn2 == 0) {
        	rgb_setLeds(4);
        }
        if(btn1 == 1 && btn2 == 1) {
        	rgb_setLeds(3);
        	pca9532_setLeds(0x0000, 0xFFFF);
        }

        led7seg_setChar(letter,0);
        nexTime = getTicks();
        my_temp_value = temp_read();
        nexTime = getTicks();
        my_light_value = light_read();

        if(btn1==0) {
        printf("%2.2f degrees\n",  my_temp_value/10.0);
        printf("%d\n",  my_light_value);
        printf("%d %d %d\n",x, y, z);

        }*/


        Timer0_Wait(1);
    }


//}

void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}

