# Artifact Evaluation submission for #260 oBBR: Optimize Retransmissions of BBR flows on the Internet

## Introduction

The goal of `oBBR` is to reduce the retransmissions of the `BBR` congestion control algorithm. It adapts the congestion window to the size of the bottleneck buffer by adjusting the `BBR` parameter `cwnd_gain` and can update  estimated bandwidth in time when the network link bandwidth drops. This artifact contains the implementation of `oBBR` and scripts for reproducing the main results of this work.

## Getting Started

### Directory structure

```sh
oBBR
|---- boringssl                # QUIC support
|---- ssl                      # ssl certificate for domain name
|---- nginx/conf/nginx.conf    # configuration file for the nginx server
|---- nginx1/conf/nginx.conf   # configuration file for the second server
|---- nginx-quic                  
    |---- src/event/quic            # codes of the quic and bbr
    |---- scripts                   # main evaluation scripts
    |---- data                      # evaluation output files
    |---- figs                      # evaluation output figures
    |---- bin                       # Executable programs of server and client
```

### Install dependencies

**OS version:** Ubuntu 20.04

```sh
sudo apt install -y \
    golang \
    cmake \
    perl \
    mercurial \
    libperl-dev \
    libpcre3-dev \
    zlib1g-dev \
    libxslt1-dev \
    libgeoip-dev \
    lrzsz unzip \
    libnss3-dev
    
pip install statsmodels pandas numpy matplotlib
```

### Build

We provide compiled server `nginx` and client `quic_client` in the `oBBR/bin` directory, you can run the experiment directly through the script without building.

```sh
git clone https://github.com/bpq233/oBBR.git
cd oBBR/nginx-quic

./auto/configure --prefix=../nginx --with-debug --with-http_v3_module \
      --with-cc-opt="-I../boringssl/include" \
      --with-ld-opt="-L../boringssl/build/ssl \
      -L../boringssl/build/crypto"
make
```

After a successful compilation, the executable `nginx` is created in `. /objs` directory, if you want to test with the `nginx` you compiled, run the command

```sh
mv ./objs/nginx ./bin/
```

### kick-the-tires

- Run a sample case

  ```sh
  sudo ./scripts/test.sh
  #Requires sudo access to bind ports 80 and 443
  ```

  You will see the success code 200 output by the client.

  This script starts a server and a client to communicate. We have provided the configuration file `nginx.conf` in the `nginx/conf` directory that is needed to run `nginx`, so you can run it directly. The domain `test.bpqiang.cloud` accessed by the client will be resolved to 127.0.0.1.

## Main Experiments 

### Scripts

**You can use the scripts directly from the `nginx-quic/scripts`. These scripts will build and conduct experiments automatically.**

```sh
|-- nets            # set up the network
|-- plots           # plot the experimental figures
|-- clean.sh        # Cleaning process
|-- fig3.sh         # bbr vs cubic (Sec3.1)
|-- fig4&5.sh       # bandwidth drop (Sec3.2 Sec4.2)
|-- fig7.sh         # stable Network Environment (Sec5.2.1)
|-- fig8.sh         # variable Bandwidth (Sec5.2.2)
|-- fig9.sh         # Realistic Network Traces (Sec5.2.3)
|-- fig10.sh        # Competitiveness (Sec5.2.4)
```

Before the experiment, you need to create a test file of `1G` size for data transfer.

```
cd oBBR
dd if=/dev/zero of=../nginx/html/test bs=1073741824 count=1
dd if=/dev/zero of=../nginx1/html/test bs=1073741824 count=1
```

**Note:** Running script for experiment will create multiple servers and clients, which will all stop after the experiment has finished running normally. If you find residual processes that are not stopped after the script ends abnormally, you can run `./clean.sh` to kill them.

### Figure 7 (About 3 hours)

**Command to run:**

```sh
sudo ./scripts/fig7.sh  
```

**Output:** `./data/oBBR_fig7` includes the data of retransmission ratio ` (Fig7.a)` and good-put `(Fig7.b)` of  different `CCAs` (congestion control algorithms) in stable Network Environment. The figures will be plotted in the `oBBR/nginx-quic/figs.`

### Figure 8 (About 15 mins)

**Command to run:**

```sh
sudo ./scripts/fig8.sh  
```

**Output:** `./data/oBBR_fig8` includes the data of bandwidth convergence rate `(Fig8.a)`, `RTT` change `(Fig8.b)`, retransmissions `(Fig8.c)` and good-put `(Fig8.d)` of  `BBR` and `oBBR` in  variable bandwidth Network Environment. The figures will be plotted in the `oBBR/nginx-quic/figs`.

### Figure 9 (About 4 hours)

**Command to run:**

```sh
sudo ./scripts/fig9.sh  
```

**Output:** `./data/oBBR_fig9` includes the data of retransmission ratio and good-put of  different `CCAs` in  Realistic Network Traces Environment. The figures `fig9.a`, `fig9.b`, `fig9.c`, `fig9.d` will be plotted in the `oBBR/nginx-quic/figs`.

### Figure 10 (About 80 mins)

**Command to run:**

```sh
sudo ./scripts/fig10.sh 
```

**Output:** The Competitiveness of `oBBR`. The `fig10` will be plotted in the `oBBR/nginx-quic/figs`.

### Figure 3 (About 90 mins)

**Command to run:**

```sh
sudo ./scripts/fig10.sh 
```

**Output:** The figures `fig3.a`, `fig3.b`, `fig3.c` will be plotted in the `oBBR/nginx-quic/figs`.

### Figure 4&5 (About 15 mins)

**Command to run:**

```sh
sudo ./scripts/fig10.sh 
```

**Output:** The figures `fig4`, `fig5.a`, `fig5.b` and `fig5.c` will be plotted in the `oBBR/nginx-quic/figs`.

### Deployed on the Internet

If you want to test on the Internet, you need to have a domain name as well as an `ssl` certificate, resolve the domain to your host, modify the configuration file `oBBR/nginx/conf/nginx.conf`

```
        listen       80;
        server_name  [your domain name];
        listen 443 http3 reuseport;
        listen 443 ssl;

        ssl_certificate ../../ssl/[your ssl.crt];
        ssl_certificate_key ../../ssl/[your ssl.key];;
        ssl_protocols      TLSv1 TLSv1.1 TLSv1.2 TLSv1.3;
```

- Create a `1G` size test file

```
cd oBBR
dd if=/dev/zero of=./nginx/html/test bs=1073741824 count=1
```

- Run server

```
cd oBBR/nginx-quic
sudo ./objs/nginx [options]

usage: nginx [-C <CCA>] [-O <outfile>] [-r <outfile>] [-l <cwnd_gain>]

Available options:
-C <CCA>              Choose a CCA,Note that oBBR needs to give the parameter μ. for example, -C oBBR 1.0 (0<=μ<=1)
-O <outfile>          Experimental data output file path
-r <outfile>          Record the RTT and bandwidth of each packet sample
-l <cwnd_gain>        Set cwnd_gain for BBR
```

- Run the client on another host

```
./quic_client https://[your domain name]/test
```