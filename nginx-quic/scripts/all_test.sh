script_dir=$(dirname "$0")
dev=lo
clean() {
    sudo pkill -f $script_dir/nets/
    sudo pkill -f test/nginx
    sudo pkill -f test/quic_client
    sudo tc qdisc del dev $dev root > /dev/null 2>&1
    exit 0
}

trap clean EXIT

start_time=$(date +%s)
sudo ./scripts/fig7.sh
end_time=$(date +%s)
duration=$((end_time - start_time))
echo "fig7 done: $duration s"

start_time=$(date +%s)
sudo ./scripts/fig8.sh
end_time=$(date +%s)
duration=$((end_time - start_time))
echo "fig8 done: $duration s"

start_time=$(date +%s)
sudo ./scripts/fig9.sh
end_time=$(date +%s)
duration=$((end_time - start_time))  
echo "fig9 done: $duration s"

start_time=$(date +%s)
sudo ./scripts/fig10.sh
end_time=$(date +%s) 
duration=$((end_time - start_time))
echo "fig10 done: $duration s"

start_time=$(date +%s)
sudo ./scripts/fig3.sh
end_time=$(date +%s)
duration=$((end_time - start_time))
echo "fig3 done: $duration s"

start_time=$(date +%s)
sudo ./scripts/fig4_5.sh
end_time=$(date +%s)
duration=$((end_time - start_time))
echo "fig4_5 done: $duration s"
