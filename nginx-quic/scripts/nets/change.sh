#!/bin/bash
dev=lo
while true
do
	sudo tc qdisc change dev $dev parent 1:1 handle 10: tbf rate $1Mbit burst $5KB limit $3KB
	sleep $4
	sudo tc qdisc change dev $dev parent 1:1 handle 10: tbf rate $2Mbit burst $5KB limit $3KB
	sleep $4
done
