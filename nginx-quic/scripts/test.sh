#!/bin/bash
script_dir=$(dirname "$0")
cd $script_dir/../

sudo ls > /dev/null 2>&1

sudo sh -c "exec ./bin/nginx -C BBR  > /dev/null 2>&1" &
pid=$!
./bin/quic_client https://test.bpqiang.cloud
sudo pkill -P $pid