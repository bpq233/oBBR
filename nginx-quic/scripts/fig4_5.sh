#!/bin/bash
script_dir=$(dirname "$0")
dev=lo
dir=$script_dir/../data/oBBR_fig4_5
run_dir=$script_dir/../bin
time=180

cd $script_dir/../

mkdir -p $dir

sudo tc qdisc del dev $dev root > /dev/null 2>&1

sudo tc qdisc add dev $dev root handle 1:0 netem delay 50ms
sudo tc qdisc add dev $dev parent 1:1 handle 10: tbf rate 20Mbit burst 100KB limit 500KB

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

echo ========== Fig4_5 experiment ==========

pwd1=$dir/L_20-5Mbps
pwd2=$dir/L_20-5Mbps_rtt
sudo rm $pwd1 > /dev/null 2>&1
sudo rm $pwd2 > /dev/null 2>&1

pid1=''
pid2=''

sudo sh -c "exec $script_dir/nets/change.sh 20 5 200 12" &
pid1=$!
sudo sh -c "exec ${run_dir}/nginx -C BBR -O $pwd1 -r $pwd2" & 
pid2=$! 
eval 'sudo timeout 40s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
sleep 40
sudo pkill -P $pid1
sudo pkill -P $pid2

sleep 5
echo 20-5Mbps_L done


pwd1=$dir/L_20-2Mbps
pwd2=$dir/L_20-2Mbps_rtt
sudo rm $pwd1 > /dev/null 2>&1
sudo rm $pwd2 > /dev/null 2>&1

pid1=''
pid2=''

sudo sh -c "exec $script_dir/nets/change.sh 20 2 200 12" &
pid1=$!
sudo sh -c "exec ${run_dir}/nginx -C BBR -O $pwd1 -r $pwd2" & 
pid2=$! 
eval 'sudo timeout 40s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
sleep 40
sudo pkill -P $pid1
sudo pkill -P $pid2

sleep 5
echo 20-2Mbps_L done

pwd1=$dir/20Mbps
pwd2=$dir/20Mbps_rtt
sudo rm $pwd1 > /dev/null 2>&1
sudo rm $pwd2 > /dev/null 2>&1

pid1=''
pid2=''

sudo tc qdisc change dev $dev parent 1:1 handle 10: tbf rate 20Mbit burst 100KB limit 500KB
sudo sh -c "exec ${run_dir}/nginx -C BBR -O $pwd1 -r $pwd2" & 
pid1=$! 
eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
sleep $time
sudo pkill -P $pid1

sleep 5
echo 20Mbps done

pwd1=$dir/20-10Mbps
pwd2=$dir/20-10Mbps_rtt
sudo rm $pwd1 > /dev/null 2>&1
sudo rm $pwd2 > /dev/null 2>&1

pid1=''
pid2=''

sudo sh -c "exec $script_dir/nets/change.sh 20 10 500 30" &
pid1=$!
sudo sh -c "exec ${run_dir}/nginx -C BBR -O $pwd1 -r $pwd2" & 
pid2=$! 
eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
sleep $time
sudo pkill -P $pid1
sudo pkill -P $pid2

sleep 5
echo 20-10Mbps done


pwd1=$dir/20-5Mbps
pwd2=$dir/20-5Mbps_rtt
sudo rm $pwd1 > /dev/null 2>&1
sudo rm $pwd2 > /dev/null 2>&1

pid1=''
pid2=''

sudo sh -c "exec $script_dir/nets/change.sh 20 5 500 30" &
pid1=$!
sudo sh -c "exec ${run_dir}/nginx -C BBR -O $pwd1 -r $pwd2" & 
pid2=$! 
eval 'sudo timeout ${time}s ${run_dir}/quic_client https://test.bpqiang.cloud/test > /dev/null 2>&1' &
sleep $time
sudo pkill -P $pid1
sudo pkill -P $pid2

sleep 5
echo 20-5Mbps done

eval 'python3 $script_dir/plots/fig4_5.py'
