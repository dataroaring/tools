mysql -h127.1 -P11030 -uroot -e 'show backends\G;' | grep Host | awk '{print $2}' >be_ips.txt
