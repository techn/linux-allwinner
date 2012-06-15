/*
 * Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define DEBUG
#define pr_fmt(fmt)	"sunxi: " fmt

#include <linux/module.h>

#include <mach/memory.h>
#include <plat/script.h>

/*
 * scan script.bin
 */
static int feature_name(const char *name, char *feature, int *id)
{
	char c;
	int ret = 0;
	size_t l = strlen(name);

	if (l == 0)
		goto done;

	/* UARTs and USBs have the %d at the end */
	c = name[l-1];
	if (c >= '0' && c <= '9') {
		*id = c - '0';
		--l;
	} else {
		*id = -1;
	}

	if (l==4 && strncmp(name, "usbc", 4) == 0) {
		/* USB controllers don't end in _para */
		strcpy(feature, "usb");
		ret = 1;
	} else if (l<6) {
		/* needs room for _para */
		;
	} else if (strncmp(name+l-5, "_para", 5) == 0) {
		l -= 5;
		if (*id < 0) {
			c = name[l-1];
			if (c >= '0' && c <= '9') {
				*id = c - '0';
				--l;
				/* ps2_%d_para */
				if (l>1 && name[l-1] == '_')
					--l;
			}
		}
		if (l > 0) {
			memcpy(feature, name, l);
			feature[l] = '\0';
			ret = 1;
		}
	}
done:
	return ret;
}

static int config_scan(void)
{
	int i;
	struct sunxi_script *head = (void*)__va(SYS_CONFIG_MEMBASE);
	struct sunxi_script_section *section = head->section;

	pr_debug("config: scanning %d sections at 0x%lx\n",
		 head->count, SYS_CONFIG_MEMBASE);
	for (i=head->count; i; i--, section++) {
		char feature[32] = "";
		int index = -1;

		if (feature_name(section->name, feature, &index)) {
			if (index < 0)
				pr_debug("config: [%s] -> %s\n", section->name, feature);
			else
				pr_debug("config: [%s] -> %s:%d\n", section->name, feature, index);
		} else {
			pr_debug("config: [%s] SKIP\n", section->name);
		}
	}
	return 0;
}

/*
 * module
 */
static int sunxi_plat_init(void)
{
	return config_scan();
}

static void sunxi_plat_exit(void)
{
}

module_init(sunxi_plat_init);
module_exit(sunxi_plat_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alejandro Mery <amery@geeks.cl>");
MODULE_DESCRIPTION("Generic platform-device handler for sunxi");
