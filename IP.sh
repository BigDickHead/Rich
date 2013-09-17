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

trap "rm -f detect_SSH.sh ; killall -9 detect_SSH.sh && exit" 1 2 15

PASSWORD=PASSWORD

Generate_detect_SSH()
{
echo  '#!/bin/bash'  >> detect_SSH.sh
echo  "PASSWORD=$PASSWORD" >> detect_SSH.sh
echo  'while true'  >> detect_SSH.sh
echo  'do'  >> detect_SSH.sh
echo  '    ps aux |grep StrictHostKeyChecking |grep -v grep'  >> detect_SSH.sh
echo  '    if [ $? != 0 ]'  >> detect_SSH.sh
echo  '    then'  >> detect_SSH.sh
echo  '        sleep 90'  >> detect_SSH.sh
echo  '        continue'  >> detect_SSH.sh
echo  '    fi'  >> detect_SSH.sh
echo  ''  >> detect_SSH.sh
echo  '    IP=`cat ssh_ip|openssl enc -aes-256-ecb -d -a -pass pass:$PASSWORD`'  >> detect_SSH.sh
echo  ''  >> detect_SSH.sh
echo  '    nc -vv -w2 -z $IP 39899 2>&1 |grep -q open'  >> detect_SSH.sh
echo  ''  >> detect_SSH.sh
echo  '    if [ $? != 0 ]'  >> detect_SSH.sh
echo  '    then'  >> detect_SSH.sh
echo  '        sleep 20'  >> detect_SSH.sh
echo  '        nc -vv -w2 -z $IP 39899 2>&1 |grep -q open'  >> detect_SSH.sh
echo  '        if [ $? != 0 ]'  >> detect_SSH.sh
echo  '        then'  >> detect_SSH.sh
echo  '            killall -9 ssh'  >> detect_SSH.sh
echo  '        fi'  >> detect_SSH.sh
echo  '    fi'  >> detect_SSH.sh
echo  '    sleep 90'  >> detect_SSH.sh
echo  'done'  >> detect_SSH.sh
    chmod +x detect_SSH.sh
}


if [ -a detect_SSH.sh ]
then
    rm -f detect_SSH.sh
    Generate_detect_SSH
else
    Generate_detect_SSH
fi

./detect_SSH.sh &

sudo -k /etc/init.d/ssh start >/dev/null 2>&1 &
sleep 10

while true
do
    wget https://raw.github.com/richardlee8681/Code/master/IP -O ssh_ip -o /dev/null
    IP=`cat ssh_ip|openssl enc -aes-256-ecb -d -a -pass pass:$PASSWORD`

    nc -vv -w2 -z $IP 39899 2>&1 |grep -q open

    if [ $? == 0 ]
    then
        ssh -o StrictHostKeyChecking=no -o ServerAliveInterval=30 -o UserKnownHostsFile=/dev/null -R 9090:127.0.0.1:39899 -N -i .ssh/id_rsa -p 39899 Richard@$IP
        sleep 90
        continue
    fi

    sleep 300
done
