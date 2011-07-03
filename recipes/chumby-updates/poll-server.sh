#!/bin/bash

# RANDOM is a random number between 0 and 32768.  Happily, 32768 very
# nearly divides evenly into 3600 (leaving 1/450 over).  This is used
# to throttle connections to the server.
DELAY=$(($RANDOM%3600))
URL=http://buildbot.chumby.com.sg/updates/_MACHINE_/update.sh
SCRIPT_PATH=/tmp/update.$$.sh
LAST_ETAG_PATH=/tmp/last-update-etag


echo "Delaying for $DELAY seconds..."
sleep $DELAY


# We use the ETag to determine if the file has changed or not.
ETAG=$(curl --stderr /dev/null -I "$URL" | grep -i ^etag: | cut -d'"' -f2)
if [ -f $LAST_ETAG_PATH ]
then
	if [ "$(cat $LAST_ETAG_PATH)" = "x${ETAG}" ]
	then
		echo "Etag hasn't changed"
		exit 0
	fi
fi
echo "x${ETAG}" > $LAST_ETAG_PATH


# Either this is the first time we've run, or the ETag is new.  Fetch the script.
if ! curl --stderr /dev/null -f -o $SCRIPT_PATH "$URL"
then
	exit 0
fi


# Verify the script's signature matches.
# XXX Implement This XXX


# Run the script
chmod a+x $SCRIPT_PATH
exec $SCRIPT_PATH
