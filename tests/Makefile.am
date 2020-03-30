
SUBDIRS = \
	testutils \
        static_unboxer \
	unboxer \
        unboxingdata

EXTRA_DIST = testdata
dist-hook:
	rm -rf `find $(distdir)/testdata -type d -name .svn`

