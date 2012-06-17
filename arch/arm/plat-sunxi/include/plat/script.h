/*
 * arch/arm/plat-sunxi/include/plat/script.h
 *
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Benn Huang <benn@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _SUNXI_PLAT_SCRIPT_H
#define _SUNXI_PLAT_SCRIPT_H

#define SYS_CONFIG_MEMBASE	(PLAT_PHYS_OFFSET + SZ_32M + SZ_16M)
#define SYS_CONFIG_MEMSIZE	(SZ_64K)

struct sunxi_script_section {
	char name[32];
	u32 count;
	u32 offset;
};

struct sunxi_script_property {
	char name[32];
	u32 offset;
	u32 pattern;
};

struct sunxi_script {
	u32 count;
	u32 version[3];
	struct sunxi_script_section section[];
};

struct sunxi_script_gpio_value {
	u32 port;
	u32 port_num;
	u32 mul_sel;
	u32 pull;
	u32 drv_level;
	u32 data;
};

enum sunxi_script_property_type {
	SUNXI_CONFIG_PROP_TYPE_INVALID = 0,
	SUNXI_CONFIG_PROP_TYPE_U32,
	SUNXI_CONFIG_PROP_TYPE_STRING,
	SUNXI_CONFIG_PROP_TYPE_U32_ARRAY,
	SUNXI_CONFIG_PROP_TYPE_GPIO,
};

/**
 * sunxi_script_property_type() - get type of a property
 */
static inline u32 sunxi_script_property_type(struct sunxi_script_property *o)
{
	return o ? (o->pattern >> 16) & 0xffff : SUNXI_CONFIG_PROP_TYPE_INVALID;
}

/**
 * sunxi_script_property_size() - get size of a property in bytes
 */
static inline u32 sunxi_script_property_size(struct sunxi_script_property *o)
{
	return o ? (o->pattern & 0xffff) << 2 : 0;
}

#define PTR(B, OFF)	(void*)((char*)(B)+((OFF)<<2))
/**
 * sunxi_script_find_section() - search for a section by name
 */
static inline struct sunxi_script_section *sunxi_script_find_section(
		struct sunxi_script *buf, const char *name)
{
	int i;
	struct sunxi_script_section *section = buf->section;
	for (i = buf->count; i--; section++)
		if (strncmp(name, section->name, sizeof(section->name)) == 0)
			return section;

	return NULL;
}

/**
 * sunxi_script_find_property() - search for a property by name in a section
 */
static inline struct sunxi_script_property *sunxi_script_find_property(
		struct sunxi_script *buf, struct sunxi_script_section *section,
		const char *name)
{
	int i;
	struct sunxi_script_property *prop = PTR(buf, section->offset);
	for (i = section->count; i--; prop++)
		if (strncmp(name, prop->name, sizeof(prop->name)) == 0)
			return prop;

	return NULL;
}

/**
 * sunxi_script_find_property2() - search for a (section, property) by name
 */
static inline struct sunxi_script_property *sunxi_script_find_property2(
		struct sunxi_script *buf, const char *name_s,
		const char *name_p)
{
	struct sunxi_script_section *section;
	struct sunxi_script_property *prop = NULL;
	section = sunxi_script_find_section(buf, name_s);
	if (section)
		prop = sunxi_script_find_property(buf, section, name_p);
	return prop;
}

/**
 * sunxi_script_find_property_fmt() - search for a property using a formated name
 */
struct sunxi_script_property *sunxi_script_find_property_fmt(
		struct sunxi_script *buf, struct sunxi_script_section *section,
		const char *fmt, ...);

/**
 * sunxi_script_property_read_u32() - read value of u32 type property
 */
static inline int sunxi_script_property_read_u32(struct sunxi_script *buf,
				     struct sunxi_script_property *prop,
				     u32 *val)
{
	if (sunxi_script_property_type(prop) == SUNXI_CONFIG_PROP_TYPE_U32) {
		u32 *v = PTR(buf, prop->offset);
		*val = *v;
		return 1;
	}
	return 0;
}

#undef PTR

#endif
