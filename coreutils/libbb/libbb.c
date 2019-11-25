#include <common.h>
#include "libbb.h"

const char * const full_version = "multi-call binary";
const char * const name_too_long = "file name too long";
const char * const omitting_directory = "%s: omitting directory";
const char * const not_a_directory = "%s: not a directory";
const char * const memory_exhausted = "memory exhausted";
const char * const invalid_date = "invalid date `%s'";
const char * const invalid_option = "invalid option -- %c";
const char * const io_error = "%s: input/output error -- %s";
const char * const dash_dash_help = "--help";
const char * const write_error = "Write Error";
const char * const too_few_args = "too few arguments";
const char * const name_longer_than_foo = "Names longer than %d chars not supported.";
const char * const unknown = "(unknown)";
const char * const can_not_create_raw_socket = "can`t create raw socket";

extern char *concat_path_file(const char *path, const char *filename)
{
	char *outbuf;
	char *lc;

	if (!path)
	    path="";
	lc = last_char_is(path, '/');
	while (*filename == '/')
		filename++;
	outbuf = xmalloc(strlen(path)+strlen(filename)+1+(lc==NULL));
	sprintf(outbuf, "%s%s%s", path, (lc==NULL)? "/" : "", filename);

	return outbuf;
}

int is_directory(const char *fileName, const int followLinks, struct stat *statBuf)
{
	int status;
	int didMalloc = 0;

	if (statBuf == NULL) {
	    statBuf = (struct stat *)xmalloc(sizeof(struct stat));
	    ++didMalloc;
	}

	if (followLinks == TRUE)
		status = stat(fileName, statBuf);
	else
		status = lstat(fileName, statBuf);

	if (status < 0 || !(S_ISDIR(statBuf->st_mode))) {
	    status = FALSE;
	}
	else status = TRUE;

	if (didMalloc) {
	    free(statBuf);
	    statBuf = NULL;
	}
	return status;
}

const char *make_human_readable_str(unsigned long size, 
									unsigned long block_size,
									unsigned long display_unit)
{
	/* The code will adjust for additional (appended) units. */
	static const char zero_and_units[] = { '0', 0, 'k', 'M', 'G', 'T' };
	static const char fmt[] = "%Lu";
	static const char fmt_tenths[] = "%Lu.%d%c";

	static char str[21];		/* Sufficient for 64 bit unsigned integers. */
	
	unsigned long long val;
	int frac;
	const char *u;
	const char *f;

	u = zero_and_units;
	f = fmt;
	frac = 0;

	val = ((unsigned long long) size) * block_size;
	if (val == 0) {
		return u;
	}

	if (display_unit) {
		val += display_unit/2;	/* Deal with rounding. */
		val /= display_unit;	/* Don't combine with the line above!!! */
	} else {
		++u;
		while ((val >= KILOBYTE)
			   && (u < zero_and_units + sizeof(zero_and_units) - 1)) {
			f = fmt_tenths;
			++u;
			frac = ((((int)(val % KILOBYTE)) * 10) + (KILOBYTE/2)) / KILOBYTE;
			val /= KILOBYTE;
		}
		if (frac >= 10) {		/* We need to round up here. */
			++val;
			frac = 0;
		}
#if 0
		/* Sample code to omit decimal point and tenths digit. */
		if ( /* no_tenths */ 1 ) {
			if ( frac >= 5 ) {
				++val;
			}
			f = "%Lu%*c" /* fmt_no_tenths */ ;
			frac = 1;
		}
#endif
	}

	/* If f==fmt then 'frac' and 'u' are ignored. */
	snprintf(str, sizeof(str), f, val, frac, *u);

	return str;
}

extern void perror_msg(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	vperror_msg(s, p);
	va_end(p);
}

char * last_char_is(const char *s, int c)
{
	char *sret;
	if (!s)
	    return NULL;
	sret  = (char *)s+strlen(s)-1;
	if (sret>=s && *sret == c) { 
		return sret;
	} else {
		return NULL;
	}
}

