#!/bin/sh


case "$1" in
        start)
		/usr/sbin/monitor-reset-button
                ;;

        stop)
                killall monitor-reset-button
                ;;

        restart)
                $0 stop
                $0 start
                ;;

        *)

                echo "Usage: $0 {start|stop|restart}"
                ;;
esac
