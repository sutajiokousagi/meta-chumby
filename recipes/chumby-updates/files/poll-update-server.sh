#!/bin/bash

# RANDOM is a random number between 0 and 32768.  Happily, 32768 very
# nearly divides evenly into 3600 (leaving 1/450 over).  This is used
# to throttle connections to the server.

# change to 900 seconds 8/30/2011 -- temporary for beta, revert to 3600 for release
DELAY=$(($RANDOM%900))
URL=http://buildbot.chumby.com.sg/updates/_MACHINE_/update.sh
SCRIPT_PATH=/tmp/update.$$.sh
LAST_ETAG_PATH=/tmp/update.last-etag
UPDATE_DEBUG=0
if [ -e /test-updates ]
then
	UPDATE_DEBUG=1
fi


if [ ${UPDATE_DEBUG} -eq 0 ]
then
	logger -t update "pausing for ${DELAY} seconds before checking update"
	sleep $DELAY
fi


# We use the ETag to determine if the file has changed or not.
ETAG=$(curl --stderr /dev/null -I "$URL" | grep -i ^etag: | cut -d'"' -f2)
if [ -f $LAST_ETAG_PATH ]
then
	if [ "x$(cat $LAST_ETAG_PATH)" = "x${ETAG}" ]
	then
		if [ ${UPDATE_DEBUG} -eq 0 ]
		then
			logger -t update "etag hasn't changed - ${ETAG}"
			exit 0
		fi
	fi
fi
echo "${ETAG}" > $LAST_ETAG_PATH


# Either this is the first time we've run, or the ETag is new.  Fetch the script.
if ! curl --stderr /dev/null -f -o $SCRIPT_PATH "$URL"
then
	logger -t update "unable to locate update file - $URL"
	exit 0
fi


# Download the script's key
if ! curl --stderr /dev/null -f -o $SCRIPT_PATH.sig "$URL.sig"
then
	logger -t update "unable to locate update signature file - $URL.sig"
	exit 0
fi


# Verify the script's signature matches, if we're on release.
if ! gpg --lock-never --no-options --no-default-keyring --keyring /etc/opkg/trusted.gpg --secret-keyring /etc/opkg/secring.gpg --trustdb-name /etc/opkg/trustdb.gpg --quiet --batch --verify $SCRIPT_PATH.sig
then
	logger -t update "update script does not match signature"
	if ! grep -q silvermoon-netv-debug /etc/opkg/*.conf
	then
		if [ ${UPDATE_DEBUG} -eq 0 ]
		then
			logger -t update "abandoning update process"
			exit 0
		fi
	fi
	logger -t update "on debug branch: continuing update anyway"
fi


# Run the script
chmod a+x $SCRIPT_PATH
exec $SCRIPT_PATH
