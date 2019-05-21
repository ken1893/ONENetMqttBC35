#ifndef _BEEP_H_
#define _BEEP_H_





typedef struct
{

	_Bool Beep_Status;

} BEEP_INFO;

#define BEEP_ON		1

#define BEEP_OFF	0

#define LED_TURN           GPIOB->ODR ^= GPIO_Pin_5

extern BEEP_INFO beep_info;


void Beep_Init(void);

void Beep_Set(_Bool status);


#endif