extern void vperror_msg(const char *s, va_list p)
{
	int err=errno;
	if(s == 0) s = "";
	verror_msg(s, p);
	if (*s) s = ": ";
	fprintf(stderr, "%s%s\n", s, strerror(err));
}

#ifndef DMALLOC
extern void *xmalloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL && size != 0)
		error_msg_and_die(memory_exhausted);
	return ptr;
}

extern void *xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL && size != 0)
		error_msg_and_die(memory_exhausted);
	return ptr;
}

extern void *xcalloc(size_t nmemb, size_t size)
{
	void *ptr = calloc(nmemb, size);
	if (ptr == NULL && nmemb != 0 && size != 0)
		error_msg_and_die(memory_exhausted);
	return ptr;
}

extern char * xstrdup (const char *s) {
	char *t;

	if (s == NULL)
		return NULL;

	t = strdup (s);

	if (t == NULL)
		error_msg_and_die(memory_exhausted);

	return t;
}
#endif

extern char * xstrndup (const char *s, int n) {
	char *t;

	if (s == NULL)
		error_msg_and_die("xstrndup bug");

	t = xmalloc(++n);
	
	return safe_strncpy(t,s,n);
}

FILE *xfopen(const char *path, const char *mode)
{
	FILE *fp;
	if ((fp = fopen(path, mode)) == NULL)
		perror_msg_and_die("%s", path);
	return fp;
}

/* Stupid gcc always includes its own builtin strlen()... */
#undef strlen
size_t xstrlen(const char *string)
{
	return(strlen(string));
}

extern void error_msg_and_die(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	verror_msg(s, p);
	va_end(p);
	putc('\n', stderr);
	exit(EXIT_FAILURE);
}

extern void perror_msg_and_die(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	vperror_msg(s, p);
	va_end(p);
	exit(EXIT_FAILURE);
}

extern char * safe_strncpy(char *dst, const char *src, size_t size)
{   
	dst[size-1] = '\0';
	return strncpy(dst, src, size-1);   
}

extern void verror_msg(const char *s, va_list p)
{
	fflush(stdout);
	fprintf(stderr, "%s: ", applet_name);
	vfprintf(stderr, s, p);
}

void chomp(char *s)
{
	char *lc = last_char_is(s, '\n');
	
	if(lc)
		*lc = 0;
}

extern char *get_line_from_file(FILE *file)
{
	static const int GROWBY = 80; /* how large we will grow strings by */

	int ch;
	int idx = 0;
	char *linebuf = NULL;
	int linebufsz = 0;

	while (1) {
		ch = fgetc(file);
		if (ch == EOF)
			break;
		/* grow the line buffer as necessary */
		while (idx > linebufsz-2)
			linebuf = xrealloc(linebuf, linebufsz += GROWBY);
		linebuf[idx++] = (char)ch;
		if (ch == '\n' || ch == '\0')
			break;
	}

	if (idx == 0)
		return NULL;

	linebuf[idx] = 0;
	return linebuf;
}


int ask_confirmation()
{
	int c = '\0';
	int ret = 0;

	while (c != '\n') {
		c = getchar();
		if ( c != '\n' ) {
			ret = ((c=='y')||(c=='Y')) ? 1 : 0;
		}
	}
	return ret;
}


