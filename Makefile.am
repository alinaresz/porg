.PHONY: \
	logme

ACLOCAL_AMFLAGS = -I build

SUBDIRS = \
	lib \
	porg \
	grop \
	doc \
	scripts

pkgdata_DATA = \
	README

EXTRA_DIST = \
	ChangeLog.paco

logdir = \
	$(DESTDIR)@LOGDIR@

svnurl = \
	svn+ssh://davidrr@svn.code.sf.net/p/porg/code


## Create the porg log directory on install, if it does not exist
install-exec-local:
	test -d $(logdir) || $(mkinstalldirs) $(logdir)

logme_files = \
	$(DESTDIR)$(pkgdatadir)/README

logme:
	@echo "'make logme' is no longer required to log the own porg installation."
	@echo "Try 'porg -lp porg-$(PACKAGE_VERSION) make install' instead."
	@echo "If it fails, try 'make logme-really' and report it, please."

## A rule to log the own porg's installation
logme-really:
	ls $(logme_files) | porg -lp porg-$(PACKAGE_VERSION) && \
	for dir in $(SUBDIRS); do \
	  ( cd $$dir && $(MAKE) logme ) 2>/dev/null; \
	done && \
	echo && porg -vvxfsty porg-$(PACKAGE_VERSION)

README: doc/index.html
	html2text $< | $(SED) 's/&ast;/*/g' > README

## Download SVN snapshot (Read / Write)
svn-checkout:
	svn checkout --username=davidrr $(svnurl) porg-code

## Make release tag
svn-release-tag:
	svn copy $(svnurl)/trunk $(svnurl)/tags/$(PACKAGE_VERSION) -m "Created tag $(PACKAGE_VERSION)"

