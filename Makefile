CFLAGS = -Wall -Wextra -DPOSIX -Werror -Wshadow -Wformat-security
COMMON = zz.o
COMMONSQL = zzsql.o
COMMONWRITE = zzwrite.o
COMMONTEXTURE = zztexture.o
COMMONVERIFY = zzverify.o
PART6 = part6.o
PROGRAMS = zzanon zzdump zzgroupfix zzread zzstudies zzprune zztojpegls zzmkrandom zznifti2dcm
HEADERS = zz.h zz_priv.h zzsql.h zzwrite.h part6.h zztexture.h

all: CFLAGS += -Os
all: sqlinit.h $(PROGRAMS)

debug: CFLAGS += -O0 -g -DDEBUG -fprofile-arcs -ftest-coverage
debug: sqlinit.h $(PROGRAMS) check

%.o : %.c
	$(CC) -o $@ $< -c $(CFLAGS)

sqlinit.h: SQL
	echo "const char *sqlinit =" > sqlinit.h
	cat SQL | sed s/^/\"/ | sed s/$$/\"/ >> sqlinit.h
	echo ";" >> sqlinit.h

zzanon: zzanon.c $(HEADERS) $(COMMON)
	$(CC) -o $@ $< $(COMMON) $(CFLAGS)

zzdump: zzdump.c $(HEADERS) $(COMMON) $(PART6) $(COMMONVERIFY)
	$(CC) -o $@ $< $(COMMON) $(CFLAGS) $(PART6) $(COMMONVERIFY)

zzgroupfix: zzgroupfix.c $(HEADERS) $(COMMON)
	$(CC) -o $@ $< $(COMMON) $(CFLAGS)

zzread: zzread.c $(HEADERS) $(COMMON) $(COMMONSQL)
	$(CC) -o $@ $< $(COMMON) $(COMMONSQL) $(CFLAGS) -lsqlite3

zzstudies: zzstudies.c $(HEADERS) $(COMMON) $(COMMONSQL)
	$(CC) -o $@ $< $(COMMON) $(COMMONSQL) $(CFLAGS) -lsqlite3

zzprune: zzprune.c $(HEADERS) $(COMMON) $(COMMONSQL)
	$(CC) -o $@ $< $(COMMON) $(COMMONSQL) $(CFLAGS) -lsqlite3

zztojpegls: zztojpegls.c $(HEADERS) $(COMMON) $(COMMONWRITE) $(PART6)
	$(CC) -o $@ $< $(COMMON) $(COMMONWRITE) $(CFLAGS) -lCharLS $(PART6)

zzmkrandom: zzmkrandom.c $(HEADERS) $(COMMON) $(COMMONWRITE)
	$(CC) -o $@ $< $(COMMON) $(CFLAGS) $(COMMONWRITE)

zznifti2dcm: zznifti2dcm.c $(HEADERS) nifti1.h $(COMMON) $(COMMONWRITE)
	$(CC) -o $@ $< $(COMMON) $(CFLAGS) $(COMMONWRITE)

clean:
	rm -f *.o sqlinit.h $(PROGRAMS) *.gcno *.gcda random.dcm

check: tests/zz1 tests/zzw tests/zzt
	tests/zz1 2> /dev/null
	tests/zzw
	tests/zzt samples/spine.dcm
	./zzdump --version > /dev/null
	./zzdump --help > /dev/null
	./zzdump --usage > /dev/null
	./zzdump -v samples/confuse.dcm > /dev/null
	./zzdump samples/tw1.dcm > /dev/null
	./zzdump samples/tw2.dcm > /dev/null
	./zzdump samples/brokensq.dcm > /dev/null
	./zzanon TEST samples/tw1.dcm
	./zzanon TEST samples/tw2.dcm
	./zznifti2dcm samples/minimal.hdr samples/minimal.img
	./zznifti2dcm samples/minimal.nii

tests/zz1: tests/zz1.c $(HEADERS) $(COMMON)
	$(CC) -o $@ $< $(COMMON) -I. $(CFLAGS)

tests/zzw: tests/zzw.c $(HEADERS) $(COMMON) $(COMMONWRITE) $(COMMONVERIFY)
	$(CC) -o $@ $< $(COMMON) -I. $(CFLAGS) $(COMMONWRITE) $(COMMONVERIFY)

tests/zzt: tests/zzt.c $(HEADERS) $(COMMON) $(COMMONTEXTURE)
	$(CC) -o $@ $< $(COMMON) -I. $(CFLAGS) $(COMMONTEXTURE) -lGLEW -lglut

install:
	install -t /usr/local/bin $(PROGRAMS)
