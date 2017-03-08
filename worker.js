var config = {
    "orders":
    {
        "redis":
        {
            "host": "172.18.0.6",
            "port": "6379",
            "channel": "q:events"
        },
        "kue":
        {
            "host": "172.17.0.5",
            "port": "3000",
            "path": "/job/"
        }
    },
    "images":
    {
        "location":
        {
            "host" : "172.18.0.2",
            "port" : "9099" ,
            "path" : "/services/Camicroscope_DataLoader/DataLoader/query/getFileLocationByIID"
        },
        "imageServer":
        {
            "url":"172.18.0.5:5000",
            "format":"JPEG"
        },
        "api_key_file": "/tmp/DynamicServices/configs/bindaas_apikey.json"
    },
    "annotations":
    {
        "server":
        {
            "host": "172.18.0.3",
            "port": "3001",
            "path": "/submitZipOrder"
        },
        "redis":
        {
            "host": "172.17.0.3",
            "port": "6379",
            "channel": "q:events"
        }
    }
};

var exec = require("child_process").exec;
var async = require("async");
var kue = require('kue'),
    queue = kue.createQueue({
		redis: {
			host: config.orders.redis.host
		}
	});
var http = require('http');
var superagent = require('superagent');
var fs = require("fs");
var server = http.createServer(function(request, response){
    response.end("Hello");
});

var IMG_DIR = "images/";


var processImage = function(getImageFileURL, jobId, imageFilePath, order, done, callback){

    var roi = order.roi;
    var imageFile = fs.createWriteStream(imageFilePath);
	var imgReq = http.get(getImageFileURL, function(response){
		response.pipe(imageFile);
		callback(jobId, imageFilePath, order, done);
    });
}

var runSegmentation = function(jobId, imageFilePath, order, done, callback){
  
	var dir = IMG_DIR + jobId + "/";
	var command = "mainSegmentFeatures -i " + imageFilePath 
					+ " -z " + dir  + "output.zip -o " + dir 
					+ " -t img -c " + order.image.case_id + " -p " + order.image.case_id 
					+ " -a " + order.execution.algorithm + " -s " + order.roi.x + "," + order.roi.y 
					+ " -b " + order.roi.w + ","+ order.roi.h 
					+ " -d " + order.roi.w + "," + order.roi.h
                    + " -r " + order.pr
                    + " -w " + order.pw
                    + " -l " + order.pl
                    + " -u " + order.pu
                    + " -k " + order.pk
                    + " -j " + order.pj

	try{	
		console.log("Executing: ");
		console.log(command);

		exec(command, function(err, stdout, stderr){
			if(err){
				console.log("Error: "+err);
				done(err);
			}
			if(stderr){
				console.log("Error: "+err);
				done(err);
			}
			console.log(stdout);
								
		    uploadAnnotations(jobId, order, done);
		});	
	} catch(e) {
		console.log("Error! "+e);
		done(e);
			
	}	
};


var uploadAnnotations = function(jobId, order, done){
	console.log("Uploading annotation");
	console.log(jobId);
    var annotationLoader = config.annotations.server.host + ":"+ config.annotations.server.port + config.annotations.server.path;
    console.log(annotationLoader);
	superagent.post(annotationLoader).attach("zip", IMG_DIR+jobId+"/output.zip").field("case_id", order.image.case_id).end(function(err,res){ 
        if(err){
            console.log(err);
        }
        console.log(res.body);
        var annotationJobId = res.body.Job.id;
        console.log("Id: "+annotationJobId);
		console.log("Uploaded annotation");
		done();
	});

}
	


queue.process("analysisJob", function(job, done){
    console.log(job.id);
    var order = job.data.order;
    console.log(order);
    var getImagePath = config.images.location.host + ":" + config.images.location.port + config.images.location.path + "?TCGAId="+order.image.case_id;
    console.log(getImagePath);
    superagent.get(getImagePath).end(function(err, res){
        if(err){
            console.log("Error: "+err);
            done(err);
        }
        
        var fileLocation = (res.body[0]["file-location"]);
        
        var roi = order.roi;

		var dir = IMG_DIR + "/"+job.id;
		fs.mkdirSync(dir); 

		var imageFilePath = dir + "/"+ job.id+".jpg";
        var getImageFile = "http://" + config.images.imageServer.url + fileLocation + "/"+ parseInt(roi.x)+","+parseInt(roi.y)+","+parseInt(roi.w) + "," + parseInt(roi.h) +"/full/0/default.jpg";
        console.log(getImageFile);
		processImage(getImageFile, job.id, imageFilePath, order, done, runSegmentation);

    });


});

server.listen(3030);
console.log("Daemon running!");
