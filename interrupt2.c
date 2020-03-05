#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h> 
#include <linux/fs.h> 
#include <asm/uaccess.h> 
#include <linux/wait.h> 
#include <linux/sched.h> 
#include <linux/delay.h> 
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#define DRIVER_AUTHOR "Juan"
#define DRIVER_DESC "Pixurrilla"
#define GPIO_BUTTON1 2
#define GPIO_BUTTON2 3
#define GPIO_SPEAKER 4
#define GPIO_GREEN1 27
#define GPIO_GREEN2 22
#define GPIO_YELLOW1 17
#define GPIO_YELLOW2 11
#define GPIO_RED1 10
#define GPIO_RED2 9
static int LED_GPIOS[]= {GPIO_GREEN1, GPIO_GREEN2, GPIO_YELLOW1, GPIO_YELLOW2, GPIO_RED1, GPIO_RED2} ;
static char *led_desc[]= {"GPIO_GREEN1","GPIO_GREEN2","GPIO_YELLOW1","GPIO_YELLOW2","GPIO_RED1","GPIO_RED2"} ;

static unsigned long ticks_unsegundo;
static void timer_handler(unsigned long);

DEFINE_TIMER(mytimer name, timer_hander antisalto, 0 time, 0 dato); 

static void timer_handler(unsigned long dato)
{
	printk(KERN_NOTICE "%s : Timer run at %u\n", KBUILD_MODNAME, (unsigned) jiffies);
	
	mod_timer(&mytimer, jiffies + ticks_unsegundo * 1);
}

static int my_init(void)
{
	ticks_unsegundo = msecs_to_jiffies(1000);
	printk(KERN_NOTICE "Hola, cargando módulo! Ticks para un segundo: %u\n", KBUILD_MODAME, (unsigned) jiffies);
	return 0;
}

static void my_exit(void)
{
	del_timer(&mytimer);
	printk(KERN_NOTICE "Adiós, saliendo del modulito pixín!\n", KBUILD_MODNAME);
}
	

/****************************************************************************/
/* LEDs write/read using gpio kernel API */
/****************************************************************************/
static void byte2leds(char ch)
{
 int i;
 int val=(int)ch;
for(i=0; i<6; i++) gpio_set_value(LED_GPIOS[i], (val >> i) & 1);
}
static char leds2byte(void)
{
 int val;
 char ch;
 int i;
 ch=0;
for(i=0; i<6; i++)
 {
 val=gpio_get_value(LED_GPIOS[i]);
 ch= ch | (val << i);
 }
 return ch;
}
/****************************************************************************/
/* LEDs device file operations */
/****************************************************************************/
static ssize_t leds_write(struct file *file, const char __user *buf,
 size_t count, loff_t *ppos)
{
char ch;
if (copy_from_user( &ch, buf, 1 )) {
 return -EFAULT;
 }
printk( KERN_INFO " valor recibido: %d\n",(int)ch);
byte2leds(ch);
return 1;
}
static ssize_t leds_read(struct file *file, char __user *buf,
 size_t count, loff_t *ppos)
{
 char ch;
if(*ppos==0) *ppos+=1;
 else return 0;
ch=leds2byte();
if(copy_to_user(buf,&ch,1)) return -EFAULT;
return 1;
}
static const struct file_operations leds_fops = {
 .owner = THIS_MODULE,
 .write = leds_write,
 .read = leds_read,
};
/****************************************************************************/
/* LEDs device struct */
static struct miscdevice leds_miscdev = {
 .minor = MISC_DYNAMIC_MINOR,
 .name = "leds",
 .fops = &leds_fops,
};
/*****************************************************************************/
/* This functions registers devices, requests GPIOs and configures interrupts */
/*****************************************************************************/
/*******************************
 * register device for leds
 *******************************/
static void r_dev_config(void)
{
 int ret;
 ret = misc_register(&leds_miscdev);
 if (ret < 0) {
 printk(KERN_ERR "misc_register failed\n");
 return;
 }
printk(KERN_NOTICE " leds_miscdev.minor =%d\n", leds_miscdev.minor);

}
/*******************************
 * request and init gpios for leds
 *******************************/
static void r_GPIO_config(void)
{
 int i;
 for(i=0; i<6; i++)
 {
 if (gpio_request_one(LED_GPIOS[i], GPIOF_INIT_LOW, led_desc[i])) 
 {
 printk("GPIO request faiure: led GPIO %d %s\n",LED_GPIOS[i], led_desc[i]);
 return ;
 }
 gpio_direction_output(LED_GPIOS[i],0);
 }
}
/****************************************************************************/
/* Module init / cleanup block. */
/****************************************************************************/
static int r_init(void) {
printk(KERN_NOTICE "Hello, loading %s module!\n", KBUILD_MODNAME);
 printk(KERN_NOTICE "%s - devices config\n", KBUILD_MODNAME);
 r_dev_config();
 printk(KERN_NOTICE "%s - GPIO config\n", KBUILD_MODNAME);
 r_GPIO_config();
return 0;
}
static void r_cleanup(void) {
 int i;
 printk(KERN_NOTICE "%s module cleaning up...\n", KBUILD_MODNAME);
 for(i=0; i<6; i++)
 {
 gpio_set_value(LED_GPIOS[i], 0);
 gpio_free(LED_GPIOS[i]);
 }
 if (leds_miscdev.this_device) misc_deregister(&leds_miscdev);
 printk(KERN_NOTICE "Done. Bye from %s module\n", KBUILD_MODNAME);
return;
}
module_init(r_init);
module_exit(r_cleanup);
/****************************************************************************/
/* Module licensing/description block. */
/****************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
