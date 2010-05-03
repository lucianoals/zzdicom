#ifndef ZZ_PRIV_H
#define ZZ_PRIV_H

#include "zz.h"

#include <stdio.h>

struct part6
{
	uint16_t group;		// private
	uint16_t element;	// private
	const char *VR;
	const char *VM;
	bool retired;
	const char *description;
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

FILE *zzopen(const char *filename, const char *mode);
bool zzread(FILE *fp, uint16_t *group, uint16_t *element, uint32_t *len);
const struct part6 *zztag(uint16_t group, uint16_t element);

#endif
