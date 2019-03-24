/***********************************************************************
 * log.c: Handles the system calls that create files and logs them.
 ***********************************************************************
 * This file is part of the package porg
 * Copyright (C) 2015 David Ricart
 * For more information visit http://porg.sourceforge.net
 ***********************************************************************/

#include "config.h"
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>			  
#include <stdarg.h>
#include <unistd.h>

#ifndef RTLD_NEXT
#	define RTLD_NEXT ((void *) -1l)
#endif

#define HAVE_64_FUNCS (HAVE_OPEN64 && HAVE_CREAT64 && HAVE_FOPEN64 && HAVE_FREOPEN64)
#define HAVE_AT_FUNCS (HAVE_OPENAT && HAVE_LINKAT && HAVE_SYMLINKAT && HAVE_RENAMEAT)

#define PORG_BUFSIZE  4096

static int	(*libc_open)		(const char*, int, ...);
static int	(*libc_creat)		(const char*, mode_t);
static int	(*libc_rename)		(const char*, const char*);
static int	(*libc_link)		(const char*, const char*);
static int	(*libc_symlink)		(const char*, const char*);
static FILE*(*libc_fopen)		(const char*, const char*);
static FILE*(*libc_freopen)		(const char*, const char*, FILE*);

#if HAVE_AT_FUNCS
static int	(*libc_openat)		(int, const char*, int, ...);
static int	(*libc_renameat)	(int, const char*, int, const char*);
static int	(*libc_linkat)		(int, const char*, int, const char*, int);
static int	(*libc_symlinkat)	(const char*, int, const char*);
#endif

#if HAVE_64_FUNCS
static int	(*libc_open64)		(const char*, int, ...);
static int	(*libc_creat64)		(const char*, mode_t);
static FILE*(*libc_fopen64)		(const char*, const char*);
static FILE*(*libc_freopen64)	(const char*, const char*, FILE*);
#endif 

#if HAVE_OPENAT64
static int	(*libc_openat64)	(int, const char*, int, ...);
#endif

static char* porg_tmpfile;
static char* porg_debug;


/* Fake declarations of libc's internal __open and __open64 */
#if !HAVE_DECL___OPEN
int __open(const char*, int, ...);
#endif
#if !HAVE_DECL___OPEN64
int __open64(const char*, int, ...);
#endif


static void porg_vprintf(const char* fmt, va_list ap)
{
	if (porg_debug) {
		fflush(stdout);
		fputs("porg-log :: ", stderr);
		vfprintf(stderr, fmt, ap);
		fputs("\n", stderr);
	}
}


