//
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//

// Access from ARM Running Linux

#include "gpio_mmap_raspi.h" 

// GPIO access
volatile unsigned *gpio;

#ifdef GPIO_TEST

int main(int argc, char **argv)
{ 
  int g, rep;

  // Set up gpio pointer for direct register access
  raspi_gpio_setup();

  // Switch GPIO 7..11 to output mode

 /************************************************************************\
  * You are about to change the GPIO settings of your computer.          *
  * Mess this up and it will stop working!                               *
  * It might be a good idea to 'sync' before running this program        *
  * so at least you still have your code changes written to the SD-card! *
 \************************************************************************/

  // Set GPIO pins 7-11 to output
  for (g = 7; g <= 11; g++)
  {
    INP_GPIO(g); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(g);
  }

  for (rep = 0; rep < 10; rep++)
  {
     for (g = 7; g <= 11; g++)
     {
       GPIO_SET = 1 << g;
       sleep(1);
     }
     for (g = 7; g <= 11; g++)
     {
       GPIO_CLR = 1 << g;
       sleep(1);
     }
  }

  return 0;

} // main

#endif

//
// Set up a memory regions to access GPIO
//
void gpio_mmap()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
   {
      printf("can't open /dev/mem \n");
      exit (-1);
   }

   /* mmap GPIO */

   // Allocate MAP block
   if ((gpio_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
   {
      printf("allocation error \n");
      exit (-1);
   }

   // Make sure pointer is on 4K boundary
   if ((unsigned long)gpio_mem % PAGE_SIZE)
   {
       gpio_mem += PAGE_SIZE - ((unsigned long)gpio_mem % PAGE_SIZE);
   }

   // Now map it
   gpio_map = (unsigned char *)mmap(
      (caddr_t)gpio_mem,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED|MAP_FIXED,
      mem_fd,
      GPIO_BASE
   );

   if ((long)gpio_map < 0)
   {
      printf("mmap error %d\n", (int)gpio_map);
      exit (-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;


} // setup_io
