#!/bin/bash
script_dir=$(dirname "$0")
dev=lo
cong=(BBRv2 BBR oBBR B3R CUBIC BBR-S)
traces=(static1 static2 car1 car2)
obbr_u=(0.5 0.75 1.0)
dir=$script_dir/../data/oBBR_fig9
run_dir=$script_dir/../bin

cd $script_dir/../

mkdir -p $dir

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

echo ========== Fig9 experiment ==========

for trace in "${traces[@]}"
do
    echo ======trace: $trace======
    pwd=$dir/$trace
    mkdir -p $pwd

    for con in "${cong[@]}"
    do
        if [ $con = 'oBBR' ]
        then
            for j in $(seq 0 `expr ${#obbr_u[*]} - 1`)
            do
                sudo rm $pwd/$con-${obbr_u[j]} > /dev/null 2>&1
                sudo sh -c "exec $script_dir/nets/$trace.sh" &
                pid1=$!
                sudo sh -c "exec $run_dir/nginx -C $con ${obbr_u[j]} -O $pwd/$con-${obbr_u[j]}" &
				pid2=$!
                sudo sh -c "exec ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1" & 
                pid3=$!

                wait $pid3
                sudo pkill -P $pid1
                sudo pkill -P $pid2

                sleep 10
                echo $con-${obbr_u[j]} done
            done
        else
			sudo rm $pwd/$con > /dev/null 2>&1
            sudo sh -c "exec $script_dir/nets/$trace.sh" &
            pid1=$!
            sudo sh -c "exec $run_dir/nginx -C $con -O $pwd/$con" &
            pid2=$!
            sudo sh -c "exec ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1" & 
            pid3=$!

            wait $pid3
            sudo pkill -P $pid1
            sudo pkill -P $pid2

            sleep 10
            echo $con done
        fi

    done

done

python3 $script_dir/plots/fig9.py