int copy_file(const char *source, const char *dest, int flags)
{
	struct stat source_stat;
	struct stat dest_stat;
	int dest_exists = 1;
	int status = 0;

	if (((flags & FILEUTILS_PRESERVE_SYMLINKS) &&
			lstat(source, &source_stat) < 0) ||
			(!(flags & FILEUTILS_PRESERVE_SYMLINKS) &&
			 stat(source, &source_stat) < 0)) {
		perror_msg("%s", source);
		return -1;
	}

	if (stat(dest, &dest_stat) < 0) {
		if (errno != ENOENT) {
			perror_msg("unable to stat `%s'", dest);
			return -1;
		}
		dest_exists = 0;
	}

	if (dest_exists && source_stat.st_rdev == dest_stat.st_rdev &&
			source_stat.st_ino == dest_stat.st_ino) {
		error_msg("`%s' and `%s' are the same file", source, dest);
		return -1;
	}

	if (S_ISDIR(source_stat.st_mode)) {
		DIR *dp;
		struct dirent *d;
		mode_t saved_umask = 0;

		if (!(flags & FILEUTILS_RECUR)) {
			error_msg("%s: omitting directory", source);
			return -1;
		}

		/* Create DEST.  */
		if (dest_exists) {
			if (!S_ISDIR(dest_stat.st_mode)) {
				error_msg("`%s' is not a directory", dest);
				return -1;
			}
		} else {
			mode_t mode;
			saved_umask = umask(0);

			mode = source_stat.st_mode;
			if (!(flags & FILEUTILS_PRESERVE_STATUS))
				mode = source_stat.st_mode & ~saved_umask;
			mode |= S_IRWXU;

			if (mkdir(dest, mode) < 0) {
				umask(saved_umask);
				perror_msg("cannot create directory `%s'", dest);
				return -1;
			}

			umask(saved_umask);
		}

		/* Recursively copy files in SOURCE.  */
		if ((dp = opendir(source)) == NULL) {
			perror_msg("unable to open directory `%s'", source);
			status = -1;
			goto end;
		}

		while ((d = readdir(dp)) != NULL) {
			char *new_source, *new_dest;

			if (strcmp(d->d_name, ".") == 0 ||
					strcmp(d->d_name, "..") == 0)
				continue;

			new_source = concat_path_file(source, d->d_name);
			new_dest = concat_path_file(dest, d->d_name);
			if (copy_file(new_source, new_dest, flags) < 0)
				status = -1;
			free(new_source);
			free(new_dest);
		}

		/* ??? What if an error occurs in readdir?  */

		if (closedir(dp) < 0) {
			perror_msg("unable to close directory `%s'", source);
			status = -1;
		}

		if (!dest_exists &&
				chmod(dest, source_stat.st_mode & ~saved_umask) < 0) {
			perror_msg("unable to change permissions of `%s'", dest);
			status = -1;
		}
	} else if (S_ISREG(source_stat.st_mode)) {
		FILE *sfp, *dfp;

		if (dest_exists) {
			if (flags & FILEUTILS_INTERACTIVE) {
				fprintf(stderr, "%s: overwrite `%s'? ", applet_name, dest);
				if (!ask_confirmation())
					return 0;
			}

			if ((dfp = fopen(dest, "w")) == NULL) {
				if (!(flags & FILEUTILS_FORCE)) {
					perror_msg("unable to open `%s'", dest);
					return -1;
				}

				if (unlink(dest) < 0) {
					perror_msg("unable to remove `%s'", dest);
					return -1;
				}

				dest_exists = 0;
			}
		}

		if (!dest_exists) {
			int fd;

			if ((fd = open(dest, O_WRONLY|O_CREAT, source_stat.st_mode)) < 0 ||
					(dfp = fdopen(fd, "w")) == NULL) {
				if (fd >= 0)
					close(fd);
				perror_msg("unable to open `%s'", dest);
				return -1;
			}
		}

		if ((sfp = fopen(source, "r")) == NULL) {
			fclose(dfp);
			perror_msg("unable to open `%s'", source);
			status = -1;
			goto end;
		}

		if (copy_file_chunk(sfp, dfp, -1) < 0)
			status = -1;

		if (fclose(dfp) < 0) {
			perror_msg("unable to close `%s'", dest);
			status = -1;
		}

		if (fclose(sfp) < 0) {
			perror_msg("unable to close `%s'", source);
			status = -1;
		}
	} else if (S_ISBLK(source_stat.st_mode) || S_ISCHR(source_stat.st_mode) ||
			S_ISSOCK(source_stat.st_mode)) {
		if (mknod(dest, source_stat.st_mode, source_stat.st_rdev) < 0) {
			perror_msg("unable to create `%s'", dest);
			return -1;
		}
	} else if (S_ISFIFO(source_stat.st_mode)) {
		if (mkfifo(dest, source_stat.st_mode) < 0) {
			perror_msg("cannot create fifo `%s'", dest);
			return -1;
		}
	} else if (S_ISLNK(source_stat.st_mode)) {
		char *lpath = xreadlink(source);
		if (symlink(lpath, dest) < 0) {
			perror_msg("cannot create symlink `%s'", dest);
			return -1;
		}
		free(lpath);

#if (__GLIBC__ >= 2) && (__GLIBC_MINOR__ >= 1)
		if (flags & FILEUTILS_PRESERVE_STATUS)
			if (lchown(dest, source_stat.st_uid, source_stat.st_gid) < 0)
				perror_msg("unable to preserve ownership of `%s'", dest);
#endif
		return 0;
	} else {
		error_msg("internal error: unrecognized file type");
		return -1;
	}

end:

	if (flags & FILEUTILS_PRESERVE_STATUS) {
		struct utimbuf times;

		times.actime = source_stat.st_atime;
		times.modtime = source_stat.st_mtime;
		if (utime(dest, &times) < 0)
			perror_msg("unable to preserve times of `%s'", dest);
		if (chown(dest, source_stat.st_uid, source_stat.st_gid) < 0) {
			source_stat.st_mode &= ~(S_ISUID | S_ISGID);
			perror_msg("unable to preserve ownership of `%s'", dest);
		}
		if (chmod(dest, source_stat.st_mode) < 0)
			perror_msg("unable to preserve permissions of `%s'", dest);
	}

	return status;
}


