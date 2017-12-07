///  Give command : mknod /dev/%s c %d 0

///  To make the device


#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>



#include <linux/sched.h>  // current
#include <linux/dcache.h>   // d_path()
#include <linux/fs_struct.h>  // fs_struct, path
  

#define MY_MAJOR 248
#define MY_MINOR 0
#define DEVICE_NAME "my_char_device"

int major, minor;
char *kernel_buffer;

struct cdev my_cdev;
int actual_rx_size=0;

int simple_open(struct inode* inode, struct file* filp)
{
  major = imajor(inode);
  minor = iminor(inode);

  printk("Device Opened --> Major : %d, Minor : %d\n", major, minor);

  return 0;
}

int simple_close(struct file* filp)
{
  printk("Device Closed \n");



  return 0;
}

ssize_t simple_read(struct file* filp, char* buf, size_t count, loff_t* offset)
{
  size_t bytes = 0;
  
  printk("Read function called.\n");
  
  char mess[] = {" kernel read "};
  int return_value = copy_to_user(buf, kernel_buffer , strlen(kernel_buffer) );
  if( return_value ){
    printk("Could not read\n");
    return -EFAULT;
  }
  else{
      printk("read done successfully %s\n", mess);
      
      return bytes;
  }
  

 }

ssize_t simple_write(struct file* filp, const char* buf, size_t count, loff_t* offset)
{
  printk("Write function called. Nothing to do\n");
  
  copy_from_user(kernel_buffer, buf, count);
  kernel_buffer[count] = 0;
  printk("User data : %s\n", kernel_buffer);
  actual_rx_size = count;   // Setting  the size of writing data
  
  return count;
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
  // ************************************************      Allocating memory
  kernel_buffer = (char*)kmalloc(1024,GFP_KERNEL);


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
   
  // **************************************      Free memory
  kfree(kernel_buffer);

  printk("Exit from Device Driver\n");
}

//module_init(simple_init);
//module_exit(simple_exit);

MODULE_LICENSE("GPL");
