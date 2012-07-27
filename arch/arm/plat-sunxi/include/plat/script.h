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

extern const struct sunxi_script *sunxi_script_base;

struct sunxi_section {
	char name[32];
	u32 count;
	u32 offset;
};

struct sunxi_property {
	char name[32];
	u32 offset;
	u32 pattern;
};

struct sunxi_script {
	u32 count;
	u32 version[3];
	struct sunxi_section section[];
};

struct sunxi_property_gpio_value {
	u32 port;
	u32 port_num;
	u32 mul_sel;
	u32 pull;
	u32 drv_level;
	u32 data;
};

enum sunxi_property_type {
	SUNXI_PROP_TYPE_INVALID = 0,
	SUNXI_PROP_TYPE_U32,
	SUNXI_PROP_TYPE_STRING,
	SUNXI_PROP_TYPE_U32_ARRAY,
	SUNXI_PROP_TYPE_GPIO,
	SUNXI_PROP_TYPE_NULL,
};

void sunxi_script_init(const struct sunxi_script *);

/**
 * sunxi_property_type() - get type of a property
 */
static inline u32 sunxi_property_type(const struct sunxi_property *o)
{
	return o ? (o->pattern >> 16) & 0xffff : SUNXI_PROP_TYPE_INVALID;
}

/**
 * sunxi_property_size() - get size of a property in bytes
 */
static inline u32 sunxi_property_size(const struct sunxi_property *o)
{
	return o ? (o->pattern & 0xffff) << 2 : 0;
}

#define PTR(B, OFF)	(void*)((char*)(B)+((OFF)<<2))
/**
 * sunxi_property_value() - returns address where the value is stored
 */
static inline void * sunxi_property_value(const struct sunxi_property *o)
{
	return o ? PTR(sunxi_script_base, o->offset) : NULL;
}

/**
 * sunxi_first_property() - get first property of a section if exists
 */
static inline struct sunxi_property *sunxi_first_property(
		const struct sunxi_section *sp)
{
	return (sp->count > 0) ? PTR(sunxi_script_base, sp->offset) : NULL;
}
#undef PTR

/**
 * sunxi_find_section() - search for a section by name
 */
static inline const struct sunxi_section *sunxi_find_section(const char *name)
{
	int i;
	const struct sunxi_section *section = sunxi_script_base->section;
	for (i = sunxi_script_base->count; i--; section++)
		if (strncmp(name, section->name, sizeof(section->name)) == 0)
			return section;

	return NULL;
}

/**
 * sunxi_find_property() - search for a property by name in a section
 */
static inline const struct sunxi_property *sunxi_find_property(
		const struct sunxi_section *sp,
		const char *name)
{
	int i;
	const struct sunxi_property *prop = sunxi_first_property(sp);
	for (i = sp->count; i--; prop++)
		if (strncmp(name, prop->name, sizeof(prop->name)) == 0)
			return prop;

	return NULL;
}

/**
 * sunxi_find_property2() - search for a (section, property) by name
 */
static inline const struct sunxi_property *sunxi_find_property2(
		const char *secname, const char *propname)
{
	const struct sunxi_section *sp;
	const struct sunxi_property *prop = NULL;
	sp = sunxi_find_section(secname);
	if (sp)
		prop = sunxi_find_property(sp, propname);
	return prop;
}

/**
 * sunxi_find_property_fmt() - search for a property using a formated name
 */
const struct sunxi_property *sunxi_find_property_fmt(
		const struct sunxi_section *sp,
		const char *fmt, ...);

/**
 * sunxi_property_read_u32() - read value of u32 type property
 */
static inline int sunxi_property_read_u32(const struct sunxi_section *sp,
					  const char *propname,
					  u32 *val)
{
	const struct sunxi_property *pp =
		sunxi_find_property(sp, propname);
	if (pp && sunxi_property_type(pp) == SUNXI_PROP_TYPE_U32) {
		u32 *v = sunxi_property_value(pp);
		*val = *v;
		return 1;
	}
	return 0;
}

static inline int sunxi_property2_read_u32(const char *secname,
					   const char *propname,
					   u32 *val)
{
	const struct sunxi_property *pp =
		sunxi_find_property2(secname, propname);
	if (pp && sunxi_property_type(pp) == SUNXI_PROP_TYPE_U32) {
		u32 *v = sunxi_property_value(pp);
		*val = *v;
		return 1;
	}
	return 0;
}

/**
 * sunxi_script_property_read_string() - read value of string property
 */
static inline int sunxi_property_read_string(const struct sunxi_property *prop,
				     const char **val, size_t *length)
{
	enum sunxi_property_type t = sunxi_property_type(prop);
	size_t s = sunxi_property_size(prop);

	if (t == SUNXI_PROP_TYPE_STRING && s > 0) {
		const char *v = sunxi_property_value(prop);
		size_t l = s-4;
		if (v[++l] && v[++l] && v[++l])
			++l;
		*val = v;
		*length = l;
	} else if (t == SUNXI_PROP_TYPE_NULL ||
		   t == SUNXI_PROP_TYPE_STRING) {
		*val = NULL;
		*length = 0;
	} else {
		return 0;
	}
	return 1;
}

/**
 * sunxi_script_property_read_gpio() - read value of gpio property
 */
static inline int sunxi_property_read_gpio(struct sunxi_property *prop,
					struct sunxi_property_gpio_value **val)
{
	if (sunxi_property_type(prop) == SUNXI_PROP_TYPE_GPIO) {
		struct sunxi_property_gpio_value *v = sunxi_property_value(prop);
		*val = v;
		return 1;
	}
	return 0;
}
#endif
