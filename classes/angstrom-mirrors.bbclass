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
