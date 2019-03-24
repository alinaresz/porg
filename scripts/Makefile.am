.PHONY: \
	logme

EXTRA_DIST = \
	porg_bash_completion

bin_SCRIPTS = \
	paco2porg \
	porgball

install-exec-local:
		$(mkinstalldirs) $(DESTDIR)$(sysconfdir)/bash_completion.d && \
		$(INSTALL_DATA) porg_bash_completion $(DESTDIR)$(sysconfdir)/bash_completion.d

logme_files = \
	$(DESTDIR)$(sysconfdir)/bash_completion.d/porg_bash_completion \
	$(DESTDIR)$(bindir)/paco2porg \
	$(DESTDIR)$(bindir)/porgball

logme:
	ls $(logme_files) | porg -lp+ porg-$(PACKAGE_VERSION)

