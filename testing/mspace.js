

	function parseplot(data){
		var motionblock = data.split(/=== [:a-z0-9]+==[a-zA-Z]+\|[0-9]+/);
		motionblock.shift();
		
		for(var i in motionblock){
			var sample = motionblock[i];
			var j = sample.split("\n");
			j.shift();
			delete j[j.length -1]
			motionblock[i] = j;
			for(var k in j){
				//for each line in motionblock (each of 50ish sample)
				var text=  j[k];
				var atx = text.split(",");
				var prc = {
					'mac': atx[0],
					'ssid': atx[1],
					'dBm': atx[2],
					'normalized': atx[3]
				}
				j[k] = prc;
			}
			
		}

		console.log(motionblock);
		formatlab(motionblock);

	}

	function average(sample){
		var sum = 0;
		for(var i in sample){
			sum += parseInt(sample[i].normalized);
		}
		return sum/sample.length;
	}

	function formatlab(mblock){
		for(var j = 0; j < mblock[0].length - 1; j++){
			var otext = "";
			for(var groupbin in mblock){
				otext += (mblock[groupbin][j].normalized + "\n");
			}
			fs.writeFile(process.argv[2] + "_processed_" +  j + ".csv", otext);
		}
	}

	var fs = require('fs')
	filename = process.argv[2];

	fs.readFile(filename, 'utf8', function(err, data) {
	  if (err) throw err;
	  parseplot(data);
	});	
