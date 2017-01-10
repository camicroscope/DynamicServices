#!bin/bash
INPUTDIR=`realpath $1`
OUTPUTDIR=`realpath $2`
INPUTTYPE=$3
INPUTNAME=$4
OUTPUTNAME=$5
echo $$
echo num_arg:$#
if [[ $# -eq 5 ]] 
then
	CONTAINER_ID=`docker run -v $INPUTDIR:/data/input -v $OUTPUTDIR:/data/output -d tmkurc/nucleus_segmentation mainSegmentFeatures -t $INPUTTYPE -i /data/input/$INPUTNAME -p /data/output/$OUTPUTNAME -v mask:img:overlay`
elif [[ $# -eq 9 ]] 
then
	X=$6
	Y=$7
	W=$8
	H=$9
	CONTAINER_ID=`docker run -v $INPUTDIR:/data/input -v $OUTPUTDIR:/data/output -d tmkurc/nucleus_segmentation mainSegmentFeatures -t $INPUTTYPE -i /data/input/$INPUTNAME -p /data/output/$OUTPUTNAME -s $X,$Y -b $W,$H -v mask:img:overlay`
fi

echo "Container_ID: "$CONTAINER_ID
#docker events --filter 'container=$contid' --filter 'event=die'
while `docker inspect -f {{.State.Running}} $CONTAINER_ID`
do
    sleep 1
done
echo "Done processing "$1
docker rm $CONTAINER_ID
