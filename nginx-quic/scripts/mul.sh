#!/bin/bash
dev=lo
bandwidth=100
delay=20
cong=(oBBR BBR-S BBR B3R) # BBRv2 CUBIC)
obbr_u=(1.0 0.5 0.75)
flows=(2 1 0 3 4) 
script_dir=$(dirname "$0")
dir=$script_dir/../data/mul
run_dir=$script_dir/../bin

cd $script_dir/../

mkdir -p $dir

len=${#cong[*]}

sudo tc qdisc del dev $dev root > /dev/null 2>&1

sudo tc qdisc add dev $dev root handle 1:0 netem delay ${delay}ms
sudo tc qdisc add dev $dev parent 1:1 handle 10: tbf rate ${bandwidth}Mbit burst 100K limit 250KB


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


for flow in "${flows[@]}"
do
    pwd=$dir/${flow}_flows
    mkdir -p $pwd

    echo ------ ${flow}_flows ------

    for con in "${cong[@]}"
    do
        if [ $con = 'oBBR' ]
        then
            for j in $(seq 0 `expr ${#obbr_u[*]} - 1`)
            do
                pid1=()
                for f in $(seq 0 $flow)
                do
                    sudo rm $pwd/$con-${obbr_u[j]}_$f > /dev/null 2>&1
                    sudo sh -c "exec ${run_dir}/nginx -c ../nginx${f}/conf/nginx.conf -C $con ${obbr_u[j]} -O $pwd/$con-${obbr_u[j]}_$f" &
                    pid1+=("$!")
                done
                pid2=()
                for f in $(seq 0 $flow)
                do
                    sudo sh -c "exec ${run_dir}/quic_client https://test.bpqiang.cloud:44${f}/test > /dev/null 2>&1" & 
                    pid2+=("$!")
                done

                for pid in "${pid2[@]}"
                do
                    wait "$pid"
                done
                for pid in "${pid1[@]}"
                do
                    sudo pkill -P $pid
                done

                sleep 5
                echo $con-${obbr_u[j]} done
            done
        else
			pid1=()
            for f in $(seq 0 $flow)
            do
                sudo rm $pwd/${con}_$f > /dev/null 2>&1
                sudo sh -c "exec ${run_dir}/nginx -c ../nginx${f}/conf/nginx.conf -C $con -O $pwd/${con}_$f" &
                pid1+=("$!")
            done
            pid2=()
            for f in $(seq 0 $flow)
            do
                sudo sh -c "exec ${run_dir}/quic_client https://test.bpqiang.cloud:44${f}/test > /dev/null 2>&1" & 
                pid2+=("$!")
            done

            for pid in "${pid2[@]}"
            do
                wait "$pid"
            done
            for pid in "${pid1[@]}"
            do
                sudo pkill -P $pid
            done

            sleep 5
            echo $con done      
        fi

    done
done


#python3 $script_dir/plots/mul.py

