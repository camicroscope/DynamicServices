##DynamicServices

###Overview
DynamicServices is a suit of web-services to run containerized algorithms on large bio-medical image data. It can used to run the algorithm on a region of interest (ROI) tile or on the whole image.  

###Architecture assumptions
DynamicServices pulls an image ROI from an image server based on JSON orders. The following describes the workflow and underground architecture assumptions. 

###Dependencies:
* `yum install curl-devel`
* `yum install jsoncpp-devel` or use http://ask.xmodulo.com/fix-fatal-error-jsoncpp.html
* Install redox: `https://github.com/hmartiro/redox`

###Build:
* `mkdir obj`
* `make`

###Config file
Refer to sample_config.json

###Deployment
####Dependencies
DynamicServices requires following tools installed and setup

##### Camicroscope (Dependency)
* Install all components of camicroscope from (https://github.com/camicroscope/Distro)
  * In `config.json` update `annotations.server` and `annotations.redis` to IP address of Loader container.
  * Set the API key in `api_key_file` as the API key of Bindaas(from Data container)
* Install Kue(https://github.com/Automattic/kue) and redis on the host machine. 
  * In `config.json` updated `orders.kue` and `orders.redis` with host and port of kue and redis.
  * Configure camicroscope viewer in `/Configuration/api/config.php` set ordering service as this Kue.

To run use:
`LD_LIBRARY_PATH=/usr/local/lib64/ ./dynamicservices config.json`
To deploy using nohup:
`LD_LIBRARY_PATH=/usr/local/lib64/ nohup ./dynamicservices config.json > dynamicservices.log &`

##### Deployment inside an algorithm docker container
To deploy inside an algorithm docker container. Take alogorithm image as base and use the given Dockerfile to build a new image with DynamicServices installed inside it. The given Dockerfile uses `sbubmi/test_segmentation:latest` as the base image.

* Build the docker image `docker build -t dynamic_services_segmentation .`
* Create a new directory (let's call it `configs`)
* Update and place `config.json` & `api_key.json` inside `configs`
* Run the container using `docker run --net="host" -itd -v <path_to_configs>:/tmp/DynamicServices/configs dynamic_services_segmentation`
* Make sure that all the entries in config.json are updated. `"api_key_file"` should have the complete path inside the docker container.
* To check the log of DynamicServices use `tail -f configs/dynamicservices.log`