extern void error_msg(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	verror_msg(s, p);
	va_end(p);
	putc('\n', stderr);
}

int full_write(int fd, const char *buf, int len)
{
	int cc;
	int total;

	total = 0;

	while (len > 0) {
		cc = write(fd, buf, len);

		if (cc < 0)
			return -1;

		buf += cc;
		total += cc;
		len -= cc;
	}

	return total;
}

char *get_last_path_component(char *path)
{
#if EMULATE_BASENAME
	static const char null_or_empty[] = ".";
#endif
	char *first = path;
	char *last;

#if EMULATE_BASENAME
	if (!path || !*path) {
		return (char *) null_or_empty;
	}
#endif

	last = path - 1;

	while (*path) {
		if ((*path != '/') && (path > ++last)) {
			last = first = path;
		}
		++path;
	}

	if (*first == '/') {
		last = first;
	}
	last[1] = 0;

	return first;
}

extern int copy_file_chunk(FILE *src_file, FILE *dst_file, unsigned long long chunksize)
{
	size_t nread, nwritten, size;
	char buffer[BUFSIZ];

	while (chunksize != 0) {
		if (chunksize > BUFSIZ)
			size = BUFSIZ;
		else
			size = chunksize;

		nread = fread (buffer, 1, size, src_file);

		if (nread != size && ferror (src_file)) {
			perror_msg ("read");
			return -1;
		} else if (nread == 0) {
			if (chunksize != -1) {
				error_msg ("Unable to read all data");
				return -1;
			}

			return 0;
		}

		nwritten = fwrite (buffer, 1, nread, dst_file);

		if (nwritten != nread) {
			if (ferror (dst_file))
				perror_msg ("write");
			else
				error_msg ("Unable to write all data");
			return -1;
		}

		if (chunksize != -1)
			chunksize -= nwritten;
	}

	return 0;
}

int make_directory (char *path, long mode, int flags)
{
	if (!(flags & FILEUTILS_RECUR)) {
		if (mkdir (path, 0777) < 0) {
			perror_msg ("Cannot create directory `%s'", path);
			return -1;
		}

		if (mode != -1 && chmod (path, mode) < 0) {
			perror_msg ("Cannot set permissions of directory `%s'", path);
			return -1;
		}
	} else {
		struct stat st;

		if (stat (path, &st) < 0 && errno == ENOENT) {
			int status;
			char *buf, *parent;
			mode_t mask;

			mask = umask (0);
			umask (mask);

			buf = xstrdup (path);
			parent = dirname (buf);
			status = make_directory (parent, (0777 & ~mask) | 0300,
					FILEUTILS_RECUR);
			free (buf);

			if (status < 0 || make_directory (path, mode, 0) < 0)
				return -1;
		}
	}

	return 0;
}

