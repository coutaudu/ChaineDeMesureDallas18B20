/* Copyright (c) 2012: Daniel Richman. License: GNU GPL 3 */
/* Additional features: Priyesh Patel                     */


function readTextFile(file){
    var rawFile = new XMLHttpRequest();
    var allText;
    rawFile.open("GET", file, false);
    rawFile.onreadystatechange = function () {
        if(rawFile.readyState === 4) {
            if(rawFile.status === 200 || rawFile.status == 0) {
                allText = rawFile.responseText;
		// var allLines = allText.split(/\n/g);
		// var fileLength = allLines.length;
		// var lastLines = allLines.slice(fileLength-10,fileLength-1).join('\n');
		// $("#readFile").text(lastLines);
		//                alert(allText);
            }
        }
    }
    rawFile.send(null);
    return (allText);
}

var nbPoints = 100;
var dataPointsA = [];


function changeNbPoints(val){
    $("#DEBUG").text("Button clicked");
    nbPoints = Math.max((nbPoints+val),10);
    // if (nbPoints==10){
    // 	nbPoints=100;
    // }else if(nbPoints==100){
    // 	nbPoints=1000;
    // }else if(nbPoints==1000){
    // 	nbPoints=10;
    // }
    dataPointsA.length = 0;
    $("#DEBUG").text(nbPoints);    

}
    

