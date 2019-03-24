lib_LTLIBRARIES = \
	libporg-log.la

libporg_log_la_SOURCES = \
	log.c

libporg_log_la_CFLAGS = \
	$(MY_CFLAGS)

logme:
	ls $(DESTDIR)$(libdir)/libporg-log* | porg -lp+ porg-$(PACKAGE_VERSION)
