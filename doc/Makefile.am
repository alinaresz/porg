.PHONY: \
	logme

EXTRA_DIST = \
	grop.desktop \
	faq.txt \
	porg.png \
	porg.spec \
	download.png \
	index.html \
	grop.svg \
	grop.png

man_MANS = \
	porg.8 \
	porgball.8 \
	porgrc.5

pkgdata_DATA = \
	porgrc \
	index.html \
	porg.png \
	download.png \
	faq.txt

logme_files = \
	$(DESTDIR)$(man5dir)/porgrc.5 \
	$(DESTDIR)$(man8dir)/porg.8 \
	$(DESTDIR)$(man8dir)/porgball.8 \
	$(DESTDIR)$(sysconfdir)/porgrc \
	$(DESTDIR)$(pkgdatadir)/porgrc \
	$(DESTDIR)$(pkgdatadir)/index.html \
	$(DESTDIR)$(pkgdatadir)/porg.png \
	$(DESTDIR)$(pkgdatadir)/download.png \
	$(DESTDIR)$(pkgdatadir)/faq.txt

if ENABLE_GROP

grop_apps_DATA = \
	grop.desktop

grop_appsdir = \
	$(DESTDIR)$(datadir)/applications
	
grop_icons_DATA = \
	grop.png

grop_iconsdir = \
	$(DESTDIR)$(datadir)/pixmaps

logme_files += \
	$(grop_appsdir)/grop.desktop \
	$(grop_iconsdir)/grop.png

porg_icon_path = \
	$(grop_iconsdir)/grop.png

else

porg_icon_path = \
	$(DESTDIR)$(pkgdatadir)/porg.png

endif

logdir = \
    $(DESTDIR)@LOGDIR@

install-data-local: porgrc
	rc_dir="$(DESTDIR)$(sysconfdir)"; \
	rc_file="$$rc_dir/porgrc"; \
	( test -d $$rc_dir || $(mkinstalldirs) $$rc_dir ); \
	( test -f $$rc_file || $(INSTALL_DATA) porgrc $$rc_file ); \
	touch $$rc_file

logme:
	ls $(logme_files) | porg -lp+ porg-$(PACKAGE_VERSION) && \
	$(SED) -i "s|^#i:.*|#i:$(porg_icon_path)|" $(logdir)/porg-$(PACKAGE_VERSION)

web: index.html
	scp $< davidrr,porg@web.sourceforge.net:htdocs/