static void porg_die(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	porg_vprintf(fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


/*
 * Get the absolute path, referring relative paths to the CWD, or to directory
 * referred to by file descriptor fd, if non negative.
 */
static void porg_get_absolute_path(int fd, const char* path, char* abs_path)
{
	static char cwd[PORG_BUFSIZE], aux[PORG_BUFSIZE];
	int old_errno = errno;

	/* already absolute (or can't get CWD) */
	if (path[0] == '/' || !getcwd(cwd, PORG_BUFSIZE))
		strncpy(abs_path, path, PORG_BUFSIZE - 1);

	/* relative to CWD */
	else if (fd < 0) {
		strncpy(abs_path, cwd, PORG_BUFSIZE - 1);
		strncat(abs_path, "/", PORG_BUFSIZE - strlen(abs_path) - 1);
		strncat(abs_path, path, PORG_BUFSIZE - strlen(abs_path) - 1);
	}
	/* relative to directory fd */
	else if (fchdir(fd) == 0 && getcwd(aux, PORG_BUFSIZE) && chdir(cwd) == 0) {
		strncpy(abs_path, aux, PORG_BUFSIZE - 1);
		strncat(abs_path, "/", PORG_BUFSIZE - strlen(abs_path) - 1);
		strncat(abs_path, path, PORG_BUFSIZE - strlen(abs_path) - 1);
	}
	else
		strncpy(abs_path, path, PORG_BUFSIZE - 1);

	abs_path[PORG_BUFSIZE - 1] = 0;

	errno = old_errno;
}


static void* porg_dlsym(const char* symbol)
{
	void* ret;
	char* err;

	dlerror();

	if (!(ret = dlsym(RTLD_NEXT, symbol))) {
		err = (char*)dlerror();
		porg_die("dlsym(RTLD_NEXT, \"%s\"): %s", symbol, err ? err : "failed");
	}

	return ret;
}


static void porg_init()
{
	if (porg_tmpfile) /* already init'ed */
		return;

	/* read the environment */
	
	if (!(porg_tmpfile = getenv("PORG_TMPFILE")))
		porg_die("variable PORG_TMPFILE undefined");
		
	porg_debug = getenv("PORG_DEBUG");
	
	/* handle system calls */
	
	libc_open 		= porg_dlsym("open");
	libc_creat 		= porg_dlsym("creat");
	libc_rename 	= porg_dlsym("rename");
	libc_link 		= porg_dlsym("link");
	libc_symlink 	= porg_dlsym("symlink");
	libc_fopen 		= porg_dlsym("fopen");
	libc_freopen 	= porg_dlsym("freopen");

#if HAVE_64_FUNCS
	libc_open64 	= porg_dlsym("open64");
	libc_creat64 	= porg_dlsym("creat64");
	libc_fopen64 	= porg_dlsym("fopen64");
	libc_freopen64 	= porg_dlsym("freopen64");
#endif

#if HAVE_AT_FUNCS
	libc_openat 	= porg_dlsym("openat");
	libc_renameat	= porg_dlsym("renameat");
	libc_linkat		= porg_dlsym("linkat");
	libc_symlinkat	= porg_dlsym("symlinkat");
#endif

#if HAVE_OPENAT64
	libc_openat64 	= porg_dlsym("openat64");
#endif
}


/*
 * Log a filename to the tmp file, and print a debug message to stderr if 
 * debugging is enabled.
 */
static void porg_log(const char* path, const char* fmt, ...)
{
	static char abs_path[PORG_BUFSIZE];
	va_list a;
	int fd, len, old_errno = errno;
	
	if (!strncmp(path, "/dev/", 5) || !strncmp(path, "/proc/", 6))
		return;

	porg_init();

	if (porg_debug) {
		va_start(a, fmt);
		porg_vprintf(fmt, a);
		va_end(a);
	}

	/* write path to tmp file to be read by porg */

	if ((fd = libc_open(porg_tmpfile, O_WRONLY | O_CREAT | O_APPEND, 0644)) < 0)
		porg_die("open(\"%s\"): %s", porg_tmpfile, strerror(errno));
	
	porg_get_absolute_path(-1, path, abs_path);
	strncat(abs_path, "\n", PORG_BUFSIZE - strlen(abs_path) - 1);
	len = strlen(abs_path);
	
	if (write(fd, abs_path, len) != len)
		porg_die("%s: write(): %s", porg_tmpfile, strerror(errno));
		
	if (close(fd) < 0)
		porg_die("close(%d): %s", fd, strerror(errno));
	
	errno = old_errno;
}


/* 
 * Handle renaming of directories 
 */
static void porg_log_rename(const char* oldpath, const char* newpath)
{
	char oldbuf[PORG_BUFSIZE], newbuf[PORG_BUFSIZE];
	struct stat st;
	DIR* dir;
	struct dirent* e;
	size_t oldlen, newlen;
	int old_errno = errno;

	/* The newpath file doesn't exist */
	if (lstat(newpath, &st) < 0) 
		goto goto_end;

	else if (!S_ISDIR(st.st_mode)) {
		/* newpath is not a directory, we're done */
		porg_log(newpath, "rename(\"%s\", \"%s\")", oldpath, newpath);
		goto goto_end;
	}

	/* Make sure we have enough space for the following slashes */
	oldlen = strlen(oldpath);
	newlen = strlen(newpath);
	if (oldlen + 3 > PORG_BUFSIZE || newlen + 3 > PORG_BUFSIZE)
		goto goto_end;

	strcpy(oldbuf, oldpath);
	strcpy(newbuf, newpath);
	oldbuf[oldlen] = newbuf[newlen] = '/';
	oldbuf[++oldlen] = newbuf[++newlen] = 0;

	if (!(dir = opendir(newbuf)))
		goto goto_end;

	while ((e = readdir(dir))) {
		if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
			continue;
		strncat(oldbuf, e->d_name, PORG_BUFSIZE - oldlen - 1);
		strncat(newbuf, e->d_name, PORG_BUFSIZE - newlen - 1);
		porg_log_rename(oldbuf, newbuf);
		oldbuf[oldlen] = newbuf[newlen] = 0;
	}

	closedir(dir);

goto_end: 
	errno = old_errno;
}


/************************/
/* System call handlers */
/************************/


int open(const char* path, int flags, ...)
{
	va_list a;
	int mode, accmode, ret;

	/* this fixes a bug when the installer program calls jemalloc 
	   (thanks Masahiro Kasahara) */
	if (!porg_tmpfile && path && !strncmp(path, "/proc/", 6))
		return __open(path, flags);

	porg_init();
	
	va_start(a, flags);
	mode = va_arg(a, int);
	va_end(a);
	
	if ((ret = libc_open(path, flags, mode)) != -1) {
		accmode = flags & O_ACCMODE;
		if (accmode == O_WRONLY || accmode == O_RDWR)
			porg_log(path, "open(\"%s\")", path);
	}

	return ret;
}


int creat(const char* path, mode_t mode)
{
	int ret;
	
	porg_init();
	
	if ((ret = libc_creat(path, mode)) != -1)
		porg_log(path, "creat(\"%s\", 0%o)", path, (int)mode);
	
	return ret;
}


int rename(const char* oldpath, const char* newpath)
{
	int ret;
	
	porg_init();
	
	if ((ret = libc_rename(oldpath, newpath)) != -1)
		porg_log_rename(oldpath, newpath);

	return ret;
}


int link(const char* oldpath, const char* newpath)
{
	int ret;
	
	porg_init();
	
	if ((ret = libc_link(oldpath, newpath)) != -1)
		porg_log(newpath, "link(\"%s\", \"%s\")", oldpath, newpath);
	
	return ret;
}


int symlink(const char* oldpath, const char* newpath)
{
	int ret;
	
	porg_init();
	
	if ((ret = libc_symlink(oldpath, newpath)) != -1)
		porg_log(newpath, "symlink(\"%s\", \"%s\")", oldpath, newpath);
	
	return ret;
}


FILE* fopen(const char* path, const char* mode)
{
	FILE* ret;
	
	porg_init();
	
	if ((ret = libc_fopen(path, mode)) && strpbrk(mode, "wa+"))
		porg_log(path, "fopen(\"%s\", \"%s\")", path, mode);
	
	return ret;
}


FILE* freopen(const char* path, const char* mode, FILE* stream)
{
	FILE* ret;
	
	porg_init();
	
	if ((ret = libc_freopen(path, mode, stream)) && strpbrk(mode, "wa+"))
		porg_log(path, "freopen(\"%s\", \"%s\")", path, mode);
	
	return ret;
}


#if HAVE_64_FUNCS

int open64(const char* path, int flags, ...)
{
	va_list a;
	int mode, accmode, ret;
	
	if (!porg_tmpfile && path && !strncmp(path, "/proc/", 6))
		return __open64(path, flags);

	porg_init();
	
	va_start(a, flags);
	mode = va_arg(a, int);
	va_end(a);
	
	if ((ret = libc_open64(path, flags, mode)) != -1) {
		accmode = flags & O_ACCMODE;
		if (accmode == O_WRONLY || accmode == O_RDWR)
			porg_log(path, "open64(\"%s\")", path);
	}

	return ret;
}


int creat64(const char* path, mode_t mode)
{
	int ret;
	
	porg_init();
	
	if ((ret = libc_creat64(path, mode)) != -1)
		porg_log(path, "creat64(\"%s\", 0%o)", path, mode);
	
	return ret;
}


FILE* fopen64(const char* path, const char* mode)
{
	FILE* ret;
	
	porg_init();
	
	ret = libc_fopen64(path, mode);
	if (ret && strpbrk(mode, "wa+"))
		porg_log(path, "fopen64(\"%s\", \"%s\")", path, mode);
	
	return ret;
}


FILE* freopen64(const char* path, const char* mode, FILE* stream)
{
	FILE* ret;
	
	porg_init();
	
	ret = libc_freopen64(path, mode, stream);
	if (ret && strpbrk(mode, "wa+"))
		porg_log(path, "freopen64(\"%s\", \"%s\")", path, mode);
	
	return ret;
}

#endif /* HAVE_64_FUNCS */


#if HAVE_AT_FUNCS

int openat(int fd, const char* path, int flags, ...)
{
	va_list a;
	int mode, accmode, ret;
	static char abs_path[PORG_BUFSIZE];

	porg_init();
	
	va_start(a, flags);
	mode = va_arg(a, int);
	va_end(a);
	
	if ((ret = libc_openat(fd, path, flags, mode)) != -1) {
		accmode = flags & O_ACCMODE;
		if (accmode == O_WRONLY || accmode == O_RDWR) {
			porg_get_absolute_path(fd, path, abs_path);
			porg_log(abs_path, "openat(%d, \"%s\")", fd, path);
		}
	}

	return ret;
}


int renameat(int oldfd, const char* oldpath, int newfd, const char* newpath)
{
	int ret;
	static char old_abs_path[PORG_BUFSIZE];
	static char new_abs_path[PORG_BUFSIZE];
	
	porg_init();

	if ((ret = libc_renameat(oldfd, oldpath, newfd, newpath)) != -1) {
		porg_get_absolute_path(oldfd, oldpath, old_abs_path);
		porg_get_absolute_path(newfd, newpath, new_abs_path);
		porg_log_rename(old_abs_path, new_abs_path);
	}

	return ret;
}


int linkat(int oldfd, const char* oldpath, 
           int newfd, const char* newpath, int flags)
{
	int ret;
	static char new_abs_path[PORG_BUFSIZE];
	
	porg_init();

	if ((ret = libc_linkat(oldfd, oldpath, newfd, newpath, flags)) != -1) {
		porg_get_absolute_path(newfd, newpath, new_abs_path);
		porg_log(new_abs_path, "linkat(%d, \"%s\", %d, \"%s\")",
			oldfd, oldpath, newfd, newpath);
	}

	return ret;
}


int symlinkat(const char* oldpath, int newfd, const char* newpath)
{
	int ret;
	static char new_abs_path[PORG_BUFSIZE];
	
	porg_init();
	
	if ((ret = libc_symlinkat(oldpath, newfd, newpath)) != -1) {
		porg_get_absolute_path(newfd, newpath, new_abs_path);
		porg_log(new_abs_path, "symlinkat(\"%s\", %d, \"%s\")", 
			oldpath, newfd, newpath);
	}

	return ret;
}

#endif	/* HAVE_AT_FUNCS */


#if HAVE_OPENAT64

int openat64(int fd, const char* path, int flags, ...)
{
	va_list a;
	int mode, accmode, ret;
	static char abs_path[PORG_BUFSIZE];

	porg_init();
	
	va_start(a, flags);
	mode = va_arg(a, int);
	va_end(a);
	
	if ((ret = libc_openat64(fd, path, flags, mode)) != -1) {
		accmode = flags & O_ACCMODE;
		if (accmode == O_WRONLY || accmode == O_RDWR) {
			porg_get_absolute_path(fd, path, abs_path);
			porg_log(abs_path, "openat64(%d, \"%s\")", fd, path);
		}
	}

	return ret;
}

#endif	/* HAVE_OPENAT64 */

