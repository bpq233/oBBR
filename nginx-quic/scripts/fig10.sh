#!/bin/bash
dev=lo
bandwidth=40
delay=50
buffer=(500 250)
cong=(CUBIC BBR BBRv2)
obbr_u=(0.5 0.75 1.0)
script_dir=$(dirname "$0")
dir=$script_dir/../data/oBBR_fig10
run_dir=$script_dir/../bin
time=240
t=10

cd $script_dir/../

mkdir -p $dir

len=${#cong[*]}

sudo tc qdisc del dev $dev root > /dev/null 2>&1

sudo tc qdisc add dev $dev root handle 1:0 netem delay ${delay}ms
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

echo ========== Fig10 experiment ==========

for buf in "${buffer[@]}"
do
    sudo tc qdisc change dev $dev parent 1:1 handle 10: tbf rate ${bandwidth}Mbit burst 100KB limit ${buf}KB

    for i in $(seq 0 `expr $len - 1`)
    do
        for j in $(seq 0 `expr ${#obbr_u[*]} - 1`)
        do
            pwd1=$dir/oBBR-${obbr_u[j]}_vs_${cong[i]}_${buf}KB_1
            pwd2=$dir/oBBR-${obbr_u[j]}_vs_${cong[i]}_${buf}KB_2

            sudo rm $pwd1 > /dev/null 2>&1
            sudo rm $pwd2 > /dev/null 2>&1

            eval 'sudo timeout ${time}s $run_dir/nginx -C oBBR ${obbr_u[j]} -O $pwd1' &
            eval 'sudo timeout ${time}s $run_dir/nginx -c ../nginx1/conf/nginx.conf -C ${cong[i]} -O $pwd2' &

            eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
            eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud:441/test > /dev/null 2>&1' &

            sleep `expr $time + $t`
            echo  oBBR-${obbr_u[j]}_vs_${cong[i]}_${buf}: done

        done
        
    done
done


python3 $script_dir/plots/fig10.py

