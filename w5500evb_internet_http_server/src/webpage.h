#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

extern uint8_t index_page_array[];

#define wiz550web_ajax_js 	"function AJAX(a,e){var c=d();c.onreadystatechange=b;function d(){if(window.XMLHttpRequest){return new XMLHttpRequest()}else{if(window.ActiveXObject){return new ActiveXObject(\"Microsoft.XMLHTTP\")}}}function b(){if(c.readyState==4){if(c.status==200){if(e){e(c.responseText)}}}}this.doGet=function(){c.open(\"GET\",a,true);c.send(null)};this.doPost=function(f){c.open(\"POST\",a,true);c.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\");c.setRequestHeader(\"ISAJAX\",\"yes\");c.send(f)}}function $(a){return document.getElementById(a)}function $$(a){return document.getElementsByName(a)}function $$_ie(a,c){if(!a){a=\"*\"}var b=document.getElementsByTagName(a);var e=[];for(var d=0;d<b.length;d++){att=b[d].getAttribute(\"name\");if(att==c){e.push(b[d])}}return e}"

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
/* Additional function: function AinDrawgraph(o) */
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
								"setTimeout('getAin6_update()',1000);"\
							"}"


#define dio_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Digital I/O</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='dio.js'></script>"\
								"</head>"\
								"<body>"\
									"<div>"\
									"<input type='button' value='LED R On' pin='10' s='0' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED R Off' pin='10' s= '1' onclick='setDiostate(this);'>"\
									"<!--to do-->"\
									"</div>"\
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
									"<div>"\
									"<!--to do-->"\
									"<br>"\
									"<div class='analog' style='padding:0px;'><strong id='ain_v6' name='ain' class='ain' style='width:0%;'></strong></div>"\
									"</div>"\
								"</body>"\
							"</html>"

#endif
