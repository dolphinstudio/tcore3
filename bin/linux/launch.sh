#!/bin/bash
launch_dir="$(cd $(dirname $0) && pwd)"
kill -9 $(pgrep -d\  tcore3)
./tcore3 --name=master &
./tcore3 --name=match --noder_ip=127.0.0.1 --noder_port=21000 --noder_area=1 &
./tcore3 --name=relation --noder_ip=127.0.0.1 --noder_port=11000 --noder_area=1 --allarea &
./tcore3 --name=logicer --noder_ip=127.0.0.1 --noder_port=12000 --noder_area=1 --allarea &
sleep 1
./tcore3 --name=gate --noder_ip=127.0.0.1 --noder_port=31000 --noder_area=1 --gameport=32000 &
./tcore3 --name=gate --noder_ip=127.0.0.1 --noder_port=31001 --noder_area=1 --gameport=32001 &
