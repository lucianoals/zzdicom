#include "zz_priv.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Verifies:
//  - group lengths
//  - groups in ascending order
//  - elements in ascending order

void dump(char *filename)
{
	struct zzfile szz, *zz;
	uint16_t group, element, lastgroup = 0xffff, lastelement = 0;
	long len, groupsize = 0, grouppos = 0, pos;

	zz = zzopen(filename, "r", &szz);

	while (zz && !feof(zz->fp) && !ferror(zz->fp))
	{
		zzread(zz, &group, &element, &len);

		if (group != lastgroup)
		{
			if (groupsize != 0)
			{
				if (ftell(zz->fp) - grouppos != groupsize)
				{
					fprintf(stderr, "Wrong group %x size - told it was %ld, but it was %ld\n", 
					        group, groupsize, ftell(zz->fp) - grouppos);
				}
			}

			if (element == 0x0000)
			{
				groupsize = zzgetuint32(zz);
				fseek(zz->fp, -4, SEEK_CUR);
			}
			groupsize = 0;
			grouppos = ftell(zz->fp);
			if (lastgroup != 0xffff && lastgroup != 0xfffe && group < lastgroup)
			{
				fprintf(stderr, "Group 0x%04x - order not ascending (last is 0x%04x)!\n", group, lastgroup);
			}
			lastgroup = group;
		}
		if (element < lastelement)
		{
			fprintf(stderr, "Element %x - order not ascending!\n", element);
		}

		pos = ftell(zz->fp);
		if ((len > 0 && len != 0xFFFFFFFF) || pos == zz->fileSize)
		{
			if (pos + len > zz->fileSize)
			{
				fprintf(stderr, "(%04x,%04x) -- size %u exceeds file end\n", group, element, (unsigned int)len);
			}

			// Abort early, skip loading pixel data into memory if possible
			if (pos + len >= zz->fileSize)
			{
				break;
			}

			// Skip ahead
			if (!feof(zz->fp))
			{
				fseek(zz->fp, len, SEEK_CUR);
			}
		}
	}
	if (zz && ferror(zz->fp))
	{
		fprintf(stderr, "%s read error: %s\n", filename, strerror(errno));
	}
	zz = zzclose(zz);
}

int main(int argc, char **argv)
{
	int i;

	for (i = zzutil(argc, argv, 2, "<filenames>", "Verify validity of a DICOM file"); i < argc; i++)
	{
		dump(argv[i]);
	}

	return 0;
}
