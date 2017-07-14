/*
   lir941r_main.c
	- kernel module entry point

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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include "chardev.h"
#include "lir941r.h"

MODULE_LICENSE("GPL");

#define DRIVER_NAME "lir941r"
#define DRIVER_VERSION "0.1"
#define DRIVER_DESCRIPTION "LIR941R encoders interface driver"
#define DRIVER_COPYRIGHT "Copyright (c) 2017 Oleg Kutkov. Crimean astrophysical observatory"

static struct pci_device_id lir941r_id_table[] = {
	{ PCI_DEVICE(LIR_941_VENDOR_ID_1, LIR_941_PRODUCT_ID_1) },
	{0,}
};

MODULE_DEVICE_TABLE(pci, lir941r_id_table);

static int lir941r_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void lir941r_remove(struct pci_dev *pdev);

static struct pci_driver lir941r_driver = {
	.name = DRIVER_NAME,
	.id_table = lir941r_id_table,
	.probe = lir941r_probe,
	.remove = lir941r_remove
};

static int __init lir941r_init(void)
{
	printk("Loading %s - version %s\n", DRIVER_DESCRIPTION, DRIVER_VERSION);
	printk("%s\n", DRIVER_COPYRIGHT);

	return pci_register_driver(&lir941r_driver);
}

static void __exit lir941r_exit(void)
{
	pci_unregister_driver(&lir941r_driver);
}

int read_device_config(struct pci_dev *pdev)
{
	u16 vendor, device, status_reg, command_reg;

	pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
	pci_read_config_word(pdev, PCI_DEVICE_ID, &device);

	printk(KERN_INFO "LIR device vid: 0x%X  pid: 0x%X\n", vendor, device);

	pci_read_config_word(pdev, PCI_STATUS, &status_reg);

	printk(KERN_INFO "LIR device status reg: 0x%X\n", status_reg);


	pci_read_config_word(pdev, PCI_COMMAND, &command_reg);

	if (command_reg | PCI_COMMAND_MEMORY) {
		printk(KERN_INFO "LIR device supports memory access\n");

		return 0;
	}

	printk(KERN_ERR "LIR device doesn't supports memory access!");

	return -1;
}

void release_device(struct pci_dev *pdev)
{
	pci_release_region(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
	pci_disable_device(pdev);
}

static int lir941r_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int bar, err;	
	unsigned long mmio_start,mmio_len;
	struct lir941r_driver *drv_priv;

	if (read_device_config(pdev) < 0) {
		return -EIO;
	}

	bar = pci_select_bars(pdev, IORESOURCE_MEM);

	printk(KERN_INFO "LIR device availale MEM BAR are 0x%x\n", bar);

	err = pci_enable_device_mem(pdev);

	if (err) {
		printk(KERN_ERR "Failed to enable LIR device memory, err: %i\n", err);
		return err;
	}

	err = pci_request_region(pdev, bar, DRIVER_NAME);

	if (err) {
		pci_disable_device(pdev);
		return err;
	}

	mmio_start = pci_resource_start(pdev, 0);
	mmio_len   = pci_resource_len(pdev, 0);

	printk(KERN_INFO "LIR device resource 0: start at 0x%lx with lenght %lu\n", mmio_start, mmio_len);

	drv_priv = kzalloc(sizeof(struct lir941r_driver), GFP_KERNEL);

	if (!drv_priv) {
		release_device(pdev);
		return -ENOMEM;
	}

	drv_priv->hwmem = ioremap(mmio_start, mmio_len);

	if (!drv_priv->hwmem) {
		release_device(pdev);
		return -EIO;
	}

	printk(KERN_INFO "LIR device mapped resource 0x%lx to 0x%p\n", mmio_start, drv_priv->hwmem);

	create_char_devs();

	pci_set_drvdata(pdev, drv_priv);

	return 0;
}

static void lir941r_remove(struct pci_dev *pdev)
{
	struct lir941r_driver *drv_priv = pci_get_drvdata(pdev);

	destroy_char_devs();

	if (drv_priv) {
		if (drv_priv->hwmem) {
			iounmap(drv_priv->hwmem);
		}

		kfree(drv_priv);
	}

	release_device(pdev);

	printk("Unloaded %s\n", DRIVER_DESCRIPTION);
}

module_init(lir941r_init);
module_exit(lir941r_exit);

