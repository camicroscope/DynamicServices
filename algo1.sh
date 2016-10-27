#!bin/bash
echo $$
DIR=`pwd`
CONTAINER_ID=`docker run -v $DIR:/data/input -v $DIR:/data/output -d tmkurc/nucleus_segmentation mainSegmentFeatures -t $1 -i /data/input/$2 -p /data/output/$3 -v mask:img:overlay`
echo "Container_ID: "$CONTAINER_ID
#docker events --filter 'container=$contid' --filter 'event=die'
while `docker inspect -f {{.State.Running}} $CONTAINER_ID`
do
    sleep 1
done
echo "Done processing "$1
docker rm $CONTAINER_ID
