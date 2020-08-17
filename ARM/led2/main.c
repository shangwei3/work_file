#include"exynos_4412.h" 
int main()
{
int cnt=0;
int cn;
while(1)
{	
	if((GPX1.DAT &(1<<1)) ==0)
	{
		cnt++;
		cn=cnt % 4;

		if(cn==0){
			GPX2.CON = (GPX2.CON & ~(0xf<<28)) | (1<<28);
			GPX2.DAT |= 1<<7;//GPX2.PUD &= ~(3<<14);	
			}
		if(cn==1){
			GPX1.CON |=1;
			GPX1.DAT |=1;
			}
		if(cn==2){	
			GPF3.CON = (GPX2.CON & ~(0xf<<16)) | (1<<16);
			GPF3.DAT |= 1<<4;
			}
		if(cn==3){
			GPF3.CON = (GPX2.CON & ~(0xf<<20)) | (1<<20);
			GPF3.DAT |= 1<<5;
			}
		
		while(1)
		{
			if( (GPX1.DAT & (1<<1)) !=0 )
			break;	
		}
			
		if(cn>4)
			cn=0; 	
	}
	
		GPX2.DAT &= 0<<7;
		GPX1.DAT &= 0;
		GPF3.DAT &= 0<<4;
		GPF3.DAT &= 0<<5;
			

}	
	

	return 0;
}
