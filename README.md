##DynamicServices

###Overview
DynamicServices is a suit of web-services to run containerized algorithms on large bio-medical image data. It can used to run the algorithm on a region of interest (ROI) tile or on the whole image.  

###Architecture assumptions
DynamicServices pulls an image ROI from an image server based on JSON orders. The following describes the workflow and underground architecture assumptions. 

###Dependencies:
* `yum install curl-devel`
* `yum install jsoncpp-devel`
* Install redox: `https://github.com/hmartiro/redox`

###Build:
* `mkdir obj`
* `make`

###Config file
Refer to sample_config.json

###Deployment
To run use:
`LD_LIBRARY_PATH=/usr/local/lib64/ ./analyticalservices config.json`
To deploy use:
`nohup LD_LIBRARY_PATH=/usr/local/lib64/ ./analyticalservices config.json`

###Orders
Refer to sample_order.json
