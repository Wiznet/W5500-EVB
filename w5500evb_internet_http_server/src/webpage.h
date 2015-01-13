#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

extern uint8_t index_page_array[];

#define wiz550web_ajax_js 	"function AJAX(a,e){var c=d();c.onreadystatechange=b;function d(){if(window.XMLHttpRequest){return new XMLHttpRequest()}else{if(window.ActiveXObject){return new ActiveXObject(\"Microsoft.XMLHTTP\")}}}function b(){if(c.readyState==4){if(c.status==200){if(e){e(c.responseText)}}}}this.doGet=function(){c.open(\"GET\",a,true);c.send(null)};this.doPost=function(f){c.open(\"POST\",a,true);c.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\");c.setRequestHeader(\"ISAJAX\",\"yes\");c.send(f)}}function $(a){return document.getElementById(a)}function $$(a){return document.getElementsByName(a)}function $$_ie(a,c){if(!a){a=\"*\"}var b=document.getElementsByTagName(a);var e=[];for(var d=0;d<b.length;d++){att=b[d].getAttribute(\"name\");if(att==c){e.push(b[d])}}return e}"

/* Get: Network Information: function NetinfoCallback(o), getNetinfo() */
#define wiz550web_netinfo_js	"function NetinfoCallback(o){"\
									 "$('txtmac').value=o.mac;"\
									 "$('txtip').value=o.ip;"\
									 "$('txtgw').value=o.gw;"\
									 "$('txtsn').value=o.sn;"\
									 "$('txtdns').value=o.dns;"\
									 "if(typeof(window.external)!='undefined'){"\
										"obj=$$_ie('input','dhcp');"\
									 "}else{"\
										"obj=$$('dhcp');"\
									 "}"\
								"}"\
								" "\
								"function getNetinfo(){"\
									 "var oUpdate;"\
									 "setTimeout(function(){"\
									 	"oUpdate=new AJAX('get_netinfo.cgi',function(t){"\
									 		"try{eval(t);}catch(e){alert(e);}"\
									 "});"\
									 "oUpdate.doGet();},300);"\
								 "}"

