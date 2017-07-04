/*
   lir941r.c
	- kernel module entry point

   Copyright 2017  Oleg Kutkov <elenbert@gmail.com>

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

MODULE_LICENSE("GPL");

#define DRIVER_NAME "lir941r"
#define DRIVER_VERSION "0.1"
#define DRIVER_DESCRIPTION "LIR941R encoders interface driver"
#define DRIVER_COPYRIGHT "Copyright (c) 2017 Oleg Kutkov. Crimean astrophysical observatory"

#define BAR_0	0

static struct pci_device_id lir941r_id_table[] = {
	{ PCI_DEVICE(0x0F0F, 0x0F0F) },
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

static int lir941r_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	u16 vendor, device;

	pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
	pci_read_config_word(pdev, PCI_DEVICE_ID, &device);
	printk("The vendor_id is %x, the device_id is %x\n", vendor, device);

	u16 status_reg;
	pci_read_config_word(pdev, PCI_STATUS, &status_reg);

	printk("Status reg: %d\n", status_reg);

	u16 command_reg;
	pci_read_config_word(pdev, PCI_COMMAND, &command_reg);

	printk("Command reg: %d\n", command_reg | PCI_COMMAND_MEMORY);

	int err = pci_enable_device(pdev);

	if (err) {
		return err;
	}

	int bar = pci_select_bars(pdev, IORESOURCE_MEM);

	printk("Availale MEM BARs are 0x%x\n", bar);

	pci_enable_device_mem(pdev);

	err = pci_request_region(pdev, bar, DRIVER_NAME);

	if (err) {
		return err;
	}

	unsigned long mmio_start, mmio_end, mmio_len, mmio_flags;

    mmio_start = pci_resource_start(pdev, bar);
    mmio_end   = pci_resource_end(pdev, bar);
    mmio_len   = pci_resource_len(pdev, bar);
    mmio_flags = pci_resource_flags(pdev, bar);
    pr_info("Resource 0: start at 0x%lx with lenght %lu\n", mmio_start, mmio_len);

	void *ioaddr = ioremap(mmio_start, mmio_len);

	if (!ioaddr) {
		return 1;
	}

	 pr_info("Mapped resource 0x%lx to 0x%p\n", mmio_start, ioaddr);

//	u8 __iomem *ioaddr = pci_ioremap_bar(pdev, 0);

//	if (!ioaddr) {
//		return 1;
//	}

//	writel(ioaddr, 16);

//	int i = 0;

//	for (i = 0; i < 1024; i++) {
//		u32 rctl = readl(ioaddr + sizeof(u32));
//		printk("read: %x\n", rctl);
//	}

//	printk("\n");

	return 0;
}

static void lir941r_remove(struct pci_dev *pdev)
{
	pci_disable_device(pdev);
}

module_init(lir941r_init);
module_exit(lir941r_exit);

