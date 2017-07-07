/*
   chardev.c
	- kernel module character devices implementation

   Copyright 2017  Oleg Kutkov <elenbert@gmail.com>
                   Crimean astrophysical observatory

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include "chardev.h"

#define MAX_DEV 4
#define DEV_MAJOR 89

static int lirdev_open(struct inode *inode, struct file *file);
static int lirdev_release(struct inode *inode, struct file *file);
static long lirdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int lirdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static int lirdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations lirdev_fops = {
	.owner      = THIS_MODULE,
	.open       = lirdev_open,
	.release    = lirdev_release,
	.unlocked_ioctl = lirdev_ioctl,
	.read       = lirdev_read,
	.write       = lirdev_write
};

struct lir_device_data {
	void *hw;
	struct device* lirdev;
	struct cdev cdev;
};


static int dev_major = 0;
static struct class *lirclass = NULL;
static struct lir_device_data lirdev_data[MAX_DEV];

int create_char_devs(void)
{
	int err, i;
	dev_t dev;

	err = alloc_chrdev_region(&dev, 0, MAX_DEV, "lir941r");

	dev_major = MAJOR(dev);

	lirclass = class_create(THIS_MODULE, "lir941r-dev");

	for (i = 0; i < MAX_DEV; i++) {
		cdev_init(&lirdev_data[i].cdev, &lirdev_fops);
		lirdev_data[i].cdev.owner = THIS_MODULE;
		cdev_add(&lirdev_data[i].cdev, MKDEV(dev_major, i), 1);

		lirdev_data[i].lirdev = device_create(lirclass, NULL, MKDEV(dev_major, i), NULL, "lir941r-%d", i);
	}

	return 0;
}

int destroy_char_devs(void)
{
	int i;

	for (i = 0; i < MAX_DEV; i++) {
		device_destroy(lirclass, MKDEV(dev_major, i));
	}

	class_unregister(lirclass);
	class_destroy(lirclass);
	unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);

	return 0;
}

static int lirdev_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);

	printk("lirdev open, minor %i\n", minor);

	file->private_data = lirdev_data[minor].hw;

	return 0;
}

static int lirdev_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;

	return 0;
}

static long lirdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int lirdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	char* tmp = "Hello from kernel world!\n"

	

	return 0;
}

static int lirdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	char* tmp;

	tmp = memdup_user(buf, count);

	printk("lirdev writing %s\n", tmp);

	kfree(tmp);

	return count;
}

