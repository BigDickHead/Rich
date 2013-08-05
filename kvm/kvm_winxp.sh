#!/bin/bash

#NAT:
kvm -m 512 -drive file=winxp.img,cache=writeback,if=virtio -localtime -net nic,model=virtio -net user -smp 2 -redir tcp:3389::3389 -redir tcp:8580::8580 -nographic #-soundhw es1370 >/dev/null 2>&1 & ####-vnc 127.0.0.1:0  ###-vnc :0

#Bridge:
#remove -soundhw es1370
kvm -m 512 -drive file=winxp.img,cache=writeback,if=virtio -localtime -net nic -net tap,ifname=tap0,script=no -smp 2 -redir tcp:3389::3389 -redir tcp:8580::8580 -nographic #-soundhw es1370 ####-vnc 127.0.0.1:0  ###-vnc :0 ###-net nic,macaddr=52-52-00-66-98-66 -net tap,ifname=tap0,script=no

# remove -D option
sleep 50 ;rdesktop localhost -u administrator -p 'password' -f -N -z -P -r sound:local -clipboard >/dev/null 2>&1 &
