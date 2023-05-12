#!/bin/bash
dev=lo
bandwidth=20
delay=20
buffer=(50 80 100 200 1600)
cwnd_gain=(1.0 1.5 2.0 3.0 4.0)
script_dir=$(dirname "$0")
dir=$script_dir/../data/oBBR_fig3
run_dir=$script_dir/../bin
time=420
t=10

cd $script_dir/../

mkdir -p $dir

sudo tc qdisc del dev $dev root > /dev/null 2>&1

sudo tc qdisc add dev $dev root handle 1:0 netem delay ${delay}ms
sudo tc qdisc add dev $dev parent 1:1 handle 10: tbf rate 20Mbit burst 100KB limit 100KB


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

echo ========== Fig3 experiment ==========

for buf in "${buffer[@]}"
do 
    sudo tc qdisc change dev $dev parent 1:1 handle 10: tbf rate ${bandwidth}Mbit burst 100KB limit ${buf}KB

    if [ $buf = '1600' ]
    then 
        for g in "${cwnd_gain[@]}"
        do
            pwd1=$dir/cubic_vs_bbr_g=${g}_${buf}KB_1
            pwd2=$dir/cubic_vs_bbr_g=${g}_${buf}KB_2
            sudo rm $pwd1 > /dev/null 2>&1
            sudo rm $pwd2 > /dev/null 2>&1

            eval 'sudo timeout ${time}s ${run_dir}/nginx -C CUBIC -O $pwd1' &
            eval 'sudo timeout ${time}s ${run_dir}/nginx -c ../nginx1/conf/nginx.conf -C BBR -l ${g} -O $pwd2' &

            eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
            sleep 3
            eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud:441/test > /dev/null 2>&1' &
            

            sleep `expr $time + $t`

            echo cubic_vs_bbr_g=${g}_${buf}KB done

        done
    else
        pwd1=$dir/cubic_vs_bbr_g=1.0_${buf}KB_1
        pwd2=$dir/cubic_vs_bbr_g=1.0_${buf}KB_2
        sudo rm $pwd1 > /dev/null 2>&1
        sudo rm $pwd2 > /dev/null 2>&1

        eval 'sudo timeout ${time}s ${run_dir}/nginx -C CUBIC -O $pwd1' &
        eval 'sudo timeout ${time}s ${run_dir}/nginx -c ../nginx1/conf/nginx.conf -C BBR -l 1.0 -O $pwd2' &

        eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
        sleep 3
        eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud:441/test > /dev/null 2>&1' &

        sleep `expr $time + $t`

        echo cubic_vs_bbr_g=1.0_${buf}KB done

        pwd1=$dir/cubic_vs_bbr_g=2.0_${buf}KB_1
        pwd2=$dir/cubic_vs_bbr_g=2.0_${buf}KB_2

        sudo rm $pwd1 > /dev/null 2>&1
        sudo rm $pwd2 > /dev/null 2>&1

        eval 'sudo timeout ${time}s ${run_dir}/nginx -C CUBIC -O $pwd1' &
        eval 'sudo timeout ${time}s ${run_dir}/nginx -c ../nginx1/conf/nginx.conf -C BBR -l 2.0 -O $pwd2' &

        eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
        sleep 3
        eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud:441/test > /dev/null 2>&1' &

        sleep `expr $time + $t`

        echo cubic_vs_bbr_g=2.0_${buf}KB done
    fi

done

python3 $script_dir/plots/fig3.py
