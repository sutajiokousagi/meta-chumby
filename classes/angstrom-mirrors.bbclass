# By default, consult angstrom mirrors last.  This is because sometimes,
# the original site is much faster.
# This overrides the class bundled with openembedded.

MIRRORS_append () {
svn://.*/.* http://www.angstrom-distribution.org/unstable/sources/
git://.*/.* http://www.angstrom-distribution.org/unstable/sources/
cvs://.*/.* http://www.angstrom-distribution.org/unstable/sources/
hg://.*/.* http://www.angstrom-distribution.org/unstable/sources/
bzr://.*/.* http://www.angstrom-distribution.org/unstable/sources/
ftp://.*/.*	http://www.angstrom-distribution.org/unstable/sources/
https?$://.*/.*	http://www.angstrom-distribution.org/unstable/sources/
}
