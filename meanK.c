/*---------------------------------------------
 * Diseño de Sistemas Operativos (DAC) *
	para compilar el módulo: 
	$ MODULE=fortune_mod_v2 make
	para instalar el módulo:
	$ sudo insmod fortune_mod_v2.ko 
	para comprobar si el módulo fue cargado:
	$ sudo lsmod
	$ dmesg | tail
	para desinstalar el módulo:
	$ sudo rmmod fortune_mod_v2
----------------------------------------------*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#define MAX_LENGTH       PAGE_SIZE

static struct proc_dir_entry *proc_entry;
static int int_index;  // Index to write next number
static int next_integer;  // Index to read next number
static int contador;	//cantidad de numeros leidos
static int suma;	//suma de los numeros leidos


/****************************************************************************/
/* file operations                                                          */
/****************************************************************************/
// escritura del fichero
ssize_t mean_write (struct file *filp,const char *buf, size_t count, loff_t *off)
{
  char mi_buffer[16];
  int numero;
  
  int space_available = (MAX_LENGTH-int_index)+1;
  if (count > 16) {
    printk(KERN_INFO "demasiados datos, max 16 bytes de longitud!\n");
    return -ENOSPC;
  }
  if (copy_from_user( mi_buffer, buf, count )) {  //para copiar del buffer del usuario al mio del kernel
    return -EFAULT;
  }
  
  if(sscanf(mi_buffer, " %d ", &numero) != 1){
	printk(KERN_INFO "El dato no es un entero\n");
	return -EINVAL;
  }
  
  printk(KERN_INFO "El dato es %d\n", numero);
  suma+=numero;
  contador++;
  
  printk(KERN_INFO "acumulado %d, cuenta = %d\n", suma,contador);
  
  
  int_index += count;
  *off+=count;  // avanzo el offset del fichero
  return count;
}

// lectura del fichero
ssize_t mean_read (struct file *filp, char __user *buf, size_t count, loff_t *off )
{
  int parteEntera, parteDecimal, len;
  char mi_buffer[16];
  
  if(*off>0) return 0;
  
  //si la cantidad de numeros leida es 0, lo advierto con un mensaje, si no devuelvo media
  if (count <= 0){
	  len=printk(KERN_INFO "la cantidad de numeros leidos es 0\n");
	  return 0;
  }else{
	  parteEntera=suma/contador;
	  parteDecimal=((suma-(parteEntera*contador))*100/contador);
	  len=printk(KERN_INFO "La cantidad de numeros leidos es %d y la media aritmetica es %d.%d", contador, parteEntera,parteDecimal);
  }
   
  copy_from_user(buf,mi_buffer,len);
  
  *off+=len+1;			//avanzo offset del fichero
 
  return len+1;			//devuelvo número de bytes leidos (incluido \n)
}


struct file_operations proc_fops = {
	read : mean_read,
	write: mean_write
};


/****************************************************************************/
/* Module init / cleanup block.                                             */
/****************************************************************************/

// al cargar el modulo
int init_mean_module( void )
{
  int ret = 0;
 
  proc_entry = proc_create("mean",0666,NULL,&proc_fops);

   
   

    if (proc_entry == NULL) {
      ret = -ENOMEM;
      printk(KERN_INFO "mean: Couldn't create proc entry\n");
    } else {
      int_index = 0;
      next_integer = 0;
      printk(KERN_INFO "mean: Module loaded.\n");
    }
  return ret;
}

// al descargar el modulo
void cleanup_mean_module( void )
{
  remove_proc_entry("mean", NULL);
  
  printk(KERN_INFO "mean: Module unloaded.\n");
}

module_init( init_mean_module );
module_exit( cleanup_mean_module );



/****************************************************************************/
/* Module licensing/description block.                                      */
/****************************************************************************/


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mean Kernel Module (DSO)");
MODULE_AUTHOR("basado en el trabajo de M. Tim Jones, adaptado para kernel > 3.10");
