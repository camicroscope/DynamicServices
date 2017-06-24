sleep 60
apikey=$(python createUser.py dynamic@quip)
sed -i -e "s/ffffffff-ffff-ffff-ffff-ffffffffffff/$apikey/g" /tmp/DynamicServices/sample_apikey.json


sed -i "s@camicroscope-jobs@$camicroscope-jobs@g" /tmp/DynamicServices/configs/config.json 
sed -i "s@camicroscope-oss@$camicroscope-oss@g" /tmp/DynamicServices/configs/config.json
sed -i "s@camicroscope-bindaas@$camicroscope-bindaas@g" /tmp/DynamicServices/configs/config.json
sed -i "s@camicroscope-uaimloader@$camicroscope-uaimloader@g" /tmp/DynamicServices/configs/config.json
	
nohup /tmp/DynamicServices/./dynamicservices /tmp/DynamicServices/configs/config.json > /tmp/DynamicServices/configs/dynamicservices.log 2>&1 &
while true; do sleep 1000; done