#define TYPEINDEX(mode) (((mode) >> 12) & 0x0f)
#define TYPECHAR(mode)  ("0pcCd?bB-?l?s???" [TYPEINDEX(mode)])

/* The special bits. If set, display SMODE0/1 instead of MODE0/1 */
static const mode_t SBIT[] = {
	0, 0, S_ISUID,
	0, 0, S_ISGID,
	0, 0, S_ISVTX
};

/* The 9 mode bits to test */
static const mode_t MBIT[] = {
	S_IRUSR, S_IWUSR, S_IXUSR,
	S_IRGRP, S_IWGRP, S_IXGRP,
	S_IROTH, S_IWOTH, S_IXOTH
};

static const char MODE1[]  = "rwxrwxrwx";
static const char MODE0[]  = "---------";
static const char SMODE1[] = "..s..s..t";
static const char SMODE0[] = "..S..S..T";

/*
 * Return the standard ls-like mode string from a file mode.
 * This is static and so is overwritten on each call.
 */
const char *mode_string(int mode)
{
	static char buf[12];

	int i;

	buf[0] = TYPECHAR(mode);
	for (i = 0; i < 9; i++) {
		if (mode & SBIT[i])
			buf[i + 1] = (mode & MBIT[i]) ? SMODE1[i] : SMODE0[i];
		else
			buf[i + 1] = (mode & MBIT[i]) ? MODE1[i] : MODE0[i];
	}
	return buf;
}

void my_getgrgid(char *group, long gid)
{
	struct group *mygroup;

	mygroup  = getgrgid(gid);
	if (mygroup==NULL)
		sprintf(group, "%-8ld ", (long)gid);
	else
		strcpy(group, mygroup->gr_name);
}

long my_getgrnam(const char *name)
{
	struct group *mygroup;

	mygroup  = getgrnam(name);
	if (mygroup==NULL)
		error_msg_and_die("unknown group name: %s", name);

	return (mygroup->gr_gid);
}

long my_getpwnam(const char *name)
{
	struct passwd *myuser;

	myuser  = getpwnam(name);
	if (myuser==NULL)
		error_msg_and_die("unknown user name: %s", name);

	return myuser->pw_uid;
}

long my_getpwnamegid(const char *name)
{
	struct group *mygroup;
	struct passwd *myuser;

	myuser=getpwnam(name);
	if (myuser==NULL)
		error_msg_and_die("unknown user name: %s", name);

	mygroup  = getgrgid(myuser->pw_gid);
	if (mygroup==NULL)
		error_msg_and_die("unknown gid %ld", (long)myuser->pw_gid);

	return mygroup->gr_gid;
}

void my_getpwuid(char *name, long uid)
{
	struct passwd *myuser;

	myuser  = getpwuid(uid);
	if (myuser==NULL)
		sprintf(name, "%-8ld ", (long)uid);
	else
		strcpy(name, myuser->pw_name);
}

