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
#define pr_fmt(fmt)	"sunxi: " fmt

#include <linux/module.h>

/*
 * module
 */
static int sunxi_plat_init(void)
{
	pr_debug("init");
	return 0;
}

static void sunxi_plat_exit(void)
{
	pr_debug("exit");
}

module_init(sunxi_plat_init);
module_exit(sunxi_plat_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alejandro Mery <amery@geeks.cl>");
MODULE_DESCRIPTION("Generic platform-device handler for sunxi");
