#include"exynos_4412.h"
#include"uart.h"
#include"pwm.h"
#include"adc.h"
#include"led.h"
#include"yp.h"



void led_dj(int data)
{
	if(data<25)
		{
			led(~0xf,0);
			led(8,1);
		}
		else if(data>25&&data<50)
		{
			led(~0xf,0);
			led(4,1);
		}
		else if(data>50&&data<75)
		{
			led(~0xf,0);
			led(2,1);
		}
		else
		{
			led(~0xf,0);
			led(1,1);
		} 	
}

int main()
{	
   int i;
   int x,y; 
  int flag=1;
  int fcunt=0;
   unsigned int data;
    uart_init();
	delay_init();	
	adc_init();
	delay_init();
	led_init();
	

	while(1) 
	{ 
	
	
	if((GPX1.DAT &(1<<1)) ==0)
	{
	flag=1;i=0;	
	}
	
	if((GPX1.DAT &(1<<2)) ==0)
	{
	flag=2;i=0;	
	}
	
	if((GPX3.DAT &(1<<2)) ==0)
	{	
		pwm_stop();
		flag=3;
		delayms(2);
		printf("adc=%d flag=%d\n\r",data,flag);
		while(1)
		{	
			if((GPX1.DAT &(1<<1)) ==0)
			{
				flag=1;i=0;	
				break;
			}
	
			if((GPX1.DAT &(1<<2)) ==0)
			{
				flag=1;i=0;
				break;	
			}
	
				
		}
		
	}
		fcunt=0;
		if (MyScore[i].mTime == 0) i=0;
		if (MyScore1[i].mTime == 0) i=0;
		//printf("x:% dy:%d",x,y);

		data = get_adc_data();
		data=data/40;
		printf("adc=%d flag=%d\n\r",data,flag);
		
		if(flag==1)
		{
			x=MyScore[i].mName/4;
			y=MyScore[i].mTime;
		}
		if(flag==2)
		{
			x=MyScore1[i].mName/4;
			y=MyScore1[i].mTime;	
		}
		
		pwm_init(x,data); 
		led_dj(data);
		delayms(y);
		i++; 
		pwm_start();

	} 	
    while(1);
    return 0;
}