extern int parse_mode(const char *s, mode_t * theMode)
{
	static const mode_t group_set[] = { 
		S_ISUID | S_IRWXU,		/* u */
		S_ISGID | S_IRWXG,		/* g */
		S_IRWXO,				/* o */
		S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO /* a */
	};

	static const mode_t mode_set[] = {
		S_IRUSR | S_IRGRP | S_IROTH, /* r */
		S_IWUSR | S_IWGRP | S_IWOTH, /* w */
		S_IXUSR | S_IXGRP | S_IXOTH, /* x */
		S_ISUID | S_ISGID,		/* s */
		S_ISVTX					/* t */
	};

	static const char group_chars[] = "ugoa";
	static const char mode_chars[] = "rwxst";

	const char *p;

	mode_t andMode =
		S_ISVTX | S_ISUID | S_ISGID | S_IRWXU | S_IRWXG | S_IRWXO;
	mode_t orMode = 0;
	mode_t mode;
	mode_t groups;
	char type;
	char c;

	if (s==NULL) {
		return (FALSE);
	}

	do {
		mode = 0;
		groups = 0;
	NEXT_GROUP:
		if ((c = *s++) == '\0') {
			return -1;
		}
		for (p=group_chars ; *p ; p++) {
			if (*p == c) {
				groups |= group_set[(int)(p-group_chars)];
				goto NEXT_GROUP;
			}
		}
		switch (c) {
			case '=':
			case '+':
			case '-':
				type = c;
				if (groups == 0) { /* The default is "all" */
					groups |= S_ISUID | S_ISGID | S_ISVTX
							| S_IRWXU | S_IRWXG | S_IRWXO;
				}
				break;
			default:
				if ((c < '0') || (c > '7') || (mode | groups)) {
					return (FALSE);
				} else {
					*theMode = strtol(--s, NULL, 8);
					return (TRUE);
				}
		}

	NEXT_MODE:
		if (((c = *s++) != '\0') && (c != ',')) {
			for (p=mode_chars ; *p ; p++) {
				if (*p == c) {
					mode |= mode_set[(int)(p-mode_chars)];
					goto NEXT_MODE;
				}
			}
			break;				/* We're done so break out of loop.*/
		}
		switch (type) {
			case '=':
				andMode &= ~(groups); /* Now fall through. */
			case '+':
				orMode |= mode & groups;
				break;
			case '-':
				andMode &= ~(mode & groups);
				orMode &= ~(mode & groups);
				break;
		}
	} while (c == ',');

	*theMode &= andMode;
	*theMode |= orMode;

	return TRUE;
}

unsigned long parse_number(const char *numstr,
		const struct suffix_mult *suffixes)
{
	const struct suffix_mult *sm;
	unsigned long int ret;
	int len;
	char *end;
	
	ret = strtoul(numstr, &end, 10);
	if (numstr == end)
		error_msg_and_die("invalid number `%s'", numstr);
	while (end[0] != '\0') {
		sm = suffixes;
		while ( sm != 0 ) {
			if(sm->suffix) {
				len = strlen(sm->suffix);
				if (strncmp(sm->suffix, end, len) == 0) {
					ret *= sm->mult;
					end += len;
					break;
				}
			sm++;
			
			} else
				sm = 0;
		}
		if (sm == 0)
			error_msg_and_die("invalid number `%s'", numstr);
	}
	return ret;
}

extern void print_file(FILE *file)
{
	fflush(stdout);
	copyfd(fileno(file), fileno(stdout));
	fclose(file);
}

extern int print_file_by_name(char *filename)
{
	struct stat statBuf;
	int status = TRUE;

	if(is_directory(filename, TRUE, &statBuf)==TRUE) {
		error_msg("%s: Is directory", filename);
		status = FALSE;
	} else {
		FILE *f = wfopen(filename, "r");
		if(f!=NULL)
			print_file(f);
		else
			status = FALSE;
	}

	return status;
}

char process_escape_sequence(const char **ptr)
{
	static const char charmap[] = {
		'a',  'b',  'f',  'n',  'r',  't',  'v',  '\\', 0,
		'\a', '\b', '\f', '\n', '\r', '\t', '\v', '\\', '\\' };

	const char *p;
	const char *q;
	int num_digits;
	unsigned int n;
	
	n = 0;
	q = *ptr;

	for ( num_digits = 0 ; num_digits < 3 ; ++num_digits) {
		if ((*q < '0') || (*q > '7')) { /* not a digit? */
			break;
		}
		n = n * 8 + (*q++ - '0');
	}

	if (num_digits == 0) {	/* mnemonic escape sequence? */
		for (p=charmap ; *p ; p++) {
			if (*p == *q) {
				q++;
				break;
			}
		}
		n = *(p+(sizeof(charmap)/2));
	}

	   /* doesn't hurt to fall through to here from mnemonic case */
	if (n > UCHAR_MAX) {	/* is octal code too big for a char? */
		n /= 8;			/* adjust value and */
		--q;				/* back up one char */
	}

	*ptr = q;
	return (char) n;
}

