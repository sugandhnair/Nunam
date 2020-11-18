#include <stdbool.h>

//defining the offset address after referring the data sheet
#define TIMER0_BASEADDRESS 	0x40008000
#define TASKS_START 		0x00
#define	TASKS_STOP 			0x04
#define MODE 				0x504
#define PRESCALER 			0x510
#define BITMODE 			0x508
#define CC_0				0x540
#define EVENT_COMPARE_0 	0x140
#define INTENSET 			0x304
#define INTENCLEAR			0x308
#define TASKS_CLEAR         0x00C

//global variable declerations 
int counter = 0 ;
void *(FunctionName)();

int start_timer(int timeus, void *callback) 
{

FunctionName = callback;
*(TIMER0_BASEADDRESS + TASKS_STOP) = 0x1;   //First stop the tasks
*(TIMER0_BASEADDRESS + MODE) = 0x0000; //Set timer mode
*(TIMER0_BASEADDRESS + PRESCALER) = 6; //Presclaing by 2^6 = 64, to get 250kHZ timer frequency
*(TIMER0_BASEADDRESS + BITMODE) = 0x0003; //Set timer resolution to 32 bit

/*
 one tick for every 4microseconds
 ? tick  509ms
=127250 = 0x1F112 ( hex value)
So it would take 127250ticks to reach 509ms 
*/
//2^6 / (16) = Resolution  in microseconds = 4

// Set capture reg with value obtained from the calculation = 127250
*(TIMER0_BASEADDRESS + CC_0) = timeus/4;

/* Clear timer int just in case */
*(TIMER0_BASEADDRESS + INTENCLEAR) = 0x0000;

/* Set inteurrpt for CC0 */
*(TIMER0_BASEADDRESS + INTENSET) = 0x0100;

/* Setting the 16th bit to enable cc0 int */
*(TIMER0_BASEADDRESS + INTENSET) = 1 << 16;

NVIC_EnableIRQ(TIMER0_IRQn); /* Enable NVIC interrupt in ARM ?!*/


*(TIMER0_BASEADDRESS + TASKS_START) = 1;

}

void stop_timer()
{
    *(TIMER0_BASEADDRESS + TASKS_STOP) = 0x1; //stop the task
    
    *(TIMER0_BASEADDRESS + INTENCLEAR) = 0x0000; // clear the timer
     
}


void TIMER0_IRQHandler(void)
{

	status = *(TIMER0_BASEADDRESS + EVENT_COMPARE_0);
	if (status) {
		//Interrupt flag disable
		*(TIMER0_BASEADDRESS + EVENT_COMPARE_0) = 0;
		*(TIMER0_BASEADDRESS + TASK_CLEAR) = 1;
		counter = counter + 1;  //incrementing the counter
		FunctionName();    //calling the counter print function
	}

}

void *printtext() 
{
		printf("The value of counter is : %d" , counter); //print the value of the counter 
}



int main() {

	start_timer(509000, &printtext);    //start function with 1st parameter time in microseconds and second parameter the callback function that prints out the counter value every time the counter value is incremented
	for(int i=0; i < 0xFFFF; i++);  //delay
	stop_timer()
	while(1) 
	{
        //an infinite loop 
	}


}