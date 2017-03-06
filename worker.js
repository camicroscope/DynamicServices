var kue = require('kue'),
    queue = kue.createQueue();
var http = require('http');
var superagent = require('superagent');
var fs = require("fs");
var server = http.createServer(function(request, response){
    response.end("Hello");
});

var config = {
    "orders":
    {
        "redis":
        {
            "host": "172.17.0.5",
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
            "host": "172.17.0.3",
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


queue.process("analysisJob", function(job, done){
    console.log(job.id);
    var order = job.data.order;

    console.log(order);


    //http.get("")
    //get image path
    //var getImagePath = order.images.host + ":" + order.images.port + order.images.path;
    var getImagePath = config.images.location.host + ":" + config.images.location.port + config.images.location.path + "?TCGAId="+order.image.case_id;
    console.log(getImagePath);
    superagent.get(getImagePath).end(function(err, res){
        if(err){
            console.log("Error: "+err);
            done(err);
        }
        
        var fileLocation = (res.body[0]["file-location"]);
        
        var roi = order.roi;
        var imageFile = fs.createWriteStream(job.id+".jpg");


        //ttp://localhost/data/myImages/imageDir2/image1.svs/8000,9000,400,400/full/0/default.jp
        var getImageFile = "http://"+config.images.imageServer.url +fileLocation + "/"+ parseInt(roi.x)+","+parseInt(roi.y)+","+parseInt(roi.w) + "," + parseInt(roi.h) +"/full/0/default.jpg";
        console.log(getImageFile);
        var imgReq = http.get(getImageFile, function(response){
            response.pipe(imageFile);


            

            done();
        });


        /*
        superagent.get(getImageFile).end(function(err2, res2){
            if(err2){
                console.log("Error: "+err2);
                done(err2);
            }
            console.log(res2.statusCode)
            done(); 
        });
        */
    });
    //Fetch image

    //Run container
    //Upload annotations



});

server.listen(3030);
console.log("Daemon running!");
