#!/bin/bash
script_dir=$(dirname "$0")
dev=lo
cong=(BBR oBBR)
dir=$script_dir/../data/oBBR_fig8
run_dir=$script_dir/../bin

mkdir -p $dir

cd $script_dir/../

sudo tc qdisc del dev $dev root > /dev/null 2>&1

sudo tc qdisc add dev $dev root handle 1:0 netem delay 50ms
sudo tc qdisc add dev $dev parent 1:1 handle 10: tbf rate 40Mbit burst 100KB limit 250KB

cleanup() {
    sudo pkill -f $script_dir/nets/
    sudo pkill -f $run_dir/nginx
    sudo pkill -f quic_client
    sudo tc qdisc del dev $dev root > /dev/null 2>&1
    exit 0
}

trap cleanup EXIT
trap cleanup SIGTERM
trap cleanup SIGINT

echo ========== Fig8 experiment ==========

for con in "${cong[@]}"
do
    pwd1=$dir/${con}
    pwd2=$dir/${con}_rtt
    sudo rm $pwd1 > /dev/null 2>&1
    sudo rm $pwd2 > /dev/null 2>&1

    sudo sh -c "exec $script_dir/nets/change.sh 40 10 300 10" &
#    eval ' sudo $script_dir/nets/change.sh' &
    pid1=$!
    pid2=''
    if [ $con = 'oBBR' ]
    then
        sudo sh -c "exec ${run_dir}/nginx -C oBBR 1.0 -O $pwd1 -r $pwd2" &
#        eval 'sudo $script_dir/${run_dir}/nginx -C oBBR 1.0 -O $pwd1 -r $pwd2' &
        pid2=$!
    else
        sudo sh -c "exec ${run_dir}/nginx -C BBR -O $pwd1 -r $pwd2" &  
#        eval 'sudo $script_dir/${run_dir}/nginx -C BBR -O $pwd1 -r $pwd2' &
        pid2=$!
    fi
    sudo sh -c "exec ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1" & 
    pid3=$!

    wait $pid3

    sudo pkill -P $pid1
 #   sudo kill -9 $pid1

    sudo pkill -P $pid2
 #   sudo kill -9 $pid2

    sleep 10
    echo $con done
done

python3 $script_dir/plots/fig8.py