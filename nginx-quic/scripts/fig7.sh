#!/bin/bash
dev=lo
bandwidth=(40 40 100 100 60 60 60)
delay=(50 50 20 20 30 30 30)
buffer=(125 125 125 125 90 180 270)
loss=(0 2 0 1 0 0 0)
cong=(oBBR BBR BBR-S B3R CUBIC BBRv2)
obbr_u=(0.5 0.75 1.0)
script_dir=$(dirname "$0")
dir=$script_dir/../data/oBBR_fig7
run_dir=$script_dir/../bin
time=300
t=10

cd $script_dir/../

len=${#bandwidth[*]}

sudo tc qdisc del dev $dev root > /dev/null 2>&1

sudo tc qdisc add dev $dev root handle 1:0 netem delay 50ms
sudo tc qdisc add dev $dev parent 1:1 handle 10: tbf rate 40Mbit burst 100KB limit 125KB


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

for i in $(seq 0 `expr $len - 1`)
do
    sudo tc qdisc change dev $dev root handle 1:0 netem delay ${delay[i]}ms loss ${loss[i]}%
    sudo tc qdisc change dev $dev parent 1:1 handle 10: tbf rate ${bandwidth[i]}Mbit burst 100KB limit ${buffer[i]}KB

    RTT=`expr ${delay[i]} \* 2`
	pwd=$dir/${bandwidth[i]}Mbps_${RTT}ms_${buffer[i]}KB_loss${loss[i]}%
    mkdir -p $pwd

    echo bandwidth:${bandwidth[i]}Mbps RTT:${RTT}ms buffer:${buffer[i]}KB Loss:${loss[i]}%

    for con in "${cong[@]}"
    do
        if [ $con = 'oBBR' ]
        then
            for j in $(seq 0 `expr ${#obbr_u[*]} - 1`)
            do
				sudo rm $pwd/$con-${obbr_u[j]} > /dev/null 2>&1
                sudo sh -c "exec ${run_dir}/nginx -C $con ${obbr_u[j]} -O $pwd/$con-${obbr_u[j]}" &
                pid1=$!
                sudo sh -c "exec timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1" & 
                pid2=$!
                wait $pid2
                sudo pkill -P $pid1
                echo $con-${obbr_u[j]} done
            done
        else
			sudo rm $pwd/$con > /dev/null 2>&1
            sudo sh -c "exec ${run_dir}/nginx -C $con -O $pwd/$con" &
            pid1=$!
            sudo sh -c "exec timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1" & 
            pid2=$!
            wait $pid2
            sudo pkill -P $pid1
            echo $con done          
        fi

    done
    

done


python3 $script_dir/plots/fig7.py

