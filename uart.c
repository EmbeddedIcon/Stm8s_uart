#include <iostm8s103.h>

void initHW(void);
#define Bufmax 198
@interrupt void ISRTIM1(void);
@interrupt void ISRUART1(void);
@interrupt void ISRTIM2(void);
@near unsigned char Buffer[200];
unsigned int autocounter,del,time;
unsigned char wptr=0,rptr=0;
unsigned char getbyte;

_Bool	CPULED	@PC_ODR:4;

void SendSerial(unsigned char c)
{
	while (!(UART1_SR & 0x80));
	for(del=0;del<100;del++){}
	UART1_DR = c;
}

void SendSerialString(unsigned char *str)
{
	while(*str != '\0')
	{
		SendSerial(*str);
		str++;
	}
}

void main()
{
	_asm("sim"); 				// disable interrupts
	initHW();
	_asm("rim");		
	//SendSerial('M');
	SendSerialString("Hi Embedded ICON \r\n");
	while(1)
	{
		if(rptr != wptr)		
		{
			getbyte = Buffer[rptr];
			rptr++;
			if(rptr > Bufmax)
				rptr = 0;
			SendSerial(getbyte);
		}
	}
}

@interrupt void ISRUART1(void)
{
	unsigned char c;
	if(UART1_SR > 0)
	{			
		if((UART1_SR & 0x20) != 0)
		{
			c = UART1_DR;
			Buffer[wptr] = c;
			wptr++;
			if(wptr > Bufmax)
				wptr = 0;
		}
		UART1_SR = 0;		
	}
}

@interrupt void ISRTIM2(void)		//ISR timer3 interrupt(11 millisec)
{
	TIM2_SR1 = 0;
}

@interrupt void ISRTIM1(void)		//ISR timer3 interrupt(11 millisec)
{
	if(time == 1000)
  {
		CPULED =! CPULED;
	}
	time++;
	if(time > 1000)
		time=0;
	TIM1_SR1 = 0;
}

void Init_Timer2(void)
{
	TIM2_CR1 = 0; 
	TIM2_IER = 0;
	TIM2_SR1 = 0;	
	TIM2_SR2 = 0;
	TIM2_EGR = 0;
	TIM2_CCMR1 = 0;
	TIM2_CCMR2 = 0;
	TIM2_CCMR3 = 0;
	TIM2_CCER1 = 0;
	TIM2_CCER2 = 0;
	TIM2_CNTRH = 0;
	TIM2_CNTRL = 0;
	TIM2_PSCR = 0;
	TIM2_ARRH  = 0;
	TIM2_ARRL  = 0;
	TIM2_CCR1H = 0;
	TIM2_CCR1L = 0;
	TIM2_CCR2H = 0;
	TIM2_CCR2L = 0;
	TIM2_CCR3H = 0;
	TIM2_CCR3L = 0;

}

void Init_Timer1(void)
{
	TIM1_CR1 = 0;
	TIM1_IER = 0;
	TIM1_SR1 = 0;	
	TIM1_SR2 = 0;
	TIM1_EGR = 0;
	TIM1_CCER1 = 0;
	TIM1_CCER2 = 0;
	TIM1_CCMR1 = 0;
	TIM1_CCMR2 = 0;
	TIM1_CCMR3 = 0;
	TIM1_CNTRH = 0;
	TIM1_CNTRL = 0;
	TIM1_PSCRH = 0;
	TIM1_PSCRL = 0;
	TIM1_ARRH  = 0;
	TIM1_ARRL  = 0;
	TIM1_CCR1H = 0;
	TIM1_CCR1L = 0;
	TIM1_CCR2H = 0;
	TIM1_CCR2L = 0;
	TIM1_CCR3H = 0;
	TIM1_CCR3L = 0;
	TIM1_SR1 = 0;
}

void initHW()
{
	unsigned char tmp = UART1_SR;
	tmp = UART1_DR;
//====================== Initialise System Clock =========================
	CLK_ICKCR = 		0;
	CLK_ECKCR = 		0;
	CLK_ICKCR = 		1;
	while((CLK_ICKCR & 2) == 0);
	CLK_CKDIVR = 	0;
	CLK_PCKENR1 = 0xFF;
	CLK_PCKENR2 = 0xFF;
	CLK_CCOR = 			0;
	CLK_HSITRIMR = 	0;
	CLK_SWIMCCR = 	0;
	CLK_SWR = 		0xE1;
	CLK_SWCR = 			0; 
	CLK_SWCR |= 	0x02;
	while ((CLK_SWCR & 1)== 1);
	
//========================= Set up I/O ports ============================		
	PA_ODR = 0xFF;	PA_DDR = 0xFF;	PA_CR1 = 0xFF;	PA_CR2 = 0x00;	PA_ODR = 0xFF;

	PB_ODR = 0xFF;	PB_DDR = 0xFF;	PB_CR1 = 0xFF;	PB_CR2 = 0x00;	PB_ODR = 0xFF;
	
	PC_ODR = 0xFF;	PC_DDR = 0xFF;	PC_CR1 = 0xFF;	PC_CR2 = 0x00;	PC_ODR = 0xFF;
	
	PD_ODR = 0x20;	PD_DDR = 0x20;	PD_CR1 = 0x20;	PD_CR2 = 0x20;	PD_ODR = 0x20;
//========================= Timer1 init ========================== 		*/
	Init_Timer1();
	TIM1_PSCRH = 0;
	TIM1_PSCRL = 1;	
	TIM1_ARRH = 0x1F;		//  High byte reload dgtue // 52 micro sec 
	TIM1_ARRL = 0x40; 		//  Low byte reload dgtue
	TIM1_IER |= 1;       	//  Enable the update interrupts.
	TIM1_CR1 |= 1;       	//  Finally enable the timer.
	
	Init_Timer2();
	TIM2_PSCR = 0;
	TIM2_ARRH = 0x06;		//  High byte reload value // 10 micro sec 
	TIM2_ARRL = 0x40; 		//  Low byte reload value
	TIM2_IER |= 1;       	//  Enable the update interrupts.
	TIM2_CR1 |= 1;       	//  Finally enable the timer.

/*========================== UART =========================== */
	UART1_CR1 = 0;
	UART1_CR2 = 0;
	UART1_CR4 = 0;
	UART1_CR3 = 0;
	UART1_CR5 = 0;
	UART1_GTR = 0;
	UART1_PSCR = 0;			
	UART1_BRR2 = 0x0b;
	UART1_BRR1 = 0x08;
	UART1_CR3 |= 7;
	UART1_CR2 |= 0x0C;
	UART1_CR2 |= 0x20;
	
}

/*
0,CONNECT

+IPD,0,3:%H$
+IPD,0,4:%11$
+IPD,0,4:%21$
+IPD,0,4:%31$
+IPD,0,4:%41$
+IPD,0,4:%51$
+IPD,0,14:%Pyercaud1234$
OK
> 
busy s...

Recv 6 bytes

SEND OK
0,CLOSED*/