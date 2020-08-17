#include"exynos_4412.h" 
int main()
{

			
//	if((GPX1.DAT &(1<<1)) ==0)

		GPD0.CON |=1;
		GPD0.DAT |=1;
		GPD0.PUD &=~0x3;
		
		while(1)

	return 0;
}
