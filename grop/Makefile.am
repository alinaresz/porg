.PHONY: \
	logme

noinst_HEADERS = \
	opt.h \
	db.h \
	find.h \
	pkg.h \
	util.h \
	mainwindow.h \
	removepkg.h \
	properties.h \
	porgball.h \
	preferences.h \
	filestreeview.h \
	infotextview.h \
	lock.h \
	maintreeview.h

if ENABLE_GROP

bin_PROGRAMS = \
	grop

grop_SOURCES = \
	main.cc \
	find.cc \
	util.cc \
	porgball.cc \
	removepkg.cc \
	filestreeview.cc \
	infotextview.cc \
	properties.cc \
	mainwindow.cc \
	preferences.cc \
	maintreeview.cc \
	db.cc \
	lock.cc \
	opt.cc

grop_CXXFLAGS = \
	-I$(top_srcdir)/lib \
	-DDATADIR='"$(DESTDIR)$(datadir)"' \
	$(MY_CXXFLAGS) \
	$(GTKMM_CFLAGS)

grop_LDADD = \
	$(GTKMM_LIBS) \
	$(top_srcdir)/lib/porg/libporg.a

logme_files = \
    $(DESTDIR)$(bindir)/grop

logme:
	ls $(logme_files) | porg -lp+ porg-$(PACKAGE_VERSION)

else

logme:

endif

