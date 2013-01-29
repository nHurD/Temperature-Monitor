<!DOCTYPE HTML>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<title>Highcharts Example</title>

		<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
		<script type="text/javascript">
$(function () {
    var chart;
    var data = [];
    $(document).ready(function() {
        chart = new Highcharts.Chart({
            chart: {
                renderTo: 'container',
                type: 'spline'
            },
            title: {
                text: 'Temperature Data'
            },
            subtitle: {
                text: 'Sampling every 500 items'
            },
            xAxis: {
                type: 'datetime',
                dateTimeLabelFormats: { // don't display the dummy year
                    month: '%e. %b',
                    year: '%b'
                }
            },
            yAxis: {
                title: {
                    text: 'Temperature (C)'
                },
                min: 0
            },
            tooltip: {
                formatter: function() {
                        return '<b>'+ this.series.name +'</b><br/>'+
                        Highcharts.dateFormat('%e. %b', this.x) +': '+ this.y +' m';
                }
            },
            
            series: [{
                name: 'Sensor Data',
                // Define the data points. All series have a dummy year
                // of 1970/71 in order to be compared on the same x axis. Note
                // that in JavaScript, months start at 0 for January, 1 for February etc.
                data: [ ]
            }]
        });
        
        $.getJSON ( '/TemperatureData', function ( result ) {
            
            $.each ( result.data, function (idx, item) {
                dat = Date.parse(item.time_data.replace(" ","T")) ;
                
                
                
                data.push ( new Array ( dat, result.data[idx].temperature ) );
            
             });
             
             

           

         }).complete(function() { 
            
         
            chart.series[0].setData(data);
          
         });
    });
    
});
		</script>
	</head>
	<body>
<script src="/static/js/highcharts.js"></script>
<script src="/static/js/themes/gray.js"></scipt>
<script src="/static/js/modules/exporting.js"></script>

<div id="container" style="min-width: 400px; height: 400px; margin: 0 auto"></div>

	</body>
</html>
