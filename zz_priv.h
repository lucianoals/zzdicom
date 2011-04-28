#ifndef ZZ_PRIV_H
#define ZZ_PRIV_H

#include "zz.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__linux__) || defined(__linux)
#define ZZ_LINUX
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

#define MAX_LEN_UI		(64 + 1)
#define MAX_LEN_PN		(64 * 5 + 1)
#define MAX_LEN_DATETIME	(26 + 1)
#define MAX_LEN_CS		(16 + 1)
#define MAX_LEN_DA		(8 + 1)
#define MAX_LEN_DS		(16 + 1)
#define MAX_LEN_DT		(26 + 1)
#define MAX_LEN_IS		(12 + 1)
#define MAX_LEN_TM		(16 + 1)
#define UNLIMITED		(long)0xffffffff
#define MAX_LEN_LO		(64 + 1)

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
	OX = ('?'<<8)|'=',
	/* special tag (no info - implicit syntax) */
	NO = ('-'<<8)|'-',
	/* special tag (hack to work around ?? as 2-byte length field VR) */
	HACK_VR = ('?'<<8)|'?',
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

enum zztxsyn
{
	ZZ_IMPLICIT,
	ZZ_EXPLICIT,
	ZZ_EXPLICIT_COMPRESSED,
	ZZ_EXPLICIT_JPEGLS,
	ZZ_TX_LAST
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
#define MAX_LADDER 24

struct zzfile
{
	FILE		*fp;			///< File pointer or network socket
	long		fileSize;		///< File or current buffer size
	char		fullPath[PATH_MAX];
	char		sopClassUid[MAX_LEN_UI];	// TODO convert to enum
	char		sopInstanceUid[MAX_LEN_UI];
	char		characterSet[MAX_LEN_CS];	// TODO convert to enum
	bool		acrNema, part10;
	time_t		modifiedTime;
	int		currNesting, nextNesting, ladderidx;
	long		pxOffsetTable, frames;

	struct
	{
		FILE		*buffer;	///< Memory buffer disguised as a file
		char		callingaet[17];	///< Our AE Title
		char		interface[64];	///< Our network interface
		void		*mem;		///< Pointer to buffer's memory
		long		maxpdatasize;	///< Maximum size of other party's pdata payload
		uint16_t	version;	///< Association version capability bitfield
		long		lastMesID;	///< Last message ID used, to make it unique
		// TODO enum zztxsyn psctxs[PSCTX_LAST];        ///< List of negotiated presentation contexts
	} net;

	struct
	{
		enum VR		vr;
		long		length;
		uint16_t	group;
		uint16_t	element;
		long		pos;		// start of a current tag's data segment
		long		frame;		// current frame number, or -1, zero indexed
		char		warning[MAX_LEN_LO];	// if !valid, this string is set to an explanation
		bool		valid;		// whether the current tag is valid, true unless issue found
		enum zzpxstate	pxstate;
	} current;

	struct
	{
		uint16_t	group;
		uint16_t	element;
		int		ladderidx;
	} previous;

	struct
	{
		long		pos;		// file position where group begins, this - 4 is value position (except for group zero)
		long		size;		// size of group/sequence
		enum zztxsyn	txsyn;		// transfer syntax of this group
		uint16_t	group;		// which group we are in
		uint16_t	element;	// which element we are in
		enum zzsteptype	type;		// type of group
		int		item;		// item number in sequence, or -1 if not in a sequence, zero indexed
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

/// Read a DS VR into an array of doubles that is len long, return number of elements found
int zzrDS(struct zzfile *zz, int len, double *input);

/// Read a IS VR into an array of long that is len long, return number of elements found
int zzrIS(struct zzfile *zz, int len, long *input);

float zzgetfloat(struct zzfile *zz, int idx);
double zzgetdouble(struct zzfile *zz, int idx);
uint32_t zzgetuint32(struct zzfile *zz, int idx);
uint16_t zzgetuint16(struct zzfile *zz, int idx);
int32_t zzgetint32(struct zzfile *zz, int idx);
int16_t zzgetint16(struct zzfile *zz, int idx);
char *zzgetstring(struct zzfile *zz, char *input, long strsize);
bool zztostring(struct zzfile *zz, char *input, long strsize);

/// From the current file position, start reading DICOM tag information.
bool zzread(struct zzfile *zz, uint16_t *group, uint16_t *element, long *len);

struct zzfile *zzopen(const char *filename, const char *mode, struct zzfile *infile);
struct zzfile *zzclose(struct zzfile *zz);

/// Utility function to process some common command-line arguments. Returns the number of initial arguments to ignore.
int zzutil(int argc, char **argv, int minArgs, const char *usage, const char *help);

/// Utility iterator that wraps zzread. Passing in a NULL pointer for zz makes it a no-op.
void zziterinit(struct zzfile *zz);
bool zziternext(struct zzfile *zz, uint16_t *group, uint16_t *element, long *len);

/// Extra low-level verification of current tag. Resides in zzverify.c
bool zzverify(struct zzfile *zz);

#ifdef __cplusplus
}
#endif

#endif