int recursive_action(const char *fileName,
					int recurse, int followLinks, int depthFirst,
					int (*fileAction) (const char *fileName,
									   struct stat * statbuf,
									   void* userData),
					int (*dirAction) (const char *fileName,
									  struct stat * statbuf,
									  void* userData),
					void* userData)
{
	int status;
	struct stat statbuf;
	struct dirent *next;

	if (followLinks == TRUE)
		status = stat(fileName, &statbuf);
	else
		status = lstat(fileName, &statbuf);

	if (status < 0) {
#ifdef DEBUG_RECURS_ACTION
		fprintf(stderr,
				"status=%d followLinks=%d TRUE=%d\n",
				status, followLinks, TRUE);
#endif
		perror_msg("%s", fileName);
		return FALSE;
	}

	if ((followLinks == FALSE) && (S_ISLNK(statbuf.st_mode))) {
		if (fileAction == NULL)
			return TRUE;
		else
			return fileAction(fileName, &statbuf, userData);
	}

	if (recurse == FALSE) {
		if (S_ISDIR(statbuf.st_mode)) {
			if (dirAction != NULL)
				return (dirAction(fileName, &statbuf, userData));
			else
				return TRUE;
		}
	}

	if (S_ISDIR(statbuf.st_mode)) {
		DIR *dir;

		if (dirAction != NULL && depthFirst == FALSE) {
			status = dirAction(fileName, &statbuf, userData);
			if (status == FALSE) {
				perror_msg("%s", fileName);
				return FALSE;
			} else if (status == SKIP)
				return TRUE;
		}
		dir = opendir(fileName);
		if (!dir) {
			perror_msg("%s", fileName);
			return FALSE;
		}
		status = TRUE;
		while ((next = readdir(dir)) != NULL) {
			char *nextFile;

			if ((strcmp(next->d_name, "..") == 0)
					|| (strcmp(next->d_name, ".") == 0)) {
				continue;
			}
			nextFile = concat_path_file(fileName, next->d_name);
			if (recursive_action(nextFile, TRUE, followLinks, depthFirst,
						fileAction, dirAction, userData) == FALSE) {
				status = FALSE;
			}
			free(nextFile);
		}
		closedir(dir);
		if (dirAction != NULL && depthFirst == TRUE) {
			if (dirAction(fileName, &statbuf, userData) == FALSE) {
				perror_msg("%s", fileName);
				return FALSE;
			}
		}
		if (status == FALSE)
			return FALSE;
	} else {
		if (fileAction == NULL)
			return TRUE;
		else
			return fileAction(fileName, &statbuf, userData);
	}
	return TRUE;
}

struct hostent *xgethostbyname(const char *name)
{
	struct hostent *retval;

	if ((retval = gethostbyname(name)) == NULL)
		herror_msg_and_die("%s", name);

	return retval;
}

#define PATH_INCR 32

char *xgetcwd (char *cwd)
{
  char *ret;
  unsigned path_max;

  errno = 0;
  path_max = (unsigned) PATH_MAX;
  path_max += 2;                /* The getcwd docs say to do this. */

  if(cwd==0)
	cwd = xmalloc (path_max);

  errno = 0;
  while ((ret = getcwd (cwd, path_max)) == NULL && errno == ERANGE) {
      path_max += PATH_INCR;
      cwd = xrealloc (cwd, path_max);
      errno = 0;
  }

  if (ret == NULL) {
      int save_errno = errno;
      free (cwd);
      errno = save_errno;
      perror_msg("getcwd()");
      return NULL;
  }

  return cwd;
}

