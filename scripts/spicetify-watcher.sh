#!/bin/bash

pid=""

while true; do 
	count=$(pgrep spotify | wc -l)
	if [ "$count" -gt 5 ] && [ -z "$pid" ]; then
		spicetify -s watch &
		pid=$!
		echo "started"
	fi
	
	if [ "$count" -lt 5 ] && [ -n "$pid" ]; then
		kill -9 "$pid" >/dev/null
		pid=""
		echo "stopped"
	fi
	
	sleep 1
done

