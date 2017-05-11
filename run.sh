apikey=$(python createUser.py dynamic@quip)

sed -i -e "s/ffffffff-ffff-ffff-ffff-ffffffffffff/$apikey/g" /tmp/DynamicServices/configs/sample_apikey.json

nohup /tmp/DynamicServices/./dynamicservices /tmp/DynamicServices/configs/config.json > /tmp/DynamicServices/configs/dynamicservices.log 2>&1 &
while true; do sleep 1000; done

