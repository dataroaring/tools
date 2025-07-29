grep "3 replicas write data failed" after_restart_fe.log | grep "publish" | awk -F 'on tablet' '{print $2}' | awk '{print $1 " "$4}' | awk -F ',' '{print $1}' | sort | uniq >padding.txt
