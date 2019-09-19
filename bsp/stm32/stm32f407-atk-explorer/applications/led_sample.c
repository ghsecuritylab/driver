#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <led.h>


void led_test(int argc,char **argv)
{
	char *cmd = argv[1];
//	char buffer[10];
//	rt_strncpy(cmd,buffer,5);
		
	
	switch(*cmd)
	{
		case 'a':
			led_init();
			rt_kprintf("led task init!\n");
		break;
		
		case 'b':
			led_start(90,LED_OFF);
		break;
		
		case 'c':
			led_start(90,LED_ON);
		break;
		
		case 'd':
			led_start(90,LED_FAST);
		break;
		
		case 'e':
			led_start(90,LED_SLOW);
		break;
	
		case 'f':
			led_start(90,LED_ONE);
		break;
		
		case 'g':
			led_start(90,LED_DOUBLE);
		break;
		
		case 'h':
			led_start(90,LED_THREE);
		break;
		
		case 'i':
			led_start(90,LED_VERY_SLOW);
		break;
		
		case 'j':
			led_start(89,LED_VERY_SLOW);
		break;
		
		
		case 'z':
			led_stop(90);
			rt_kprintf("led1 stop!\n");
		break;
		
		default:
			break;
	}
}
MSH_CMD_EXPORT(led_test,test led ran state);
