if [ -z "$QUIP_JOBS" ]; then
	sed -i 's/\@QUIP_JOBS/\$QUIP_JOBS/g' /tmp/DynamicServices/configs/config.json 
	sed -i 's/\@QUIP_OSS/\$QUIP_OSS/g' /tmp/DynamicServices/configs/config.json
	sed -i 's/\@QUIP_DATA/\$QUIP_DATA/g' /tmp/DynamicServices/configs/config.json
	sed -i 's/\@QUIP_LOADER/\$QUIP_LOADER/g' /tmp/DynamicServices/configs/config.json
fi
	
nohup /tmp/DynamicServices/./dynamicservices /tmp/DynamicServices/configs/config.json > /tmp/DynamicServices/configs/dynamicservices.log 2>&1 &
while true; do sleep 1000; done

