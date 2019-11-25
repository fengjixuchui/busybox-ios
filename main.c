#include "common.h"

const char *applet_name;

int main(int argc, char **argv)
{
	const char *s;

	applet_name = argv[0];

	if (applet_name[0] == '-')
		applet_name++;

	for (s = applet_name; *s != '\0';) {
		if (*s++ == '/')
			applet_name = s;
	}

#define TRY_CALL(X)                                 \
    if (!strcmp(applet_name, #X)) {                 \
        extern int X##_main(int argc, char **argv); \
        X##_main(argc, argv);                       \
        exit(0);                                    \
    }

    TRY_CALL(basename);
    TRY_CALL(cat);
    TRY_CALL(chgrp);
    TRY_CALL(chmod);
    TRY_CALL(chown);
    TRY_CALL(cmp);
    TRY_CALL(cut);
    TRY_CALL(date);
    TRY_CALL(dirname);
    TRY_CALL(du);
    TRY_CALL(echo);
    TRY_CALL(env);
    TRY_CALL(find);
    TRY_CALL(head);
    TRY_CALL(hostid);
    TRY_CALL(hostname);
    TRY_CALL(id);
    TRY_CALL(ln);
    TRY_CALL(ls);
    TRY_CALL(md5sum);
    TRY_CALL(mkdir);
    TRY_CALL(mkfifo);
    TRY_CALL(more);
    TRY_CALL(mv);
    TRY_CALL(nc);
    TRY_CALL(nslookup);
    TRY_CALL(printf);
    TRY_CALL(pwd);
    TRY_CALL(readlink);
    TRY_CALL(rm);
    TRY_CALL(rmdir);
    TRY_CALL(sleep);
    TRY_CALL(sort);
    TRY_CALL(tail);
    TRY_CALL(tee);
    TRY_CALL(test);
    TRY_CALL(time);
    TRY_CALL(touch);
    TRY_CALL(tr);
    TRY_CALL(uname);
    TRY_CALL(uniq);
    TRY_CALL(wc);
    TRY_CALL(wget);
    TRY_CALL(which);
    TRY_CALL(whoami);
    TRY_CALL(xargs);
    TRY_CALL(yes);
    printf("invalid command\n");
    exit(0);
}

// vim:ft=objc
