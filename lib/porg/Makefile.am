noinst_LIBRARIES = \
	libporg.a

libporg_a_SOURCES = \
	common.cc \
	basepkg.cc \
	baseopt.cc \
	rexp.cc \
	file.cc

noinst_HEADERS = \
	common.h \
	basepkg.h \
	baseopt.h \
	rexp.h \
	file.h

libporg_a_CXXFLAGS = \
	$(MY_CXXFLAGS) \
	-DPORGRC='"$(DESTDIR)$(sysconfdir)/porgrc"'