(function () {

    var dataelem = "#data";
    var pausetoggle = "#pause";
    var scrollelems = ["html", "body"];

    var url = "./data.csv";
    var fix_rn = true;
    var load = 30 * 1024; /* 30KB */
    var poll = 1000; /* 1s */
    
    var kill = false;
    var loading = false;
    var pause = false;
    var reverse = true;
    var log_data = "";
    var log_file_size = 0;



    window.onload = function () {
	var chart;
        function getDataPointsFromCSV(file) {
	    csv = readTextFile(file);
            csvLines = csv.split(/[\r?\n|\r|\n]+/).slice(-(nbPoints+1));         
            for (var i = 0; i < csvLines.length; i++)
                if (csvLines[i].length > 0) {
                    points = csvLines[i].split(",");
		    var curTS = new Date(points[0]);
		    if(dataPointsA.length==0 || dataPointsA[dataPointsA.length-1].x<curTS) {
			dataPointsA.push({ 
                            x: curTS, 
                            y: parseFloat(points[6])
			});
			while (dataPointsA.length >  nbPoints ) { dataPointsA.shift(); }
		    }
                }
            return dataPointsA;
        }


	getDataPointsFromCSV(url);
	chart = new CanvasJS.Chart("chartContainer",{
      	    title :{ text: "Is it hot or is it not hot ? Let's find out !"},
      	    axisX: { valueFormatString: "YYYY-MM-DD'T'HH:mm:ss" ,
		     labelAngle: -50  },
      	    axisY: { title: "C"+String.fromCharCode(176),
		     includeZero: true},
	    toolTip: {content:"{x} {y}C"+String.fromCharCode(176)},
      	    data: [{
		type: "scatter",
		color: "red",
		xValueFormatString: "HH:mm:ss",
		dataPoints : dataPointsA
	    },
		   {
		type: "line",
		color: "green",
		xValueFormatString: "HH:mm:ss",
		dataPoints : dataPointsA
	    }]
	});				   
	
	chart.render();
	setTimeout(function(){updateChart()}, poll);
	
    /////////:
    function updateChart() {
	getDataPointsFromCSV(url);
	chart.render();
	setTimeout(function(){updateChart()}, poll);
    }
    /////////:
}

 

 
 /* :-( https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/parseInt */
 function parseInt2(value) {
     if(!(/^[0-9]+$/.test(value))) throw "Invalid integer " + value;
     var v = Number(value);
     if (isNaN(v))                 throw "Invalid integer " + value;
     return v;
 }

 function get_log() {
     if (kill | loading) return;
     loading = true;

     var range;
     var first_load;
     var must_get_206;
     if (log_file_size === 0) {
         /* Get the last 'load' bytes */
         range = "-" + load.toString();
         first_load = true;
         must_get_206 = false;
     } else {
         /* Get the (log_file_size - 1)th byte, onwards. */
         range = (log_file_size - 1).toString() + "-";
         first_load = false;
         must_get_206 = log_file_size > 1;
     }

     /* The "log_file_size - 1" deliberately reloads the last byte, which we already
      * have. This is to prevent a 416 "Range unsatisfiable" error: a response
      * of length 1 tells us that the file hasn't changed yet. A 416 shows that
      * the file has been trucnated */

     $.ajax(url, {
         dataType: "text",
         cache: false,
         headers: {Range: "bytes=" + range},
         success: function (data, s, xhr) {
	     loading = false;

	     var content_size;

	     if (xhr.status === 206) {
                 var c_r = xhr.getResponseHeader("Content-Range");
                 if (!c_r)
		     throw "Server did not respond with a Content-Range";

                 log_file_size = parseInt2(c_r.split("/")[1]);
                 content_size = parseInt2(xhr.getResponseHeader("Content-Length"));
	     } else if (xhr.status === 200) {
                 if (must_get_206)
		     throw "Expected 206 Partial Content";

                 content_size = log_file_size =
                     parseInt2(xhr.getResponseHeader("Content-Length"));
	     } else {
                 throw "Unexpected status " + xhr.status;
	     }

	     if (first_load && data.length > load)
                 throw "Server's response was too long";

	     var added = false;

	     if (first_load) {
                 /* Clip leading part-line if not the whole file */
                 if (content_size < log_file_size) {
		     var start = data.indexOf("\n");
		     log_data = data.substring(start + 1);
                 } else {
		     log_data = data;
                 }

                 added = true;
	     } else {
                 /* Drop the first byte (see above) */
                 log_data += data.substring(1);

                 if (log_data.length > load) {
		     var start = log_data.indexOf("\n", log_data.length - load);
		     log_data = log_data.substring(start + 1);
                 }

                 if (data.length > 1)
		     added = true;
	     }

	     if (added)
                 show_log(added);
	     setTimeout(get_log, poll);
         },
         error: function (xhr, s, t) {
	     loading = false;

	     if (xhr.status === 416 || xhr.status == 404) {
                 /* 416: Requested range not satisfiable: log was truncated. */
                 /* 404: Retry soon, I guess */

                 log_file_size = 0;
                 log_data = "";
                 show_log();

                 setTimeout(get_log, poll);
	     } else {
                 throw "Unknown AJAX Error (status " + xhr.status + ")";
	     }
         }
     });
 }

 function scroll(where) {
     for (var i = 0; i < scrollelems.length; i++) {
         var s = $(scrollelems[i]);
         if (where === -1)
	     s.scrollTop(s.height());
         else
	     s.scrollTop(where);
     }
 }

 function show_log() {
     if (pause) return;

     var t = log_data;

     if (reverse) {
         var t_a = t.split(/\n/g).slice(-10);
         t_a.reverse();
         if (t_a[0] == "") 
	     t_a.shift();
         t = t_a.join("\n");
     }

     if (fix_rn)
         t = t.replace(/\n/g, "\r\n");
     
     $(dataelem).text(t);
     if (!reverse)
         scroll(-1);
 }

 function error(what) {
     kill = true;

     $(dataelem).text("An error occured :-(.\r\n" +
		      "Reloading may help; no promises.\r\n" + 
		      what);
     scroll(0);

     return false;
 }

 $(document).ready(function () {
     window.onerror = error;

     /* If URL is /logtail/?noreverse display in chronological order */
     var hash = location.search.replace(/^\?/, "");
     if (hash == "noreverse")
         reverse = false;

     /* Add pause toggle */
     $(pausetoggle).click(function (e) {
         pause = !pause;
         $(pausetoggle).text(pause ? "Unpause" : "Pause");
         show_log();
         e.preventDefault();
     });

     
     readTextFile(url);	
     get_log();
 });

})();
