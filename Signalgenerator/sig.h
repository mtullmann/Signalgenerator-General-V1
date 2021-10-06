/*
* sig.h
*
* Created: 11.05.2021 19:49:46
*  Author: Notebook
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>
#define PULSE_PERIOD 40000 //40000*1/16000000
#define PULSE_MIN 1
#define PULSE_MAX PULSE_PERIOD

#ifndef SIG_H_
#define SIG_H_
volatile uint8_t reset_intern_cnt=0;
//volatile float step = PULSE_PERIOD/250;//250 //:15
volatile uint16_t step =0;//250 //:15
void (*sigCallback)();
uint8_t (*sigCallbackP)();

float sig_map(float x, float in_min, float in_max, float out_min, float out_max) {
	float divisor = (in_max - in_min);
	if (divisor == 0) {
		return -1; //AVR returns -1, SAM returns 0
	}
	return (x - in_min) * (out_max - out_min) / divisor + out_min;
}
void sig_high(){
	static uint16_t in=0;
	if(reset_intern_cnt==1){
		in=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	OCR1A=PULSE_MAX-1;
}
void sig_low(){
	static uint16_t in=0;
	if(reset_intern_cnt==1){
		in=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	OCR1A=PULSE_MIN-1;
}
void sig_pulse(){
	static uint16_t in=0;
	static uint8_t state=0;
	if(reset_intern_cnt==1){
		in=0;
		state=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		state=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	switch(state){
		case 0:OCR1A=PULSE_MAX-1;if(in>=(PULSE_MAX-1)/2)state=1;break;
		case 1:OCR1A=PULSE_MIN-1;break;
	}
	
}
void sig_npulse(){
	static uint16_t in=0;
	static uint8_t state=0;
	if(reset_intern_cnt==1){
		in=0;
		state=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		state=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	switch(state){
		case 0:OCR1A=PULSE_MIN-1;if(in>=(PULSE_MAX-1)/2)state=1;break;
		case 1:OCR1A=PULSE_MAX-1;break;
	}
	
}
void sig_sin(){
	static uint16_t in=0;
	uint16_t temp=0;
	if(reset_intern_cnt==1){
		in=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	temp=sig_map(sin(sig_map(in,PULSE_MIN-1,PULSE_MAX-1,0,360)*M_PI/180),-1,1,PULSE_MIN-1,PULSE_MAX-1);
	OCR1A=temp;
}
void sig_cos(){
	static uint16_t in=0;
	uint16_t temp=0;
	if(reset_intern_cnt==1){
		in=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	temp=sig_map(cos(sig_map(in,PULSE_MIN-1,PULSE_MAX-1,0,360)*M_PI/180),-1,1,PULSE_MIN-1,PULSE_MAX-1);
	OCR1A=temp;
}

void sig_saeg(){
	static uint16_t in=0;
	if(reset_intern_cnt==1){
		in=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	OCR1A=in;
	
}
void sig_nsaeg(){
	static uint16_t in=0;
	if(reset_intern_cnt==1){
		in=0;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=step+in+PULSE_MIN-1)
	{
		in=0;
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=step+in+PULSE_MIN-1;
	OCR1A=PULSE_MAX-in;
	
}
void sig_drei(){
	static uint16_t in=0;
	static int8_t ud=1;
	static uint8_t start=1;
	if(reset_intern_cnt==1){
		in=0;
		ud=1;
		start=1;
		reset_intern_cnt=0;
	}
	if(PULSE_MAX<=(step*2)+in+PULSE_MIN-1||(start==0&&in==0))
	{
		start=0;
		ud=(ud==1)?-1:1;
	}
	if(in==0&&start==0){
		if(sigCallbackP!=0)
		if(!sigCallbackP())
		return;
	}
	in=ud*(step*2)+in+PULSE_MIN-1;
	OCR1A=in;
	
}
ISR(TIMER1_OVF_vect){
	//sig_saeg();
	if(sigCallback!=0)
	sigCallback();
}
void sig_init(void (*callback)(),uint32_t period_ms){
	//step=sig_map(period,0,500000,)
	step=100/((double)period_ms/1000);
	TCCR1A&=~(1<<WGM10);//PWM
	TCCR1A|=(1<<WGM11);//PWM
	TCCR1B|=(1<<WGM12);//PWM
	TCCR1B|=(1<<WGM13);//PWM
	ICR1=PULSE_PERIOD-1;
	DDRB|=(1<<5);
	OCR1A=(PULSE_MIN-1+PULSE_MAX-1)/2;
	TCCR1B|=(1<<CS10);
	//TCCR1B|=(1<<CS11);
	
	// 	TCCR1A=((1<<WGM41));//Fast-PWM
	// 	TCCR1B=((1<<WGM42)|(1<<WGM43));
	//
	// 	TCCR1A|=(1<<COM4A1);//create PWM-signal
	//	TCCR1B|=(1<<CS41);//PS=8
	//
	// 	TIMSK1=(1<<OCIE4A);//enable compare -Match -A interrupt
	//
	// 	ICR1=PULSE_PERIOD-1;//20ms Period
	// 	OCR1A=PULSE_MIN-1;
	
	TCCR1A|=(1<<COM1A1);
	TIMSK1|=(1<<TOIE1);
	sei();
	sigCallback=callback;
	reset_intern_cnt=1;
}
void sig_callbackAfterPeriod(uint8_t (*callback)()){
	sigCallbackP=callback;
}
void sig_start(){
	TCCR1B|=(1<<CS10);
	TCCR1A|=(1<<COM1A1);
}
void sig_stop(){
	TCCR1B&=~(1<<CS10);
	TCCR1A&=~(1<<COM1A1);
}

#endif /* SIG_H_ */