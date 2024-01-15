#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define SCULL_DEV_NO 4

MODULE_LICENSE("Dual BSD/GPL");

static int scull_major = 0;
static int scull_minor = 0;
static int scull_nr_devs = 4;
static int scull_qset = 1000;

module_param(scull_major, int, S_IRUGO);

static dev_t dev = 0;

struct scull_qset{
	void **data;
	struct scull_qset * next;
};

struct scull_dev{
	struct cdev cdev;
	struct scull_qset *data;
	int qset;
};

int scull_trim(struct scull_dev *dev){
	struct scull_qset *dptr, *next;
	int i;
	int qset =  dev->qset;
	for (dptr = dev->data; dptr; dptr = next){
		if (dptr->data){
			for(i = 0; i < qset; i++){
				kfree(dptr->data[i]);
			}
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}

	dev->data = NULL;
	dev->qset = scull_qset;
	return 0;
}

int scull_open(struct inode * inode, struct file * filp){
	struct scull_dev * dev;
	dev = container_of(inode->i_cdev, struct scull_dev, cdev);

	filp->private_data = dev;

	if((filp->f_flags & O_ACCMODE) == O_WRONLY){
		scull_trim(dev);
	}

	printk(KERN_INFO "scull: Opened device %d\n", iminor(inode));

	return 0;
}

int scull_release(struct inode * inode, struct file * filp){
	printk(KERN_INFO "scull: Released device %d\n", iminor(inode));
	return 0;
}

static struct scull_dev scull_devp_array[SCULL_DEV_NO];

struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.open = scull_open,
	.release = scull_release
};

static void scull_setup_cdev(struct scull_dev *dev, int index){
	dev_t dev_no = MKDEV(scull_major, scull_minor + index);
	cdev_init(&dev->cdev, &scull_fops);
	printk(KERN_INFO "scull: Initialized Major: %d, Minor: %d\n", scull_major, scull_minor + index);

	int result = cdev_add(&dev->cdev, dev_no, 1);
	if (result < 0){
		printk(KERN_ERR "scull: Failed to add Major: %d, Minor: %d\n", scull_major, scull_minor + index);
	}
}

static void scull_rm_cdev(struct scull_dev *dev){
	printk(KERN_INFO "scull: Removing cdev\n");
	cdev_del(&dev->cdev);
}

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

	for(int i = scull_minor; i < scull_nr_devs; i++){
		scull_setup_cdev(&scull_devp_array[i], i);
	}
	return 0;
}

static void __exit scull_exit(void){
	printk(KERN_INFO "scull: Exiting...\n");
	unregister_chrdev_region(dev, scull_nr_devs);
	printk(KERN_INFO "scull: Unregistered device\n");
	for(int i = scull_minor; i < scull_nr_devs; i++){
		scull_rm_cdev(&scull_devp_array[i]);
		printk(KERN_INFO "scull: Freeing scull_dev %d\n", i);
	}
}

module_init(scull_init);
module_exit(scull_exit);
