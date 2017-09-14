var config = require('./config.json'),
    username = config.user,
    apikey = config.apikey,
    tokens = config.tokens,
    Plotly = require('../.')(username, apikey),
    Signal = require('random-signal');
	
var net = require('net');
var HOST = '127.0.0.1';
var PORT = 8589;
var i = 0;

function initTrace(i) {
    return {
        x: [],  // init. data arrays
        y: [],
        type: 'scatter',
        mode: 'lines+markers',
        stream: {
            "token": tokens[i],
            "maxpoints": 300
        }
    };
}

var t_data = [0, 1, 2, 3, 4].map(initTrace);

var layout = {
    filename: "Need-A-Hand-Data",
    fileopt: "overwrite",
    layout: {
      title: "Need-A-Hand Real Time Data"
    },
    world_readable: true
};

Plotly.plot(t_data, layout, function (err, resp) {
    if (err) return console.log("ERROR", err, t_data);
    console.log(resp);

net.createServer(function(sock) {
    
    // We have a connection - a socket object is assigned to the connection automatically
   // console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
    
    // Add a 'data' event handler to this instance of socket
    sock.on('data', function(data) {
		var str = String(data);
		var arr = str.split(",").map(function (val) {
		  return Number(val) + 1;
		});
		var stream0 = Plotly.stream(tokens[0], function(){});
		var stream1 = Plotly.stream(tokens[1], function(){});
		var stream2 = Plotly.stream(tokens[2], function(){});
		var stream3 = Plotly.stream(tokens[3], function(){});
		var stream4 = Plotly.stream(tokens[4], function(){});

		var data0 = { x : i, y : arr[0]};
		var data1 = { x : i, y : arr[1]};
		var data2 = { x : i, y : arr[2]};
		var data3 = { x : i, y : arr[3]};
		var data4 = { x : i, y : arr[4]};
		
		var streamObject0 = JSON.stringify(data0);
		var streamObject1 = JSON.stringify(data1);
		var streamObject2 = JSON.stringify(data2);
		var streamObject3 = JSON.stringify(data3);
		var streamObject4 = JSON.stringify(data4);
		
		stream0.write(streamObject0+'\n');
        stream1.write(streamObject1+'\n');
		stream2.write(streamObject2+'\n');
		stream3.write(streamObject3+'\n');
		stream4.write(streamObject4+'\n');
		i++;
    });
    
    // Add a 'close' event handler to this instance of socket
    sock.on('close', function(data) {
        console.log('CLOSED: ' + sock.remoteAddress +' '+ sock.remotePort);
    });
    
}).listen(PORT, HOST);

console.log('Server listening on ' + HOST +':'+ PORT);
});
