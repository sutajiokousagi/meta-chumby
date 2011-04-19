CNPLATFORM = "${@bb.data.getVar('MACHINE', d, 1).replace('chumby-', '')}"

# chumby SVN repos are pulled into GIT.
# Because we pull from the production/release branch (instead of trunk),
# information about a particular platform "leaks" into PV.
# The chumby build system versions are of the form of:
#     [CNPLATFORM]-[Major].[Minor].[Build]
# Because of the way PV is sorted, CNPLATFORM would always override the
# Major.Minor.Build version, which is usually correct.
# When recipes are created, we encode this in the PV as:
#     [Build].[Major].[Minor].[CNPLATFORM]
# That way it sorts nicely.
# In order to get the repo URL, we need to unpack it.
RBUILD = "${@bb.data.getVar('PV', d, 1).split('.')[-4]}"
RMAJOR = "${@bb.data.getVar('PV', d, 1).split('.')[-3]}"
RMINOR = "${@bb.data.getVar('PV', d, 1).split('.')[-2]}"
RPLATFORM = "${@bb.data.getVar('PV', d, 1).split('.')[-1]}"
RVERSION = "${RPLATFORM}_${RMAJOR}.${RMINOR}.${RBUILD}"

