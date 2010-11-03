#ifndef ZZ_WRITE_H
#define ZZ_WRITE_H

#include "zz_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

void zzwAE(struct zzfile *zz, zzKey key, const char *string);
void zzwAS(struct zzfile *zz, zzKey key, const char *string);
void zzwCS(struct zzfile *zz, zzKey key, const char *string);
void zzwOB(struct zzfile *zz, zzKey key, const char *string, int length);
void zzwOW(struct zzfile *zz, zzKey key, const char *string, int length);
void zzwSH(struct zzfile *zz, zzKey key, const char *string);
void zzwUI(struct zzfile *zz, zzKey key, const char *string);
void zzwUL(struct zzfile *zz, zzKey key, uint32_t value);
void zzwSQ(struct zzfile *zz, zzKey key, uint32_t size);
void zzwUN(struct zzfile *zz, zzKey key, uint32_t size);

void zzwHeader(struct zzfile *zz, const char *sopclass, const char *sopinstanceuid, const char *transfer);
void zzwEmpty(struct zzfile *zz, zzKey key, const char *vr);
struct zzfile *zzcreate(const char *filename, struct zzfile *zz, const char *sopclass, const char *sopinstanceuid, const char *transfer);

#ifdef __cplusplus
}
#endif

#endif