FILE *wfopen(const char *path, const char *mode)
{
	FILE *fp;
	if ((fp = fopen(path, mode)) == NULL) {
		perror_msg("%s", path);
		errno = 0;
	}
	return fp;
}

ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

extern int remove_file(const char *path, int flags)
{
	struct stat path_stat;
	int path_exists = 1;

	if (lstat(path, &path_stat) < 0) {
		if (errno != ENOENT) {
			perror_msg("unable to stat `%s'", path);
			return -1;
		}

		path_exists = 0;
	}

	if (!path_exists) {
		if (!(flags & FILEUTILS_FORCE)) {
			perror_msg("cannot remove `%s'", path);
			return -1;
		}
		return 0;
	}

	if (S_ISDIR(path_stat.st_mode)) {
		DIR *dp;
		struct dirent *d;
		int status = 0;

		if (!(flags & FILEUTILS_RECUR)) {
			error_msg("%s: is a directory", path);
			return -1;
		}

		if ((!(flags & FILEUTILS_FORCE) && access(path, W_OK) < 0 &&
					isatty(0)) ||
				(flags & FILEUTILS_INTERACTIVE)) {
			fprintf(stderr, "%s: descend into directory `%s'? ", applet_name,
					path);
			if (!ask_confirmation())
				return 0;
		}

		if ((dp = opendir(path)) == NULL) {
			perror_msg("unable to open `%s'", path);
			return -1;
		}

		while ((d = readdir(dp)) != NULL) {
			char *new_path;

			if (strcmp(d->d_name, ".") == 0 ||
					strcmp(d->d_name, "..") == 0)
				continue;

			new_path = concat_path_file(path, d->d_name);
			if (remove_file(new_path, flags) < 0)
				status = -1;
			free(new_path);
		}

		if (closedir(dp) < 0) {
			perror_msg("unable to close `%s'", path);
			return -1;
		}

		if (flags & FILEUTILS_INTERACTIVE) {
			fprintf(stderr, "%s: remove directory `%s'? ", applet_name, path);
			if (!ask_confirmation())
				return status;
		}

		if (rmdir(path) < 0) {
			perror_msg("unable to remove `%s'", path);
			return -1;
		}

		return status;
	} else {
		if ((!(flags & FILEUTILS_FORCE) && access(path, W_OK) < 0 &&
					!S_ISLNK(path_stat.st_mode) &&
					isatty(0)) ||
				(flags & FILEUTILS_INTERACTIVE)) {
			fprintf(stderr, "%s: remove `%s'? ", applet_name, path);
			if (!ask_confirmation())
				return 0;
		}

		if (unlink(path) < 0) {
			perror_msg("unable to remove `%s'", path);
			return -1;
		}

		return 0;
	}
}

extern int copyfd(int fd1, int fd2)
{
	char buf[8192];
	ssize_t nread, nwrote;

	while (1) {
		nread = safe_read(fd1, buf, sizeof(buf));
		if (nread == 0)
			break;
		if (nread == -1) {
			perror_msg("read");
			return -1;
		}

		nwrote = full_write(fd2, buf, nread);
		if (nwrote == -1) {
			perror_msg("write");
			return -1;
		}
	}

	return 0;
}

extern void herror_msg_and_die(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	vherror_msg(s, p);
	va_end(p);
	exit(EXIT_FAILURE);
}

extern char *xreadlink(const char *path)
{                       
	static const int GROWBY = 80; /* how large we will grow strings by */

	char *buf = NULL;   
	int bufsize = 0, readsize = 0;

	do {
		buf = xrealloc(buf, bufsize += GROWBY);
		readsize = readlink(path, buf, bufsize); /* 1st try */
		if (readsize == -1) {
		    perror_msg("%s:%s", applet_name, path);
		    return NULL;
		}
	}           
	while (bufsize < readsize + 1);

	buf[readsize] = '\0';

	return buf;
}       

extern void vherror_msg(const char *s, va_list p)
{
	if(s == 0)
		s = "";
	verror_msg(s, p);
	if (*s)
		fputs(": ", stderr);
	herror("");
}

