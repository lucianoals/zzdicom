#ifndef ZZ_TEXTURE_H
#define ZZ_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/gl.h>
#include "zz_priv.h"

struct zztexture
{
	/// Slices are stored as a variety of texture types, all unsigned fixed point.
	/// The pixels are not transformed through the modality LUT.
	GLint volume;

	/// Positions are stored as a 3D texture of grayscale floating point values,
	/// with length of each slice exactly 12 points (9 orientation, 3 position)
	GLint positions;

	/// Each slice has a LUT. Each LUT is stored as a sequence of floating point
	/// interval values. A simple linear LUT for example can be stored as two
	/// values. They are stored in a 3D grayscale texture. The LUT index values
	/// must be reverse transformed through the modality LUT before uploading.
	GLint luts;

	/// Frame of reference UID for coordinate space
	char frameOfReferenceUid[MAX_LEN_UID];

	/// Size of coordinate space
	struct
	{
		GLfloat x, y, z;
	} coordsize;

	/// Size of pixel space
	struct
	{
		int x, y, z;
	} pixelsize;

	/// Modality LUT transform (an actual LUT is not supported though)
	struct
	{
		int intercept, slope;
	} rescale;
};

/// Generate and upload 3D textures for visualization from a single multi-frame DICOM file. OpenGL
/// context must be initialized first.
struct zztexture *zzcopytotexture(struct zzfile *zz, struct zztexture *zzt);

/// Bind shaders and textures for 2D visualization. Pass in NULL to unbind and reset OpenGL states.
bool zztextureslicing(struct zztexture *zzt);

/// Bind shaders and textures for 3D visualization. Pass in NULL to unbind and reset OpenGL states.
// TODO
//bool zztextureraycast(struct zztexture *zzt);

/// Remove data from GPU memory
bool zztexturefree(struct zztexture *zzt);

#ifdef __cplusplus
}
#endif

#endif
