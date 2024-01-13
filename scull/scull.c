#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("Dual BSD/GPL");

static int scull_major = 0;
static int scull_minor = 0;
static int scull_nr_devs = 4;

module_param(scull_major, int, S_IRUGO);

static dev_t dev = 0;
static struct cdev my_cdev;

struct file_operations scull_fops = {
	.owner = THIS_MODULE,
};

static int __init scull_init(void){
	printk(KERN_INFO "scull: Allocating char device...\n");
	
	int result;
	if(scull_major){
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	}

	else{
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
		scull_major = MAJOR(dev);
	}

	if (result < 0){
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return result;
	}

	printk(KERN_INFO "scull: Major is %d", scull_major);

	cdev_init(&my_cdev, &scull_fops);
	printk(KERN_INFO "scull: Initialized cdev\n");

	result = cdev_add(&my_cdev, dev, scull_nr_devs);
	if (result < 0){
		printk(KERN_WARNING "scull: couldn't add cdev\n");
		return result;
	}
	else {
		printk(KERN_INFO "scull: added cdev\n");
	}

	return 0;
}

static void __exit scull_exit(void){
	printk(KERN_INFO "scull: Exiting...\n");
	unregister_chrdev_region(dev, scull_nr_devs);
	printk(KERN_INFO "scull: Unregistered device\n");
	cdev_del(&my_cdev);
	printk(KERN_INFO "scull: Deleted cdev\n");
}

module_init(scull_init);
module_exit(scull_exit);
