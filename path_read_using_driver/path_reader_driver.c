///   To make the device, Give command : mknod /dev/%s c %d 0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#include <linux/sched.h>    // current
#include <linux/dcache.h>   // d_path()
#include <linux/fs_struct.h>  // fs_struct, path
  

#include <linux/rcupdate.h>
#include <linux/fdtable.h>



#define MY_MAJOR 248
#define MY_MINOR 0
#define DEVICE_NAME "path_reader_driver"

int major, minor;
char *kernel_buffer;

struct cdev my_cdev;
int actual_rx_size=0;

int simple_open(struct inode* inode, struct file* filp)
{
  major = imajor(inode);
  minor = iminor(inode);

  printk("Device Opened --> Major : %d, Minor : %d\n", major, minor);
  printk("Kernel buffer defined\n");
  // ************************************************      Allocating memory
  kernel_buffer = (char*)kmalloc(1024,GFP_KERNEL);

  return 0;
}

int simple_close(struct file* filp)
{
  printk("Device Closed \n");
  printk("Flashed Kernel buffer\n");
  // **************************************      Free memory
  kfree(kernel_buffer);



  return 0;
}

ssize_t simple_read(struct file* filp, char* buf, size_t count, loff_t* offset)
{
  
  char *cwd;

  printk("Read function called.\n");
  struct fs_struct *fs = current->fs;
  struct path pwd;

  //get_pwd() // it also works. But, it is not thread safe mode

  get_fs_pwd( fs, &pwd);                                    // getting current path
  cwd = d_path(&pwd, kernel_buffer ,1024 * sizeof(char));   // converting to char
  
  int return_value =0;
  
  if(strlen(cwd) > count){
    printk(" current path length larger than requested buffer size\n");
    return -1;        // buffer size problem

  }

  // we have enough size to read
  return_value = copy_to_user(buf, cwd , strlen(cwd) ); // returning to the user in safe mode
  if ( return_value ){                                 // read zero byte
     printk("error in path reading\n");
     return return_value;
  }
  else{
    printk(" current path: %s\n",cwd);          // printing in kernel   
    return strlen(cwd);                         // number of characters in path  
  }

 }

ssize_t simple_write(struct file* filp, const char* buf, size_t count, loff_t* offset)
{
  
  printk("Write function called. Nothing to do\n");
  /*
  copy_from_user(kernel_buffer, buf, count);
  kernel_buffer[count] = 0;
  printk("User data : %s\n", kernel_buffer);
  actual_rx_size = count;   // Setting  the size of writing data
  */
  return 0;
}

struct file_operations my_fops =
{
	open    : simple_open,
	release : simple_close,
	read	: simple_read,
	write	: simple_write,
	owner   : THIS_MODULE,
};

int init_module()
{
  dev_t devno;
  int err;

  // Register character device
  devno = MKDEV(MY_MAJOR, MY_MINOR);
  register_chrdev_region(devno, 1, DEVICE_NAME);       //  Provide device name

  // Init device
  cdev_init(&my_cdev, &my_fops);
  my_cdev.owner = THIS_MODULE;

  // Add device to kernel
  err = cdev_add(&my_cdev, devno, 1);
  if(err<0)
  {
    printk("Driver add failed.\n");
    return -1;
  }
  

  printk("Device Driver To write is Complete\n");
  return 0;
}

void cleanup_module()
{
  dev_t devno;

  // Unregister & remove device from kernel
  devno = MKDEV(MY_MAJOR, MY_MINOR);
  unregister_chrdev_region(devno, 1);
  cdev_del(&my_cdev);
   
  
  printk("Exit from Device Driver\n");
}

MODULE_LICENSE("GPL");
