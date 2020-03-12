#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GLP");
MODULE_AUTHOR("Juan Álvaro Caravaca Seliva");
MODULE_DESCRIPTION("Killing your PC since 1997");

#define MAX_XURRA_LENGTH       PAGE_SIZE

static struct proc_dir_entry *proc_entry;
/*static char *xurra_space;  // Space for Mean strings
static int xurra_index;  // Index to write next Mean
static int next_mean;  // Index to read next Mean */
static int suma_numeros=0;
static int cantidad_numeros=0;

/****************************************************************************/
/* file operations                                                          */
/****************************************************************************/

// escritura del fichero
#define MAXSIZE[32]
ssize_t mean_write (struct file *filp,const char __user *buf, size_t count, loff_t *off)
{
	char copia_de_buf[16];
	char *dynamicbuffer;
	int space_available[16];
	int numero;
	if (count >16) {
		printk(KERN_INFO "demasiados datos, max = 16 bytes long!\n");
		return -ENOSPC;
		}
		dynamicbuffer=(char*)vmalloc(count);
		
		if(dynamicbuffer==NULL) return -ENOMEM;
		
	if(*off>0) return 0;
	
	if(count>32) count=32;
	
  if (copy_from_user( copia_de_buf, buf, (count>32)?32:count)) {
    vfree(dynamicbuffer);
    return -EFAULT;
    }
    
   if (sscanf(copia_de_buf, " %d ", &numero)==1)
   {
	 suma_numeros+=numero;
	 cantidad_numeros+=1; 
   }
   else if( strncmp(copia_de_buf,"CLEAR",5)==0)
   {
	    suma_numeros=0;
		cantidad_numeros=0; 
	}   
   else
   {
   vfree(dynamicbuffer);
   return -EINVAL;
   }
   	     
  *off+=count; // avanza el counter del fichero
  return count;
}

// lectura del fichero
ssize_t mean_read (struct file *filp, char __user *buf, size_t count, loff_t *off )
{
	int len;
	int entera,fraccion;
	char respuesta[32];
	if(*off>0) return 0;
	if(cantidad_numeros>0)
	{
		entera=suma_numeros/cantidad_numeros;
		fraccion=((suma_numeros-(entera*cantidad_numeros))*100/cantidad_numeros);
		len=sprintf(respuesta,"La media es %d.%d de %d números\n",entera,fraccion,cantidad_numeros);
	
    } 
    else
    {
		len=sprintf(respuesta, "No hay datos, pixita\n");
	}
		    
	copy_to_user(buf,respuesta,len);
	*off+=len;
	return len;
}	

struct file_operations proc_fops = {
	read : mean_read,
    write: mean_write,
};

/****************************************************************************/
/* Module init / cleanup block.                                             */
/****************************************************************************/

// al cargar el módulo
int init_mean_module( void )
{
  int ret = 0;
  
  proc_entry = proc_create("mean",0666,NULL,&proc_fops);
  printk(KERN_INFO "Iniciando\n");
  return ret;
}
	
// al descargar el modulo
void cleanup_mean_module( void )
{
  remove_proc_entry("mean", NULL);
  printk(KERN_INFO "Mean: Módulo descargado, pixita mia.\n");
}
module_init( init_mean_module );
module_exit( cleanup_mean_module );
