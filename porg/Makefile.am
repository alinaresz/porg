.PHONY: \
	logme

bin_PROGRAMS = \
	porg

porg_CXXFLAGS = \
	-I$(top_srcdir)/lib \
	-DLIBDIR='"$(DESTDIR)$(libdir)"' \
	$(MY_CXXFLAGS)

porg_SOURCES = \
	main.cc \
	pkg.cc \
	newpkg.cc \
	out.cc \
	db.cc \
	logger.cc \
	opt.cc \
	util.cc

noinst_HEADERS = \
	util.h \
	out.h \
	db.h \
	pkg.h \
	newpkg.h \
	logger.h \
	main.h \
	opt.h

porg_LDADD = \
	$(top_builddir)/lib/porg/libporg.a

logme_files = \
	$(DESTDIR)$(bindir)/porg

logme:
	ls $(logme_files) | porg -lp+ porg-$(PACKAGE_VERSION)

