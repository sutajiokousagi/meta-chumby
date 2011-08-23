def get_hostname():
    from subprocess import Popen, PIPE
    return Popen(["wget", "-q", "-O", "-", "http://ifconfig.me/host"], stdout=PIPE).communicate()[0].replace("\n", "")

FEED_DEPLOYDIR_BASE_URI ?= "http://${@get_hostname()}${FEED_DEPLOYDIR_PATH}"
