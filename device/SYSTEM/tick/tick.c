#include "tick.h"

volatile uint32_t sysTickMillis;
static const    uint32_t sysTickPerUs  = 72;

/* ¼ì²éÊÇ·ñÒç³ö */
int systick_check_underflow(void)
{
   return SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
}

uint32_t micros(void)
{
   uint32_t cycle, timeMs;
 
   do
    {
       timeMs = sysTickMillis;
       cycle  = SysTick->VAL;
       
       __ASM volatile("nop");
       
   }while (timeMs != sysTickMillis);
 
   if (systick_check_underflow())
   {
       timeMs++;
       
       cycle = SysTick->VAL;
   }
 
   return (timeMs * 1000) + (SysTick->LOAD + 1 - cycle) / sysTickPerUs;
}

uint32_t millis(void)
{
   return sysTickMillis;
}
 
 /* Î¢ÃëÑÓÊ±º¯Êı */
void delay_us_(uint32_t us) 
{
    static int currentTime, lastTime = 0;
 
    lastTime= micros();
        
    while(1)
    {       
         currentTime= micros();
 
         if ( currentTime - lastTime >=  us )
         {
            break;
         }
    }
}
 
/* ºÁÃëÑÓÊ±º¯Êı */
void delay_ms_(uint32_t ms)  
{
    static int currentTime, lastTime = 0;
 
    lastTime= millis();
        
    while(1)
    {       
        currentTime= millis();
 
        if( currentTime - lastTime >=  ms )
        {
            break;
        }
    }
}
