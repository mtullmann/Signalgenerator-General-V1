/*
* Signalgenerator.c
*
* Created: 11.05.2021 19:47:35
* Author : Notebook
*/

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "sig.h"
uint32_t period=1000;
uint8_t callP(){
	static uint8_t state=1;
	PORTB|=(1<<6);
	switch(state++){
 		case 0:sig_init(sig_drei,period);break;
		case 1:sig_init(sig_nsaeg,period);break;
		case 2:sig_init(sig_sin,period);break;
		case 3:sig_init(sig_cos,period);break;
 		case 4:sig_init(sig_saeg,period);break;
 		case 5:sig_init(sig_npulse,period);break;
 		case 6:sig_init(sig_pulse,period);state =0;break;
 		//case 4:sig_init(sig_drei,0);break;
 		default:state=0;
		//sig_init(sig_saeg,0);
		break;
	}
	PORTB&=~(1<<6);
	
	return 0;
}
int main(void)
{
	sig_init(sig_sin,period);
	/* Replace with your application code */
	DDRB|=(1<<6);
	//PORTB|=(1<<6);
	//sig_start();
	sig_callbackAfterPeriod(callP);
	//sig_init(sig_pulse,20);
	while(1){
		
		
	}
}

