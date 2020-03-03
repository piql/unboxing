
SUBDIRS = \
	testutils \
        static_unboxer \
	unboxer

if UNIT_TESTS_ENABLED
        SUBDIRS += unboxingdata
endif

EXTRA_DIST = testdata
dist-hook:
	rm -rf `find $(distdir)/testdata -type d -name .svn`

