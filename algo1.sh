#!bin/bash
echo $$
DIR=`pwd`
CONTAINER_ID=`docker run -v $DIR:/data/input -v $DIR:/data/output -d tmkurc/nucleus_segmentation mainSegmentFeatures -t img -i /data/input/$1 -p /data/output/$2 -v mask:img:overlay`
echo "Container_ID: "$CONTAINER_ID
#docker events --filter 'container=$contid' --filter 'event=die'
while `docker inspect -f {{.State.Running}} $CONTAINER_ID`
do
    sleep 1
done
echo "Done processing "$1
docker rm $CONTAINER_ID
