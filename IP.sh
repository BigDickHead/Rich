#!/bin/bash
while true
do
    wget http://iframe.ip138.com/ic.asp -O /tmp/ip
    sendemail -t somebody@gmail.com -f somebody@gmail.com -s smtp.gmail.com -u `egrep "[0-9]{1,3}\." /tmp/ip |awk -F "[" '{print $2}'|awk -F "]" '{print $1}'` -a /tmp/ip -m `cat /tmp/ip` -xu somebody -xp password
    sleep 600
done


ssh_tunnel.sh
#========================================================================
#!/bin/bash
sudo -k /etc/init.d/ssh start >/dev/null 2>&1 &
sleep 10

sh detect.sh

while true
do
    wget https://raw.github.com/richardlee8681/Code/master/IP -O ssh_ip -o /dev/null
    IP=`cat ssh_ip|openssl enc -aes-256-ecb -d -a -pass pass:PASSWORD`

    nc -vv -w2 -z $IP 39899 2>&1 |grep -q open

    if [ $? == 0 ]
    then
        ssh -o StrictHostKeyChecking=no -o ServerAliveInterval=30 -o UserKnownHostsFile=/dev/null -R 9090:127.0.0.1:39899 -N -i .ssh/id_rsa -p 39899 Richard@$IP
        sleep 90
        continue
    fi

    sleep 300
done
#========================================================================




detect.sh
#========================================================================
#!/bin/bash
while true
do
    ps aux |grep StrictHostKeyChecking |grep -v grep
    if [ $? != 0 ]
    then
        sleep 90
        continue
    fi

    IP=`cat ssh_ip|openssl enc -aes-256-ecb -d -a -pass pass:PASSWORD`

    nc -vv -w2 -z $IP 39899 2>&1 |grep -q open

    if [ $? != 0 ]
    then
        sleep 20
        nc -vv -w2 -z $IP 39899 2>&1 |grep -q open
        if [ $? != 0 ]
        then
            killall -9 ssh
        fi
    fi
    sleep 90
done
#========================================================================
