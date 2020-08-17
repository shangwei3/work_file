#ifndef _LED_HEAD_
#define _LED_HEAD_

#define	LED_IOCTL_BASE	'L'

#define LEDIOC_ON   _IO(LED_IOCTL_BASE,0)
#define LEDIOC_OFF  _IO(LED_IOCTL_BASE,1)


#endif
