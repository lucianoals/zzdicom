#ifndef ZZ_NET_H
#define ZZ_NET_H

#include "zz_priv.h"

// -----
// Setup functions

/// Tells the zzreceive method to fork when receiving a connection. The parent then immediately returns false. The child
/// starts to negotiate an association then returns true.
#define ZZNET_FORK 0x0001

/// Setup networking. The returned zz file pointer is still only half initialized, and needs to be further passed
/// to zzconnect or zzlisten. Network interface may be NULL.
struct zzfile *zznetwork(const char *interface, const char *myaetitle, struct zzfile *zz);

/// Add supported presentation context
bool zznetaddctx(struct zzfile *zz, const char *uid);

/// Connect to given host. Returns once a connection has been negotiated. The zz file pointer must first have been
/// passed to zznetwork().
bool zzconnect(struct zzfile *zz, const char *host, int port, const char *theiraetitle, const char *sopclass, int flags);

/// Listen to given port for connections. Returns once a connection has been negotiated. The zz file pointer must first have been
/// passed to zznetwork().
bool zzlisten(struct zzfile *zz, int port, const char *myaetitle, int flags);

// -----
// Write functions

/// Send an echo request.
void znwechoreq(struct zzfile *zz);

/// Send an echo response. Add echo message ID received as parameter.
void znwechoresp(struct zzfile *zz, long mesID);

#endif