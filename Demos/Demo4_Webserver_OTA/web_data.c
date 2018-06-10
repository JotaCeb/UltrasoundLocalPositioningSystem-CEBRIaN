const char headerPage[]={
"HTTP/1.1 200 OK\r\n\
Server: MySocket Server\r\n\
Date: TEST\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
Connection: close\r\n\
Accept-Ranges: bytes\r\n\r\n"
};

const char HTTPSaveResponse[] = {
"HTTP/1.1 200 OK\r\n\
Server: MySocket Server\r\n\
Date: TEST\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
Accept-Ranges: bytes\r\n\
Connection: close\r\n\r\n\
%s"
};

const char authrized[] = {
"HTTP/1.1 401 Authorization Required\r\n"
"Server: MySocket Server\r\n"
"WWW-Authenticate: Basic realm=\"MXCHIP 3162\"\r\n"
"Content-Type: text/html\r\n"
"Content-Length: 169\r\n\r\n"
"<HTML>\r\n<HEAD>\r\n<TITLE>Error</TITLE>\r\n"
"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">\r\n"
"</HEAD>\r\n<BODY><H1>401 Unauthorized.</H1></BODY>\r\n</HTML>"
};

const char not_found[] = {
"HTTP/1.1 200 OK\r\n\
Server: MySocket Server\r\n\
Content-Length: 145\r\n\
Connection: close\r\n\
Content-Type: text/html\r\n\r\n\
<html><head><title>404 Not Found</title></head><body>\r\n\
<h1>Not Found</h1>\r\n\
<p>The requested URL was not found on this server.</p>\r\n\
</body></html>"
};

/*<link href=\"css/movil.css\" rel=\"stylesheet\" media=\"screen and (max-device-width: 480px)\" width=\"480px\" />\r\n\*/
const char systemPage[] = {
"<html><head><title>System Setting</title>\r\n\
<head>\r\n\
<style type=\"text/css\">\r\n\
body{background-color:orange}\r\n\
</style>\r\n\
</head>\r\n\
<body>\r\n\
<p align=\"left\"><font size=\"3\"&nbsp</font></p>\r\n\
<p align=\"left\"><font size=\"6\" color=\"red\"><b>SISTEMA DE BALIZAMIENTO</b></font></p>\r\n\
<hr><hr>\r\n\
<p align=\"left\"><font size=\"5\" color=\"blue\"><b>PARÁMETROS INDEPENDIENTES </b></font></p>\r\n\
<p align=\"left\"><font size=\"3\"&nbsp</font></p>\r\n\
<h4><a  href=\"/transductor1.htm\">TRANS 1</a>&nbsp;&nbsp;\r\n\
<a href=\"/transductor2.htm\">TRANS 2</a>&nbsp;&nbsp;\r\n\
<a href=\"/transductor3.htm\">TRANS 3</a>&nbsp;&nbsp;\r\n\
<a href=\"/transductor4.htm\">TRANS 4</a>&nbsp;&nbsp;\r\n\
<a href=\"/transductor5.htm\">TRANS 5</a></h4>\r\n\
<hr>\r\n\
<hr>\r\n\
<p align=\"left\"><font size=\"5\" color=\"blue\"><b>PARÁMETROS COMUNES</b></font></p>\r\n\
<h5><p align=\"left\"><b>12 muestras/ciclo</b></p>\r\n\
<form action=\"paramconfig.htm\" method=\"post\">\r\n\
<p align=\"left\"><b>Selección de frecuencia de muestreo</b></p>\r\n\
<select name=\"FR\">\r\n\
<option selected value=\"40\">40 kHz</option>\r\n\
<option value=\"41\">41.66 kHz</option>\r\n\
</select>\r\n\
<p align=\"left\"><b>Selección de número de bits de la secuencia</b></p>\r\n\
<select name=\"NB\">\r\n\
<option selected value=\"255\">Kasami 255 bit</option>\r\n\
<option value=\"1023\">Kasami 1023 bit</option>\r\n\
</select></h5>\r\n\
<h5><p align=\"left\"><b>Ciclos/Simbolo&nbsp;</b><br>(De 1 a 4 para 255 bits, \r\n\
1 o 2 para 1023 bits)<br><Input type=\"number\" name=\"CS\" min=\"1\" max=\"4\" value = \"%c\"/></p>\r\n\
<p align=\"left\"><b>Periodo de emisión&nbsp;(ms)&nbsp;</b><br>(De 9 a 120) <br><Input \r\n\
type=\"number\" min=\"9\" max=\"120\" name=\"PE\" value = \"%c\"/></p>\r\n\
<p align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp\r\n\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp\r\n\
<INPUT type=\"submit\" name=\"botonokey\" value=\"OK\"></p></h5>\r\n\
</FORM>\r\n\
<hr><hr>\r\n\
</body></html>\r\n"
};
/*<link rel=\"stylesheet\" media=\"screen and (max-device-width: 480px)\" />\r\n\*/
const char Transductor1htm[]={
"<html>\r\n\
<head>\r\n\
<head>\r\n\
<style type=\"text/css\">\r\n\
body{background-color:orange}\r\n\
width=\"480px\"\r\n\
</style>\r\n\
</head>\r\n\
<title>Configurador de número de secuencias</title>\r\n\
</head>\r\n\
<body>\r\n\
<p><font size=\"5\" color=\"blue\"><b>Número de secuencia</b></font><p>\r\n\
<form action=\"paramconfig1.htm\" method=\"post\">\r\n\
<p align=\"left\"><font size=\"3\"><b>Secuencia Kasami</b><br>(De 1 a 16 para 255 bits. \r\n\
De 1 a 32 para 1023 bits)<br/></font><p>\r\n\
<p align=\"left\"><font size=\"3\"><Input type=\"number\" min=\"1\" max=\"32\" name=\"KS\" value = \r\n\
\"%c\"/></font><p>\r\n\
<p align=\"left\"><font size=\"5\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\r\n\
&nbsp<INPUT type=\"submit\" name=\"botonokey\" value=\"OK\"></font></p>\r\n\
</FORM>\r\n\
</body>\r\n\
</html>"};

