#!/bin/bash
script_dir=$(dirname "$0")
cd $script_dir/../

dev=lo

sudo tc qdisc add dev $dev root handle 1:0 netem delay 50ms
sudo tc qdisc add dev $dev parent 1:1 handle 10: tbf rate 40Mbit burst 100KB limit 250KB
sudo tc qdisc del dev $dev root

sudo sh -c "exec ./bin/nginx -C BBR  > /dev/null 2>&1" &
pid=$!
./bin/quic_client https://test.bpqiang.cloud
sudo pkill -P $pid