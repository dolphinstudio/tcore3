cd %~dp0
echo off;
echo "kill old proc..."
taskkill /im tcore3.exe
pause
echo "launche string...."

start "master" tcore3 --name=master
start "gate" tcore3 --name=gate --noder_ip=127.0.0.1 --noder_port=31000 --noder_area=1 --gameport=32000
start "match " tcore3 --name=match --noder_ip=127.0.0.1 --noder_port=21000 --noder_area=1
start "relation" tcore3 --name=relation --noder_ip=127.0.0.1 --noder_port=11000 --noder_area=1 --allarea
start "logicer" tcore3 --name=logicer --noder_ip=127.0.0.1 --noder_port=12000 --noder_area=1 --allarea
