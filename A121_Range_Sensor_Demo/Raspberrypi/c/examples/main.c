#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "DEV_Config.h"

#ifdef A121_DISTANCE_DETECTOR
#include "A121_Distance_Detector.h"
#elif A121_PRESENCE_DETECTOR
#include "A121_Presence_Detector.h"
#elif A121_CARGO_EXAMPLE
#include "A121_Cargo_Example.h" 
#elif A121_BREATHING_REFERENCE
#include "A121_Breathing_Reference.h"
#endif

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Program stop\r\n"); 
    DEV_Module_Exit();
    exit(0);
}

int main(int argc, char **argv)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    DEV_Module_Init();
    DEV_GPIO_Mode(A121_BUSY_PIN,INPUT);
    DEV_I2C_Init(A121_DEV_ADDR);

    A121_init();
    while (1)
    {
        if(DEV_Digital_Read(A121_BUSY_PIN))
        {
#ifdef A121_DISTANCE_DETECTOR
            A121_Get_Distance_mm();
#elif A121_PRESENCE_DETECTOR
            A121_Get_Presence_Detector();
#elif A121_CARGO_EXAMPLE
            A121_Get_Cargo_application();
#elif A121_BREATHING_REFERENCE
            A121_Get_Breathing_application();
#endif
        }    
        DEV_Delay_ms(10);
    }
	DEV_Module_Exit();
    return 0; 
}
