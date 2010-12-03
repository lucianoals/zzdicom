#ifndef ZZ_PRIV_H
#define ZZ_PRIV_H

#include "zz.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

#define UID_LittleEndianImplicitTransferSyntax			"1.2.840.10008.1.2"
#define UID_LittleEndianExplicitTransferSyntax			"1.2.840.10008.1.2.1"
#define UID_BigEndianExplicitTransferSyntax			"1.2.840.10008.1.2.2"
#define UID_JPEGLSLosslessTransferSyntax			"1.2.840.10008.1.2.4.80"
#define UID_DeflatedExplicitVRLittleEndianTransferSyntax	"1.2.840.10008.1.2.1.99"

#define UID_RawDataStorage					"1.2.840.10008.5.1.4.1.1.66"
#define UID_SpatialRegistrationStorage				"1.2.840.10008.5.1.4.1.1.66.1"
#define UID_SpatialFiducialsStorage				"1.2.840.10008.5.1.4.1.1.66.2"
#define UID_SecondaryCaptureImageStorage			"1.2.840.10008.5.1.4.1.1.7"
#define UID_MultiframeSingleBitSecondaryCaptureImageStorage	"1.2.840.10008.5.1.4.1.1.7.1"
#define UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage	"1.2.840.10008.5.1.4.1.1.7.2"
#define UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage	"1.2.840.10008.5.1.4.1.1.7.3"
#define UID_MultiframeTrueColorSecondaryCaptureImageStorage	"1.2.840.10008.5.1.4.1.1.7.4"

#define MAX_LEN_UID		(64 + 1)
#define MAX_LEN_PN		(64 * 5 + 1)
#define MAX_LEN_DATETIME	(26 + 1)
#define MAX_LEN_CS		(16 + 1)
#define UNLIMITED		(long)0xffffffff

/// Enumerant for Value Representations. Approach taken from XMedCon.
enum VR
{
	AE = ('A'<<8)|'E',
	AS = ('A'<<8)|'S',
	AT = ('A'<<8)|'T',
	CS = ('C'<<8)|'S',
	DA = ('D'<<8)|'A',
	DS = ('D'<<8)|'S',
	DT = ('D'<<8)|'T',
	FL = ('F'<<8)|'L',
	FD = ('F'<<8)|'D',
	IS = ('I'<<8)|'S',
	LO = ('L'<<8)|'O',
	LT = ('L'<<8)|'T',
	OB = ('O'<<8)|'B',
	OW = ('O'<<8)|'W',
	OF = ('O'<<8)|'F',
	PN = ('P'<<8)|'N',
	SH = ('S'<<8)|'H',
	SL = ('S'<<8)|'L',
	SQ = ('S'<<8)|'Q',
	SS = ('S'<<8)|'S',
	ST = ('S'<<8)|'T',
	TM = ('T'<<8)|'M',
	UI = ('U'<<8)|'I',
	UL = ('U'<<8)|'L',
	US = ('U'<<8)|'S',
	UN = ('U'<<8)|'N',
	UT = ('U'<<8)|'T',
	/* special tag (multiple choices) */
	OX = ('O'<<8)|'X',
	/* special tag (no info - implicit syntax) */
	NO = ('N'<<8)|'O',
};

#define MAX_LEN_VR 3	///< 2 chars for VR and one for terminating null
#define ZZ_VR(_m1, _m2) ((_m1 << 8) | _m2)

/// dest must be a string of at least 3 letters long
static inline const char *zzvr2str(enum VR vr, char *dest)
{
	dest[0] = vr >> 8;
	dest[1] = vr & 0xff;
	dest[2] = '\0';
	return dest;
}

struct part6
{
	uint16_t group;		// private
	uint16_t element;	// private
	const char *VR;
	const char *VM;
	bool retired;
	const char *description;
};

enum zztxsyn
{
	ZZ_IMPLICIT,
	ZZ_EXPLICIT,
	ZZ_EXPLICIT_COMPRESSED
};

enum zzpxstate
{
	ZZ_NOT_PIXEL,
	ZZ_PIXELDATA,
	ZZ_OFFSET_TABLE,
	ZZ_PIXELITEM
};

enum zzsteptype
{
	ZZ_BASELINE,
	ZZ_GROUP,
	ZZ_SEQUENCE,
	ZZ_ITEM
};

/// Maximum amount of recursiveness in a DICOM file
#define MAX_LADDER 16

struct zzfile
{
	FILE		*fp;
	long		fileSize;
	char		fullPath[PATH_MAX];
	char		sopClassUid[MAX_LEN_UID];	// TODO convert to enum
	char		sopInstanceUid[MAX_LEN_UID];
	bool		acrNema, part10;
	time_t		modifiedTime;
	int		currNesting, nextNesting, ladderidx;
	enum zzpxstate	pxstate;

	struct
	{
		enum VR		vr;
		long		length;
		uint16_t	group;
		uint16_t	element;
		long		pos;
	} current;

	struct
	{
		long		pos;		// file position where group begins, this - 4 is value position (except for group zero)
		long		size;		// size of group/sequence
		enum zztxsyn	txsyn;		// transfer syntax of this group
		uint16_t	group;		// if group type, which group
		enum zzsteptype	type;		// type of group
	} ladder[MAX_LADDER];
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// inspired by linux kernel sources
#define MIN(x, y) ({                                \
            typeof(x) _min1 = (x);                  \
            typeof(y) _min2 = (y);                  \
            (void) (&_min1 == &_min2);              \
            _min1 < _min2 ? _min1 : _min2; })
 
#define MAX(x, y) ({                                \
            typeof(x) _max1 = (x);                  \
            typeof(y) _max2 = (y);                  \
            (void) (&_max1 == &_max2);              \
            _max1 > _max2 ? _max1 : _max2; })

float zzgetfloat(struct zzfile *zz, int index);
double zzgetdouble(struct zzfile *zz, int index);
uint32_t zzgetuint32(struct zzfile *zz, int index);
uint16_t zzgetuint16(struct zzfile *zz, int index);
int32_t zzgetint32(struct zzfile *zz, int index);
int16_t zzgetint16(struct zzfile *zz, int index);
char *zzgetstring(struct zzfile *zz, char *input, long strsize);
char *zztostring(struct zzfile *zz, char *input, long strsize);

/// From the current file position, start reading DICOM tag information.
bool zzread(struct zzfile *zz, uint16_t *group, uint16_t *element, long *len);

struct zzfile *zzopen(const char *filename, const char *mode, struct zzfile *infile);
const struct part6 *zztag(uint16_t group, uint16_t element);
static inline struct zzfile *zzclose(struct zzfile *zz) { if (zz) { fclose(zz->fp); } return NULL; }

/// Utility function to process some common command-line arguments. Returns the number of initial arguments to ignore.
int zzutil(int argc, char **argv, int minArgs, const char *usage, const char *help);

/// Utility iterator that wraps zzread. Passing in a NULL pointer for zz makes it a no-op.
void zziterinit(struct zzfile *zz);
bool zziternext(struct zzfile *zz, uint16_t *group, uint16_t *element, long *len);

#ifdef __cplusplus
}
#endif

#endif
