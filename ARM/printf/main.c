#include"exynos_4412.h" 
#include"uart.h" 
int main()
{
	char a;
	int i=0;
	int j=0;
	int k=0;
	printf("hello"); 
	led_init();
	uart_init();
	GPX1.CON |=1;
	while(1)
	{
		
		if(UART2.UTRSTAT2 & 1)
		{
			while( !(UART2.UTRSTAT2 & (1<<1)));
			a= UART2.URXH2;
			UART2.UTXH2  =a;
			if(a =='1')
			{	
				i++;
				if(i==1)
				{
					while(1)
					{
						if(UART2.UTRSTAT2 & 1)
						{
							while( !(UART2.UTRSTAT2 & (1<<1)));
							i++;
							a= UART2.URXH2;
							UART2.UTXH2  =a;
						
							if(i>=2)//
							{
								led(~0xf,0);
								i=0;
								break;	
							}
							if(a =='2')
								goto loop;	
						}
						j++;
						if(j>100000)
						{	k++;
							//led(0xf,1);
							if(k==1)
								led(1,1);
							if(k==2)
								led(2,1);
							if(k==3)
								led(4,1);
							if(k==4)
								led(8,1);
							if(k==4)
								k=0;
							///////	
							j=0;
						}
							
						if(j==0)
						{	while(1)
							{	
								j++;
								if(j>100000)
								{
									led(~0xf,0);
									j=0;
									break;	
								}
							}	
						}	
					}
				}
				
				if(i>=2)
				{
					loop:
					led(~0xf,0);
					i=0;	
				}
					
				
					
				//GPX1.DAT |=1;	
			}
			if(a =='2')
			{	//loop:
				led(~0xf,0);
				GPX1.DAT &= ~1;
			}
			
		}
	}

	return 0;
}
