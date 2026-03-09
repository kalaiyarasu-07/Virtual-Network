#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const char* ssid = "INZORA";

WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;

String people = "";
String landmark = "";
String message = "";

// ---------- CSS ----------
String css() {
  return "<style>"
  "body{margin:0;font-family:Arial;background:#0b0b0b;color:white;text-align:center;}"
  ".box{background:#111;padding:20px;margin:20px auto;width:90%;max-width:500px;border-radius:10px;}"
  "input,textarea{width:90%;padding:10px;margin:5px;background:#222;color:white;border:none;}"
  "button{padding:10px 20px;background:red;color:white;border:none;font-size:18px;cursor:pointer;margin:5px;}"
  ".title{font-size:26px;margin-top:15px;}"
  "</style>";
}

// ---------- PEOPLE PAGE ----------
void peoplePage() {

  String html = "<html><head>";
  html += css();
  html += "</head><body>";

  html += "<div class='title'>SEND RESCUE INFO</div>";
  html += "<div class='box'>";
  html += "<form action='/send' method='POST'>";
  html += "People Count<br><input name='p'>";
  html += "Landmark<br><input name='l'>";
  html += "Message<br><textarea name='m'></textarea>";
  html += "<br><button>SEND</button>";
  html += "</form>";
  html += "</div></body></html>";

  server.send(200,"text/html",html);
}

// ---------- SAVE ----------
void saveData() {
  if(server.hasArg("p")) people = server.arg("p");
  if(server.hasArg("l")) landmark = server.arg("l");
  if(server.hasArg("m")) message = server.arg("m");

  server.sendHeader("Location","/people");
  server.send(303);
}

// ---------- RESCUE DASHBOARD ----------
void rescuePage() {

  String html = "<html><head>";
  html += css();
  html += "<script>setTimeout(()=>{location.reload();},3000);</script>";
  html += "</head><body>";

  html += "<div class='title'>RESCUE TEAM DASHBOARD</div>";
  html += "<div class='box'>";
  html += "<h3>People: " + people + "</h3>";
  html += "<h3>Landmark: " + landmark + "</h3>";
  html += "<h3>Message: " + message + "</h3>";
  html += "<h4>ESP IP: " + WiFi.softAPIP().toString() + "</h4>";
  html += "</div></body></html>";

  server.send(200,"text/html",html);
}

// ---------- ADMIN DASHBOARD ----------
void adminPage() {

  String html = "<html><head>";
  html += css();
  html += "</head><body>";

  html += "<div class='title'>ADMIN DASHBOARD</div>";
  html += "<div class='box'>";
  html += "<h3>Current Data</h3>";
  html += "People: " + people + "<br>";
  html += "Landmark: " + landmark + "<br>";
  html += "Message: " + message + "<br><br>";

  html += "<a href='/clear'><button>Clear Data</button></a>";
  html += "</div></body></html>";

  server.send(200,"text/html",html);
}

// ---------- CLEAR ----------
void clearData() {
  people = "";
  landmark = "";
  message = "";
  server.sendHeader("Location","/admin");
  server.send(303);
}

// ---------- ROOT ----------
void root() {
  server.sendHeader("Location","/people");
  server.send(302);
}

// ---------- SETUP ----------
void setup() {

  Serial.begin(115200);

  WiFi.disconnect(true,true);
  delay(1000);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid, NULL, 1, 0, 4);

  Serial.println("AP Started");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  server.on("/", root);
  server.on("/people", peoplePage);
  server.on("/send", HTTP_POST, saveData);
  server.on("/rescue", rescuePage);
  server.on("/admin", adminPage);   // <-- THIS WAS MISSING
  server.on("/clear", clearData);   // <-- ALSO ADDED

  server.on("/generate_204", root);
  server.on("/fwlink", root);
  server.on("/hotspot-detect.html", root);

  server.begin();
}

// ---------- LOOP ----------
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}