# By default, consult angstrom mirrors last for git and svn
# Let all other URL types consult angstrom-mirrors first.
# This overrides the class bundled with openembedded.

PREMIRRORS_append () {
cvs://.*/.* http://www.angstrom-distribution.org/unstable/sources/
hg://.*/.* http://www.angstrom-distribution.org/unstable/sources/
bzr://.*/.* http://www.angstrom-distribution.org/unstable/sources/
}

PREMIRRORS_append () {
ftp://.*/.*	http://www.angstrom-distribution.org/unstable/sources/
https?$://.*/.*	http://www.angstrom-distribution.org/unstable/sources/
}

POSTMIRRORS_append () {
svn://.*/.* http://www.angstrom-distribution.org/unstable/sources/
git://.*/.* http://www.angstrom-distribution.org/unstable/sources/
}
