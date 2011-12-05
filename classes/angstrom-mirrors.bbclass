# By default, consult angstrom mirrors last.  This is because sometimes,
# the original site is much faster.
# This overrides the class bundled with openembedded.

PREMIRRORS_append () {
svn://.*/.* http://netv-bunnie-bar.com/sources-mirror/
git://.*/.* http://netv-bunnie-bar.com/sources-mirror/
cvs://.*/.* http://netv-bunnie-bar.com/sources-mirror/
hg://.*/.* http://netv-bunnie-bar.com/sources-mirror/
bzr://.*/.* http://netv-bunnie-bar.com/sources-mirror/
ftp://.*/.*	http://netv-bunnie-bar.com/sources-mirror/
https?$://.*/.*	http://netv-bunnie-bar.com/sources-mirror/

svn://.*/.* http://www.angstrom-distribution.org/unstable/sources/
git://.*/.* http://www.angstrom-distribution.org/unstable/sources/
cvs://.*/.* http://www.angstrom-distribution.org/unstable/sources/
hg://.*/.* http://www.angstrom-distribution.org/unstable/sources/
bzr://.*/.* http://www.angstrom-distribution.org/unstable/sources/
ftp://.*/.*	http://www.angstrom-distribution.org/unstable/sources/
https?$://.*/.*	http://www.angstrom-distribution.org/unstable/sources/
}
