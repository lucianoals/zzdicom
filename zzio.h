// High-performance buffered IO layer with transparent support for
// packet splitting and zlib compression. Note: Do not mix write
// calls to this interface with write calls through other inferfaces
// to the same file. Do not mix read or write calls to this interface
// with read calls through other interfaces to the same socket. Always
// call the flush function after writing before reading if the areas
// might overlap (taking buffer size into consideration).

#ifndef ZZIO_H
#define ZZIO_H

#if ( defined  __cplusplus ) || ( __STDC_VERSION__ < 199901L )
#define restrict
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct zzio;

// flags
#define ZZIO_ZLIB         8

/// Header write function. First parameter is number of bytes to be written in next packet.
/// Second parameter is the buffer to write into. Third parameter is user supplied.
typedef void headerwritefunc(long, char *, const void*);

/// Header read function. First parameter is buffer to read from. Second parameter is user
/// supplied. Returns number of bytes in next packet.
typedef long headerreadfunc(char *, void *);

/// Open a zzio buffer on a socket. It must already be opened and connected.
struct zzio *ziopensocket(int sock, int flags);

/// Open a zzio buffer on a file. Mode has the same meaning as for fopen(3).
struct zzio *ziopenfile(const char *path, const char *mode);

/// Change buffer size. This may flush the existing buffer.
void zisetbuffersize(struct zzio *zi, long buffersize);

/// Set up packet splitter that turns a stream of data into neat packets with custom headers. Max length of packet
/// must be equal to size of buffer as told earlier to ziopen*().
void zisplitter(struct zzio *zi, long headersize, headerwritefunc writefunc, headerreadfunc readfunc, void *userdata);

/// Return last error in human readable format.
const char *zistrerror(const struct zzio *zi);

/// Return non-zero if error flag is set. Will not clear the error flag.
int zierror(const struct zzio *zi);

/// Return the read position. Note that this is independent of write position.
long zireadpos(const struct zzio *zi);

/// Return the write position. Note that this is independent of read position.
long ziwritepos(const struct zzio *zi);

/// Set the read position. Note that this is independent of write position.
bool zisetreadpos(struct zzio *zi, long pos);

/// Set the write position. Note that this is independent of read position.
bool zisetwritepos(struct zzio *zi, long pos);

/// Get a single character from the buffer. This is much faster than ziread()
/// of a single character.
int zigetc(struct zzio *zi);

/// Put a single character. This is much faster than ziwrite() of a single character.
void ziputc(struct zzio *zi, int ch);

/// General purpose read function.
long ziread(struct zzio *zi, void *buf, long count);

/// General purpose write function.
long ziwrite(struct zzio *zi, const void *buf, long count);

/// Inform the zzio code about how much data you intend to read. zzio may use
/// this information to speed up the read.
void ziwillneed(struct zzio *zi, long offset, long length);

/// Close a buffer. Will also close the socket or file it operates on. Always returns NULL.
struct zzio *ziclose(struct zzio *zi);

/// Returns true if read position is at end of file.
bool zieof(const struct zzio *zi);

/// Flushes the write buffer to file. Note that this does not force the OS to flush its write
/// buffer to disk or to send it on the network. Also, you must flush the write buffer before
/// you read in the same area, otherwise you might get old data.
void ziflush(struct zzio *zi);

/// Flush and force OS to commit file data to storage. Does nothing for sockets. File metadata
/// (modification and creation time) may remain uncommitted.
void zicommit(struct zzio *zi);

/// Optimized way to send data from open file descriptor fd to zzio target zi
long zisendfile(struct zzio *zi, int fd, long offset, long length);

/// Optimized way to receive data from source zi to open file descriptor fd
long zirecvfile(struct zzio *zi, int fd, long offset, long length);

// void zirepeat(struct zzio *zi, int ch, long num);	// repeat character ch num times (use memset in buffer, repeatedly if necessary)

/// Write one byte at a certain position in the file without changing write position.
/// If splitter is active, or operating on a socket, the position must be within the
/// current buffer. Returns true on success.
bool ziwriteu8at(struct zzio *zi, uint8_t value, long pos);

/// Write two bytes at a certain position in the file without changing write position.
/// If splitter is active, or operating on a socket, the position must be within the
/// current buffer. Returns true on success.
bool ziwriteu16at(struct zzio *zi, uint16_t value, long pos);

/// Write four bytes at a certain position in the file without changing write position.
/// If splitter is active, or operating on a socket, the position must be within the
/// current buffer. Returns true on success.
bool ziwriteu32at(struct zzio *zi, uint32_t value, long pos);

/// Highly optimized buffer read function (implemented with memory mapping where possible) meant for sequential access patterns
void *zireadbuf(struct zzio *zi, long pos, long size);

/// Deallocate memory buffer created with zireadbuf
void zifreebuf(struct zzio *zi, void *buf, long size);


/* --------------------------------------------------------------------- */
// Private functions below - only for unit testing

struct zzio *ziopenread(const char *path, int bufsize, int flags);
struct zzio *ziopenwrite(const char *path, int bufsize, int flags);
struct zzio *ziopenmodify(const char *path, int bufsize, int flags);

#endif