/* Get: Digital I/O state/dir:  function DioCallback(o), getDio(o) */
/* Set: Digital I/O state: 		function setDiostate(o) */
/* Set: Digital I/O direction: 	function setDiodir(o) */
#define wiz550web_dio_js	"function DioCallback(o){"\
								"var pin = o.dio_p;"\
								"$('txtdio_s'+pin).value=o.dio_s;"\
								"$('txtdio_d'+pin).value=o.dio_d;"\
							"}"\
							"function getDio(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"oUpdate=new AJAX('get_dio'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();"\
							"}"\
							" "\
							"function setDiostate(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_s'+p).value;*/"\
								"var v=o.attributes['s'].value;"\
								"dout=new AJAX('set_diostate.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"\
							" "\
							"function setDiodir(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_d'+p).value;*/"\
								"var v=o.attributes['d'].value;"\
								"dout=new AJAX('set_diodir.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"

/* Get: Analog Input: 	function AinCallback(o), getAin(o) */
/* Additional function: function AinDrawgraph(o), getAin6_update() */
#define wiz550web_ain_js	"function AinCallback(o){"\
								"var pin = o.ain_p;"\
								"$('txtain_v'+pin).value=o.ain_v;"\
								"AinDrawgraph(o);"\
							"}"\
							"function getAin(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
							"}"\
							"function AinDrawgraph(o){"\
								"var pin=o.ain_p;"\
								"var val=o.ain_v;"\
								"$('ain_v'+pin).style.width=val*500/1023+'px';"\
							"}"\
							"function getAin6_update() {"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain6.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
								"setTimeout('getAin6_update()',500);"\
							"}"


#define ain_gauge_js		"google.load('visualization', '1', {packages:['gauge']});"\
							"google.setOnLoadCallback(AinDrawGoogleGauge);"\
							"function AinCallback(o){"\
								"var pin = o.ain_p;"\
								"$('txtain_v'+pin).value=o.ain_v;"\
								"AinDrawGoogleGauge(o);"\
							"}"\
							"function getAin(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
							"}"\
							"function AinDrawGoogleGauge(o){"\
								"var val=o.ain_v;"\
								"var temp_val=Number(((((val*3300)/1023)-500)/10).toFixed(2));"\
								"var data = google.visualization.arrayToDataTable(["\
									"['Label', 'Value'],"\
									"['Temp.', 80]"\
								"]);"\
								"var options = {"\
									"width:400, height:120,"\
									"max:100, min:-40,"\
									"greenFrom:-40, greenTo:0,"\
									"redFrom:90, redTo:100,"\
									"yellowFrom:75, yellowTo:90,"\
									"minorTicks: 5"\
								"};"\
								"var chart = new google.visualization.Gauge(document.getElementById('chart_div'));"\
								"data.setValue(0, 1, temp_val);"\
								"chart.draw(data, options);"\
							"}"\
							"function getAin6_update() {"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain6.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
								"setTimeout('getAin6_update()',500);"\
							"}"



#define netinfo_page 		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Network Info</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<style>"\
								"label{float:left;text-align:left;width:50px;}"\
								"li {list-style:none;}"\
								"</style>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='netinfo.js'></script>"\
								"</head>"\
								"<body onload='getNetinfo();'>"\
									"<div>"\
									"W5500-EVB Web Server Network Information"\
									"</div>"\
									"<br>"\
									"<ul>"\
										"<li><label for='txtmac'>MAC:</label><input id='txtmac' name='mac' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtip'>IP:</label><input id='txtip' name='ip' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtgw'>GW:</label><input id='txtgw' name='gw' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtsn'>SN:</label><input id='txtsn' name='sn' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtdns'>DNS:</label><input id='txtdns' name='dns' type='text' size='20' disabled='disabled'/></li> "\
									"</ul>"\
								"</body>"\
							"</html>"


#define dio_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Digital I/O</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='dio.js'></script>"\
								"</head>"\
								"<body>"\
									"<!-- to do -->"\
									"<div>"\
									"<input type='button' value='LED R On' pin='8' s='0' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED R Off' pin='8' s= '1' onclick='setDiostate(this);'>"\
									"<br>"\
									"<input type='button' value='LED B On' pin='9' s='0' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED B Off' pin='9' s= '1' onclick='setDiostate(this);'>"\
									"<br>"\
									"<input type='button' value='LED G On' pin='10' s='0' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED G Off' pin='10' s= '1' onclick='setDiostate(this);'>"\
									"</div>"\
									"<!-- to do -->"\
								"</body>"\
							"</html>"


#define ain_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Analog Input</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
 	 	 	 	 	 	 	 	"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='ain.js'></script>"\
								"<style>"\
									".analog{margin-top:2px;margin-right:10px;border:1px solid #ccc;height:20px;width:500px;display:block;}"\
									".ain{width:0%;height:100%;text-align:center;background:red;float:left;display:block;}"\
								"</style>"\
								"</head>"\
								"<body>"\
									"<!-- to do -->"\
									"<div>"\
										"<input type='text' id='txtain_v6' size='5' disabled='disabled' value=''> "\
										"<input type='button' value='Get AIN' pin='6' onclick='getAin(this);'> "\
										"<input type='button' value='Get AIN Auto' onclick='getAin6_update();'> "\
										"<br>"\
										"<div class='analog' style='padding:0px;'><strong id='ain_v6' name='ain' class='ain' style='width:0%;'></strong></div>"\
									"</div>"\
									"<!-- to do -->"\
								"</body>"\
							"</html>"



#define ain_gauge_page		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Analog Input Gauge</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
 	 	 	 	 	 	 	 	"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='https://www.google.com/jsapi'></script>"\
								"<script type='text/javascript' src='ain_gauge.js'></script>"\
								"</head>"\
								"<body>"\
									"<div>"\
										"<input type='text' id='txtain_v6' size='5' disabled='disabled' value=''> "\
										"<input type='button' value='Get AIN' pin='6' onclick='getAin(this);'> "\
										"<input type='button' value='Get AIN Auto' onclick='getAin6_update();'> "\
										"<br>"\
										"<div class='analog' style='padding:0px;'><strong id='ain_v6' name='ain' class='ain' style='width:0%;'></strong></div>"\
										"<br>"\
										"<!--Draw the Google Gauge Chart-->"\
										"<div id='chart_div' style='width: 400px; height: 120px;'></div>"\
									"</div>"\
								"</body>"\
							"</html>"


#endif


/*Original Chart Example*/
/*
  "<script type='text/javascript' src='https://www.google.com/jsapi?autoload={'modules':[{'name':'visualization','version':'1','packages':['gauge']}]}'></script>"\
*/

/*
#define ain_gauge_js		"google.setOnLoadCallback(drawChart);"\
							"function drawChart() {"\
								"var data = google.visualization.arrayToDataTable(["\
									"['Label', 'Value'],"\
									"['Temp.', 80]"\
								"]);"\
								"var options = {"\
									"width:400, height:120,"\
									"max:100, min:-40,"\
									"greenFrom:-40, greenTo:0,"\
									"redFrom:90, redTo:100,"\
									"yellowFrom:75, yellowTo:90,"\
									"minorTicks: 5"\
								"};"\
								"var chart = new google.visualization.Gauge(document.getElementById('chart_div'));"\
								"chart.draw(data, options);"\
								"setInterval(function() {"\
									"data.setValue(0, 1, 32.1);"\
									"chart.draw(data, options);"\
									"}, 1000);"\
							"}"
*/

