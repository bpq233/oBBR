#!/bin/bash
script_dir=$(dirname "$0")

check_and_kill_process() {
    process_name="$1"

    pids=$(pgrep -f "$process_name")

    if [ -n "$pids" ]
    then
        sudo kill $pids
    fi
}

check_and_kill_process "fig3.sh"
check_and_kill_process "fig4&5.sh"
check_and_kill_process "fig7.sh"
check_and_kill_process "fig8.sh"
check_and_kill_process "fig9.sh"
check_and_kill_process "fig10.sh"


sudo pkill -f $script_dir/nets/
sudo pkill -f test/nginx
sudo pkill -f test/quic_client
sudo tc qdisc del dev $dev root > /dev/null 2>&1