#include "ex.h"
// Global Variables initialised
int i, q_idx=0, n =0,leds = 0,fault=0, temp=0, T=0;  /*T = 4,5 to 6,7 toggler*/
int q_data[6], irq_counter=0; 

struct elements   // Task variables
{
	int b_T, b_T2, pwm_T, b_cnt, pwm_cnt, b;
};
// -------------- Task Table ----------------
struct elements x[8]=
{
	{1000,500,20,0,0,0},
	{1100,550,20,0,0,0},
	{1200,600,20,0,0,0},
	{1300,650,20,0,0,0},
	{700,350,10,0,0,0},
	{750,375,10,0,0,0},
	{800,400,10,0,0,0},
	{850,425,10,0,0,0}
};

//----------------------------------Sleep state----------------------------------
void __attribute__ ((naked)) __attribute__ ((noinline)) sleep_state()
{
  IRQ_ENABLE;								// Interrupt enabled
  asm ("sleep_forever: b sleep_forever");	// sleep, "asm" prevents loop elimination
}

//---------------TASK Block ----------------------
void __attribute__((naked)) task()
{
	IRQ_ENABLE;
	irq_counter++;
	while( n==2 && irq_counter>10000 && irq_counter<90000);
	
	// Counter 0-999 ms
	if(x[n].b_cnt < x[n].b_T)
	x[n].b_cnt++;
	else
	x[n].b_cnt=0;
	// Assigning brightness - increase and then decrease
	if(x[n].b_cnt < x[n].b_T2)
	x[n].b=x[n].b_cnt;
	else
	x[n].b = x[n].b_T - x[n].b_cnt;
	//PWM counter 20 ms
	if(x[n].pwm_cnt < x[n].pwm_T)
	x[n].pwm_cnt++;
	else
	x[n].pwm_cnt=0;
	// Assigning LED ON/OFF for the task
	if((x[n].pwm_cnt * x[n].b_T2) < (x[n].b * x[n].pwm_T)) 
	leds |= 1<<n;
	else
	leds &= ~(1<<n);
	
	asm("swi 0");   // Switch to Supervisor mode
}

//-------------- Dispatcher Block--------------------------
void __attribute__((naked)) dispatcher()
{ 
		if(q_idx == 6)
		{
			sleep_state();
		}
		else 
		{
			n = q_data[q_idx];
			q_idx++;
			USR_MODE;   // Switch to USER mode
			task();
		}
}


//---------------SW exception handler/SWI Block-------------------
void __attribute__ ((naked)) swi ()
{
	IRQ_ENABLE;
	*LEDS_P=leds;
	dispatcher();
}

//-----------------------IRQ handler, toggle LEDs------------------------------
void __attribute__ ((naked)) irq ()	// stack not used
{
// Timer support
  *INT_RAW_P = 0;				// Disabled fruther interrupt on the same request
  SVC_MODE;						// Switch to Supervisor mode
  *TIMER_COMPARE_P = *TIMER_P + 1;
  IRQ_ENABLE;					// Interrupt enable
  if(q_idx != 6)				// checking for fault and if true, sorting the queue elements
  {
	  fault=n;
	  if(T==0)
	  {
		for (i=0;i<6;i++)
		q_data[i]=i;
		for(i=0;i<5;i++)
		{
			if(q_data[i]==fault)
			{
				temp=q_data[i];		//Sorting the queue elements
				q_data[i]=q_data[i+1];
				q_data[i+1]=temp;
			}
		}	
		T=1;
	  }
	  else
	  {
		for (i=0;i<4;i++)
		  q_data[i]=i;
		q_data[4]=6;  		// Manually assigning the values out of 'for loop'
		q_data[5]=7;
		for(i=0;i<5;i++)
		{
			if(q_data[i]==fault)
			{
				temp=q_data[i];		//Sorting the queue elements
				q_data[i]=q_data[i+1];
				q_data[i+1]=temp;
			}
		}
		T=0;
	  }
  }
  else					// If there is no Fault
  {
	  if(T==0)
	  {
		for (i=0;i<6;i++)
		q_data[i]=i;
		T=1;
	  }
	  else
	  {
		for (i=0;i<4;i++)
		  q_data[i]=i;
		q_data[4]=6;  		// Manually assigning the values out of 'for loop'
		q_data[5]=7;
		T=0;
	  }
  }
  q_idx=0;
  dispatcher(); 
}

//---------------------------Start and initialise--------------------------------
int main ()               // INIT Block
{
  *LEDS_P = 0;
  *TIMER_COMPARE_P = *TIMER_P + 1;	// Timer initialised
  *INT_ENABLE_P |= INT_MASK_TIMER;
  sleep_state();
}
