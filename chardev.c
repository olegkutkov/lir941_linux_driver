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
#include <linux/uaccess.h>
#include <linux/fs.h>
#include "chardev.h"
#include "lir941r_user.h"
#include "lir941r_hw.h"

#define MAX_DEV 4
#define DEV_MAJOR 89

static int lirdev_open(struct inode *inode, struct file *file);
static int lirdev_release(struct inode *inode, struct file *file);
static long lirdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t lirdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t lirdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations lirdev_fops = {
	.owner      = THIS_MODULE,
	.open       = lirdev_open,
	.release    = lirdev_release,
	.unlocked_ioctl = lirdev_ioctl,
	.read       = lirdev_read,
	.write       = lirdev_write
};

struct lir_device_data {
	struct device* lirdev;
	struct cdev cdev;
};

struct lir_device_private {
	uint8_t chnum;
	struct lir941r_driver* drv;
};

static int dev_major = 0;
static struct class *lirclass = NULL;
static struct lir_device_data lirdev_data[MAX_DEV];
static struct lir941r_driver* drv_access = NULL;

static int lir941_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);

	return 0;
}

int create_char_devs(struct lir941r_driver* drv)
{
	int err, i;
	dev_t dev;

	err = alloc_chrdev_region(&dev, 0, MAX_DEV, "lir941r");

	dev_major = MAJOR(dev);

	lirclass = class_create(THIS_MODULE, "lir941r-dev");

	lirclass->dev_uevent = lir941_uevent;

	for (i = 0; i < MAX_DEV; i++) {
		cdev_init(&lirdev_data[i].cdev, &lirdev_fops);
		lirdev_data[i].cdev.owner = THIS_MODULE;
		cdev_add(&lirdev_data[i].cdev, MKDEV(dev_major, i), 1);

		lirdev_data[i].lirdev = device_create(lirclass, NULL, MKDEV(dev_major, i), NULL, "lir941r-%d", i);
	}

	drv_access = drv;

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
	struct lir_device_private* lir_priv;
	unsigned int minor = iminor(inode);

	lir_priv = kzalloc(sizeof(struct lir_device_private), GFP_KERNEL);
	lir_priv->drv = drv_access;
	lir_priv->chnum = minor;

	file->private_data = lir_priv;

	return 0;
}

static int lirdev_release(struct inode *inode, struct file *file)
{
	struct lir_device_private* priv = file->private_data;

	kfree(priv);

	priv = NULL;

	return 0;
}

static long lirdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lir_device_private* drv = file->private_data;

	switch (cmd) {
		case LIR941_START_CHANNEL_POLLING:
			start_channel_polling(drv->drv, drv->chnum);
			break;

		case LIR941_STOP_CHANNEL_POLLING:
			stop_channel_polling(drv->drv, drv->chnum);
			break;

		case LIR941_STOP_CHANNEL_ONE_REQUEST:
			channel_generate_one_req(drv->drv, drv->chnum);
			break;

		case LIR941_CHANNEL_DATAWIDTH:
			set_channel_bit(drv->drv, drv->chnum, (uint8_t)arg);
			break;

		case LIR941_CHANNEL_SPEED:
			set_channel_clk(drv->drv, drv->chnum, (uint16_t)arg);
			break;

		case LIR941_CHANNEL_PAUSE:
			set_channel_pause_rate(drv->drv, drv->chnum, (uint16_t)arg);
			break;

		default:
			return -EINVAL;
	};

	return 0;
}

static ssize_t lirdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	struct lir_device_private* drv = file->private_data;
	uint32_t data;

	data = get_channel_data(drv->drv, drv->chnum);

	if (copy_to_user(buf, &data, count)) {
		return -EFAULT;
	}

	return count;
}

static ssize_t lirdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	return count;
}

