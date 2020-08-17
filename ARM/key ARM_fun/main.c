
#include "exynos_4412.h"
#include "uart.h"
#include "key.h"
#include "led.h"

void irq_func(int irq)
{
	char flag;
	flag = get_led_flag();
	printf("irq=%d\n\r",irq);
	switch(irq)
	{
		case K2:
			led(flag ^ 1, (flag ^ 1)& 1);
			break;
		case K3:
			led(flag ^ 2, (flag ^ 2)& 2);
			break;
		case K4:
			led(flag ^ 4, (flag ^ 4)& 4);
			break;
	}

}

int main()
{
    int a = 100;
	led_init();
    uart_init();
    printf("hello!a=%d\r\n",a);

	key_init(irq_func );

    while(1);
    return 0;
}