const char SaveResponseError[]={
"<html>\r\n\
<head>\r\n\
<style type=\"text/css\">\r\n\
body{background-color:orange}\r\n\
</style>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<br>\r\n\
<h2>Introduzca los parámetros según las indicaciones<br><br><a href=\"/system.htm\">Regreso a la web principal</a></h2>\r\n\
</body>\r\n\
</html>"};

const char RespuestaParametros[]={
"<html>\r\n\
<head>\r\n\
<head>\r\n\
<style type=\"text/css\">\r\n\
body{background-color:orange}\r\n\
</style>\r\n\
</head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<br>\r\n\
<h2>Configuracion admitida<br><br><a href=\"/system.htm\">Regreso a la web principal</a></h2>\r\n\
</body>\r\n\
</html>"};

const char RespuestaIncorrectaParentesis[]={
"<html>\r\n\
<head>\r\n\
<head>\r\n\
<style type=\"text/css\">\r\n\
body{background-color:orange}\r\n\
</style>\r\n\
</head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<br>\r\n\
<h2>#ERROR DE SECUENCIAS:<BR>Siga las instrucciones marcadas entre paréntesis<br><br><a href=\"/system.htm\">Regreso a la web principal</a><h2>\r\n\
</body>\r\n\
</html>"};

const char RespuestaIncorrectaPeriodo[]={
"<html>\r\n\
<head>\r\n\
<head>\r\n\
<style type=\"text/css\">\r\n\
body{background-color:orange}\r\n\
</style>\r\n\
</head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<br>\r\n\
<h2>#ERROR DE PERIODO DE EMISIÓN<BR><BR>Con:<br>- T = 24 para frec. de 40 kHz <br> - T = 25 para frec. de 41.66 kHz <br><br>\r\n\
Cumpla lo siguiente:<br><br>\r\n\
Para 255 bits: <br> &nbsp;&nbsp;Periodo Emisión (ms) > (T*255*Ciclos/Simbolo)/1000 + 2*Ciclos/Simbolo\r\n\
<br><br>Para 1023 bits: <br> &nbsp;&nbsp;Periodo Emisión (ms) > (T*1023*Ciclos/Simbolo)/1000 + 4*Ciclos/Simbolo\r\n\
<br><br><a href=\"/system.htm\">Regreso a la web principal</h2>\r\n\
</body>\r\n\
</html>"};

//const char systemResponseSucc[]={
//"<html>\r\n\
//<head>\r\n\
//<style type=\"text/css\">\r\n\
//body{background-color:orange}\r\n\
//</style>\r\n\
//<title>MXCHIP Wi-Fi module</title>\r\n\
//</head>\r\n\
//<body>\r\n\
//<p>Firmware cargado satisfactoriamente. Espere 5 segundos y refresque.</p>\r\n\
//</body>\r\n\
//</html>"};

//const char systemResponseError[]={
//"<html>\r\n\
//<head>\r\n\
//<style type=\"text/css\">\r\n\
//body{background-color:orange}\r\n\
//</style>\r\n\
//<title>MXCHIP Wi-Fi module</title>\r\n\
//</head>\r\n\
//<body>\r\n\
//<p>Firmware no cargado. Espere 5 segundos y refresque.</p>\r\n\
//</body>\r\n\
//</html>"};

//const char SaveResponseSucc[]={
//"<html>\r\n\
//<head>\r\n\
//<style type=\"text/css\">\r\n\
//body{background-color:orange}\r\n\
//</style>\r\n\
//<title>MXCHIP Wi-Fi module</title>\r\n\
//</head>\r\n\
//<body>\r\n\
//<p>Configuración correcta<a href=\"/system.htm\">Retorno</a></p>\r\n\
//</body>\r\n\
//</html>"};

//const char ResponseReset[]={
//"<html>\r\n\
//<head>\r\n\
//<style type=\"text/css\">\r\n\
//body{background-color:orange}\r\n\
//</style>\r\n\
//<title>MXCHIP Wi-Fi module</title>\r\n\
//</head>\r\n\
//<body>\r\n\
//<p>Reseteando sistema, por favor, espere 5 segundos y refresque.</p>\r\n\
//</body>\r\n\
//</html>"};


