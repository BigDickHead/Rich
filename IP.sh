#!/bin/bash
while true
do
wget http://iframe.ip138.com/ic.asp -O /tmp/ip
sendemail -t somebody@gmail.com -f somebody@gmail.com -s smtp.gmail.com -u `egrep "[0-9]{1,3}\." /tmp/ip |awk -F "[" '{print $2}'|awk -F "]" '{print $1}'` -a /tmp/ip -m `cat /tmp/ip` -xu somebody -xp password
sleep 600
done


#========================================================================
#!/bin/bash

while true
do
wget https://raw.github.com/richardlee8681/Code/master/IP -O ssh_ip
IP=`cat ssh_ip|openssl enc -aes-256-ecb -d -a -pass pass:PASSWORD`
nc -vv -w2 -z $IP 39899 2>&1 |grep -q open

if [ $? == 0 ]
then
   ssh -o StrictHostKeyChecking=no -R 9090:127.0.0.1:39899 -N -i .ssh/id_rsa -p 39899 User@$IP
fi

sleep 600
done
