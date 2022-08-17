/*This code was developed by Brenden Friedel for use with a 4 button QLab controller using OSC, MIDI, and Keyboard messages.*/
/******Libraries******/
//Ethernet
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
//SD Card
#include <SD.h>
//OSC
#include <OSCMessage.h>
//Display
#include <LiquidCrystal_I2C.h>
//Midi
#include <MIDIUSB.h>
//Keyboard
//#include <Keyboard.h>


#define MaxHeaderLength 150//Max charcters in Get Statement


/*****LCD*****/
LiquidCrystal_I2C lcd(0x27, 20, 4);
/*****Mode Set*****/
String UserDefinedMode = "Not Set";
/*****Button/LED Mapping******/
//LEDs
const int NextLED = 40;
const int GoLED = 42;
const int PrevLED = 43;
const int StopLED = 41;
//Buttons
const int NextButton = 30;
const int GoButton = 33;
const int PrevButton = 32;
const int StopButton = 31;
//Last Button States
int  LastGoButtonState = 0;
int LastStopButtonState = 0;
int LastNextButtonState = 0;
int LastPrevButtonState = 0;
//Number Array for OSC and Midi
int OSC[4] = {10, 11, 12, 13};
int MIDI[4] = {10, 11, 12, 13};
//Reset Counter
unsigned long ResetCounter = 0;
//*****Declorations*****/
//Indexs for return from webpage
int index1, index2, index3, index4, index5, index6, index7, index8, index9, index10, index11, index12, index13, index14, index15, index16, index17, index18, index19, index20, index21, index22, index23, index24, index25, index26;
int ind1, ind2, ind3, ind4, ind5, ind6, ind7, ind8;
String BipA, BipB, BipC, BipD;
String DipA, DipB, DipC, DipD;
String SubA, SubB, SubC, SubD;
String SDstrBIPAddress, SDstrDIPAddress, SDstrSubnetAddress, SDstrPortnum;
/******SD CARD Setup******/
File Networkinfo;
File Resetorno;
/******Network Setup******/
//Define for stingey variable
EthernetUDP Udp;
String HttpHeader = String(MaxHeaderLength);
String ShouldIReset;
//Default IP Address
uint32_t ipA = 192;
uint32_t ipB = 168;
uint32_t ipC = 1;
uint32_t ipD = 5;
//OSC Dest IP
uint32_t dipA = 192;
uint32_t dipB = 168;
uint32_t dipC = 1;
uint32_t dipD = 50;
//Default Subnet
uint32_t subnetA = 255;
uint32_t subnetB = 255;
uint32_t subnetC = 255;
uint32_t subnetD = 0;
//Port
const unsigned int destPort = 53000;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {ipA, ipB, ipC, ipD};
byte subnet[4] = {subnetA, subnetB, subnetC, subnetD};
EthernetServer server(80);//Server on port 80 (default for HTTP)
//OSC
byte destip[4] = { dipA, dipB, dipC, dipD};
byte port = destPort;
/*****Start up to Defualt IP, Subnet, and Port*****/
void ethernetDEFAULTS() {
  Serial.println("Default Loaded...");
  Ethernet.begin(mac, ip, subnet); //start with defaults, ignore ethernet reset
  server.begin();
  Udp.begin(53069);//Allow Incoming Messages
}
/*****Start up to previosuly saved ethernet data*****/
void ethernetRESET() {
  Serial.println("Reset loaded...");
  String BoardIP;
  String DestIP;
  String Subnet;
  String Port;
  Networkinfo = SD.open("Netinfo.txt", FILE_READ);
  BoardIP += Networkinfo.readStringUntil('\n');
  DestIP += Networkinfo.readStringUntil('\n');
  Subnet += Networkinfo.readStringUntil('\n');
  Port += Networkinfo.readStringUntil('\n');
  Networkinfo.close();
  //BoardIP
  ind1 = BoardIP.indexOf('.');
  BipA = BoardIP.substring(0, ind1);
  ind2 = BoardIP.indexOf('.', ind1 + 1);
  BipB = BoardIP.substring(ind1 + 1, ind2);
  ind3 = BoardIP.indexOf('.', ind2 + 1);
  BipC = BoardIP.substring(ind2 + 1, ind3);
  ind4 = BoardIP.indexOf('.', ind3 + 1);
  BipD = BoardIP.substring(ind3 + 1, ind4);
  uint32_t ipA = BipA.toInt();
  uint32_t ipB = BipB.toInt();
  uint32_t ipC = BipC.toInt();
  uint32_t ipD = BipD.toInt();
  //Subnet
  ind5 = Subnet.indexOf('.');
  SubA = Subnet.substring(0, ind5);
  ind6 = Subnet.indexOf('.', ind5 + 1);
  SubB = Subnet.substring(ind5 + 1, ind6);
  ind7 = Subnet.indexOf('.', ind6 + 1);
  SubC = Subnet.substring(ind6 + 1, ind7);
  ind8 = Subnet.indexOf('.', ind7 + 1);
  SubD = Subnet.substring(ind7 + 1, ind8);
  uint32_t subnetA = SubA.toInt();
  uint32_t subnetB = SubB.toInt();
  uint32_t subnetC = SubC.toInt();
  uint32_t subnetD = SubD.toInt();
  //Network Info
  byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  byte ip[4] = { ipA, ipB, ipC, ipD };
  byte subnet[4] = {subnetA, subnetB, subnetC, subnetD};
  Ethernet.begin(mac, ip, subnet); //start with defaults, ignore ethernet reset
  server.begin();
  Udp.begin(53069);
}
void setup() {
  //Serial
  Serial.begin(115200);//Start Serial Com
  //LCD
  lcd.init();
  lcd.backlight();
  int lcdcount = 2;//How many times to repeat startup sequence
  while (lcdcount > 0) {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Starting Up.");
    delay(500);
    lcd.setCursor(3, 0);
    lcd.print("Starting Up..");
    delay(500);
    lcd.setCursor(3, 0);
    lcd.print("Starting Up...");
    delay(500);
    lcdcount = lcdcount - 1;
  }
  //SD Card Setup
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    lcd.clear();
    lcd.print("SD Card Failed!");
    while (1);
  }
  Serial.println("initialization done.");
  lcd.setCursor(0, 1);
  lcd.print("SD Loaded");
  //IP Check
  Resetorno = SD.open("Reset.txt", FILE_READ);
  ShouldIReset += Resetorno.readStringUntil('\n');
  Serial.println(ShouldIReset);
  Resetorno.close();
  //Reset to defualt network settings or use last network settings
  if (ShouldIReset.indexOf("DEFAULT") < 0) {
    ethernetRESET();
    lcd.setCursor(0, 2);
    lcd.print("Prev Network Setting");
  }
  else {
    ethernetDEFAULTS();
    lcd.setCursor(0, 2);
    lcd.print("Def Network Settings");
  }
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());//Print Server webaddress onto Serial port.
  lcd.setCursor(0, 3);
  lcd.print(Ethernet.localIP());
  HttpHeader = "";
  //LED Pinmode
  pinMode(NextLED, OUTPUT);
  pinMode(GoLED, OUTPUT);
  pinMode(PrevLED, OUTPUT);
  pinMode(StopLED, OUTPUT);
  //Button Pinmode
  pinMode(NextButton, INPUT);
  pinMode(GoButton, INPUT);
  pinMode(PrevButton, INPUT);
  pinMode(StopButton, INPUT);
  //Startup LED Sequence
  StartupSequence();
  delay(3000);
  lcd.clear();
}
/*****Startup Sequence*****/
void StartupSequence() {
  digitalWrite(GoLED, HIGH);
  delay(200);
  digitalWrite(StopLED, HIGH);
  delay(200);
  digitalWrite(NextLED, HIGH);
  delay(200);
  digitalWrite(PrevLED, HIGH);
  delay(300);
  digitalWrite(GoLED, LOW);
  digitalWrite(StopLED, LOW);
  digitalWrite(NextLED, LOW);
  digitalWrite(PrevLED, LOW);
  delay(1000);
  digitalWrite(GoLED, HIGH);
  digitalWrite(StopLED, HIGH);
  digitalWrite(NextLED, HIGH);
  digitalWrite(PrevLED, HIGH);
}
void loop() {
  Webserver();
  //Check Button States
  int GoButtonState = digitalRead(GoButton);
  int StopButtonState = digitalRead(StopButton);
  int NextButtonState = digitalRead(NextButton);
  int PrevButtonState = digitalRead(PrevButton);
  /*****Mode Set*****/
  if (UserDefinedMode == "Not Set") {
    Serial.println(UserDefinedMode);
    lcd.setCursor(2, 0);
    lcd.print("Choose your mode");
    lcd.setCursor(4, 1);
    lcd.print("Blue for OSC");
    lcd.setCursor(4, 2);
    lcd.print("Red for MIDI");
    lcd.setCursor(0, 3);
    lcd.print("Yellow for Keyboard");
    //Button logic
    if (NextButtonState == HIGH) {
      lcd.clear();
      lcd.setCursor(8, 0);
      lcd.print("OSC");
      Serial.println("OSC Selected");
      UserDefinedMode = "OSC";
    }
    if (StopButtonState == HIGH) {
      lcd.clear();
      lcd.setCursor(8, 0);
      lcd.print("MIDI");
      Serial.println("MIDI Selected");
      UserDefinedMode = "MIDI";
    }
    if (PrevButtonState == HIGH) {
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("Keyboard");
      Serial.println("Keyboard Selected");
      UserDefinedMode = "Keyboard";
    }
  }
  //Send to function
  Buttons(GoButtonState, LastGoButtonState, OSC[0], MIDI[0], "Go Pushed", GoLED, UserDefinedMode);
  Buttons(StopButtonState, LastStopButtonState, OSC[1], MIDI[1], "Stop Pushed", StopLED, UserDefinedMode);
  Buttons(NextButtonState, LastNextButtonState, OSC[2], MIDI[2], "Next Pushed", NextLED, UserDefinedMode);
  Buttons(PrevButtonState, LastPrevButtonState, OSC[3], MIDI[3], "Prev Pushed", PrevLED, UserDefinedMode);

  //Reset Network
  if (PrevButtonState == HIGH && NextButtonState == HIGH) {
    UserDefinedMode = "Reset Network";
  }
  if (UserDefinedMode == "Reset Network"){
  ResetNetwork(GoButtonState, StopButtonState, UserDefinedMode);
  }
  //Remember what last state was to prevent flood
  LastGoButtonState = GoButtonState;
  LastStopButtonState = StopButtonState;
  LastNextButtonState = NextButtonState;
  LastPrevButtonState = PrevButtonState;
}
/*****Button Logic*****/
void Buttons(int ButtonState, int LastButtonState, int OSCMessage, int MIDIMessage, String toSerial, int LED, String CurrentMode) {
  if (ButtonState == HIGH && LastButtonState == LOW) {
    Serial.println(toSerial);
    if (CurrentMode == "OSC") {
      SendOSCMessage(OSCMessage);
      lcd.clear();
      lcd.setCursor(8, 0);
      lcd.print("OSC");
      lcd.setCursor(4, 3);
      lcd.print(toSerial);
    }
    else if (CurrentMode == "MIDI") {
      controlChange(1, MIDIMessage, 127);
      lcd.clear();
      lcd.setCursor(8, 0);
      lcd.print("MIDI");
      lcd.setCursor(5, 3);
      lcd.print(toSerial);
    }
    digitalWrite(LED, LOW);
    delay(100);
  }
  if (ButtonState == LOW) {
    digitalWrite(LED, HIGH);
  }
}
/*****OSC Message*****/
void SendOSCMessage(int OSCOut) {
  String BoardIP;
  String DestIP;
  String Subnet;
  String Port;
  Networkinfo = SD.open("Netinfo.txt", FILE_READ);
  BoardIP += Networkinfo.readStringUntil('\n');
  DestIP += Networkinfo.readStringUntil('\n');
  Subnet += Networkinfo.readStringUntil('\n');
  Port += Networkinfo.readStringUntil('\n');
  Networkinfo.close();
  ind1 = DestIP.indexOf('.');
  DipA = DestIP.substring(0, ind1);
  ind2 = DestIP.indexOf('.', ind1 + 1);
  DipB = DestIP.substring(ind1 + 1, ind2);
  ind3 = DestIP.indexOf('.', ind2 + 1);
  DipC = DestIP.substring(ind2 + 1, ind3);
  ind4 = DestIP.indexOf('.', ind3 + 1);
  DipD = DestIP.substring(ind3 + 1, ind4);
  uint32_t dipA = DipA.toInt();
  uint32_t dipB = DipB.toInt();
  uint32_t dipC = DipC.toInt();
  uint32_t dipD = DipD.toInt();

  byte destip[4] = { dipA, dipB, dipC, dipD};
  const unsigned int destPort = Port.toInt();
  OSCMessage msg("/arduino/");
  msg.add(OSCOut);
  Udp.beginPacket(destip, destPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
}
/*****MIDI Control Change*****/
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
/*****Reset Network Settings*****/
void ResetNetwork(int ButtonStateA, int ButtonStateB, String CurrentMode) {
  if(CurrentMode == "Reset Network"){
    if(ResetCounter == 0){
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Network Reset");
    lcd.setCursor(1, 1);
    lcd.print("RESET to Defaults?");
    lcd.setCursor(0, 2);
    lcd.print("Press Green to Reset");
    lcd.setCursor(0, 3);
    lcd.print("Press Red to Cancel");
    ResetCounter = ResetCounter + 1;
    }
  if (ButtonStateA == HIGH) {
    lcd.clear();
    lcd.print("Restoring..");
    SD.remove("Reset.txt");
    delay(5);
    delay(5);
    Resetorno = SD.open("Reset.txt", FILE_WRITE);
    Resetorno.println("DEFAULT");
    delay(5);
    Resetorno.close();
    Serial.print("Done writing to SD");
    delay(500);
    lcd.clear();
    lcd.print("Done.");
    lcd.setCursor(0, 1);
    lcd.print("Please Restart");
  }
  else if (ButtonStateB == HIGH) {
    lcd.clear();
    lcd.print("Canceled.");
    lcd.setCursor(0, 1);
    lcd.print("Please Restart");
  }
}
}
/*****OSC LCD Display*****/
void OSCMsgReceive() {
  char str [10]; // big enough array of char to hold 8 characters and a terminating zero
  for (int i = 0; i < 10; i++) {
    str [i] = 0; // make sure to clear the array of char with zeros to have a terminating zero
  }
  int size;
  if ( (size = Udp.parsePacket() ) > 0) {
    Udp.read(str, size); // read in "size" number of bytes into array of char "str"
    Serial.println(str);
  }
}
/*****WebPage*****/
void Webserver() {
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String BoardIP;
        String DestIP;
        String Subnet;
        String Port;
        Networkinfo = SD.open("Netinfo.txt", FILE_READ);
        BoardIP += Networkinfo.readStringUntil('\n');
        DestIP += Networkinfo.readStringUntil('\n');
        Subnet += Networkinfo.readStringUntil('\n');
        Port += Networkinfo.readStringUntil('\n');
        Networkinfo.close();
        char c = client.read();
        //read MaxHeaderLength number of characters in the HTTP header
        //discard the rest until \n
        if (HttpHeader.length() < MaxHeaderLength)
        {
          //store characters to stringtwi
          HttpHeader = HttpHeader + c;
        }
        //if HTTP request has ended
        if (c == '\n') {
          // show the string on the monitor
          // start of web page
          client.println("<!DOCTYPE html>");
          client.println("<html><body style=\"background-color:black\";>");
          client.println("<h2 style=\"color:white\"style=\"text-align:center;\" >Arduino OSC Manager</h2><h4 style=\"color:white\">*Update will cause Arduino to reintalize network settings*</h4><p1 style=\"color:white\">If Board IP changes, you must change it in your network settings inorder to access this page again. Refresh upon submission to see updated network info.</p1><p style=\"color:white\">Current Board IP:");
          client.print(Ethernet.localIP());
          client.println("</p><p style=\"color:white\">Current Destination IP:");
          client.print(DestIP);
          client.println("</p><p style=\"color:white\">Current Subnet Address:");
          client.print(Subnet);
          client.println("</p><p style=\"color:white\">Current Destination Port:");
          client.println(Port);
          client.println("<br><br></p><form name=\"IP Input\" action=\"submit\" method=\"get\"><p style=\"color:white\">");
          client.println("<label style=\"color:red\" for=\"naddr\">New Board IP Address:</label>");
          client.println("<input type=\"text\" id=\"baddr1\" name=\"baddr1\" maxlength=\"3\" size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"baddr2\" name=\"baddr2\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"baddr3\" name=\"baddr3\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"baddr4\" name=\"baddr4\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\"><br><br>");
          client.println("<label style=\"color:red\" for=\"naddr\">New Destination IP Address:</label>");
          client.println("<input type=\"text\" id=\"naddr1\" name=\"naddr1\" maxlength=\"3\" size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"naddr2\" name=\"naddr2\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"naddr3\" name=\"naddr3\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"naddr4\" name=\"naddr4\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\"><br><br>");
          client.println("<label style=\"color:red\" for=\"naddr\">New Subnet Address:</label>");
          client.println("<input type=\"text\" id=\"sub1\" name=\"sub2\" maxlength=\"3\" size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"sub2\" name=\"sub2\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"sub3\" name=\"sub3\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\">.<input type=\"text\" id=\"sub4\" name=\"sub4\"maxlength=\"3\"size=\"3\"pattern=\"[0-9]+\"><br><br>");
          client.println("<label style=\"color:red\" for=\"nport\">New Destination Port:</label>");
          client.println("<input type=\"text\" id=\"nport\" name=\"nport\" maxlength=\"5\" size= \"5\"pattern=\"[0-9]+\"></p><br><br>");
          client.println("<input type=\"submit\" value=\"Update\"window.location.reload()\">");
          client.println("</form></body></html>");
          client.stop();
          //Serial.println(HttpHeader);
          //Serial.println(portA);
          //Serial.println(outip);
          oscIP(HttpHeader);

          //stopping client

          //clearing string for next read
          HttpHeader = "";
        }
      }
    }
  }
}
/*****Store IP Data*****/
void oscIP(String A) {
  if (A.indexOf("baddr1") > 0) {
    String str_incoming = A;
    Serial.print("This is A:");
    Serial.println(A);
    index1 = str_incoming.indexOf('=');//Board IP A
    index2 = str_incoming.indexOf('&');//Board IP A
    index3 = str_incoming.indexOf('=', index2 + 1);//Board IP B
    index4 = str_incoming.indexOf('&', index2 + 1);//Board IP B
    index5 = str_incoming.indexOf('=', index4 + 1);//Board IP C
    index6 = str_incoming.indexOf('&', index4 + 1);//Board IP C
    index7 = str_incoming.indexOf('=', index6 + 1);//Board IP D
    index8 = str_incoming.indexOf('&', index6 + 1);//Board IP D
    index9 = str_incoming.indexOf('=', index8 + 1);//Dest IP A
    index10 = str_incoming.indexOf('&', index8 + 1);//Dest IP A
    index11 = str_incoming.indexOf('=', index10 + 1);//Dest IP B
    index12 = str_incoming.indexOf('&', index10 + 1);//Dest IP B
    index13 = str_incoming.indexOf('=', index12 + 1);//Dest IP C
    index14 = str_incoming.indexOf('&', index12 + 1);//Dest IP C
    index15 = str_incoming.indexOf('=', index14 + 1);//Dest IP D
    index16 = str_incoming.indexOf('&', index14 + 1);//Dest IP D
    index17 = str_incoming.indexOf('=', index16 + 1);//Subnet A
    index18 = str_incoming.indexOf('&', index16 + 1);//Sunmet A
    index19 = str_incoming.indexOf('=', index18 + 1);//Subnet B
    index20 = str_incoming.indexOf('&', index18 + 1);//Subnet B
    index21 = str_incoming.indexOf('=', index20 + 1);//Subnet C
    index22 = str_incoming.indexOf('&', index20 + 1);//Subnet c
    index23 = str_incoming.indexOf('=', index22 + 1);//Sunmet D
    index24 = str_incoming.indexOf('&', index22 + 1);//Subnet F
    index25 = str_incoming.indexOf('=', index24 + 1);//Port
    index26 = str_incoming.indexOf(' ', index24 + 1);

    //Board IP
    String strBIPfirstnum = str_incoming.substring(index1 + 1, index2);
    String strBIPsecondnum = str_incoming.substring(index3 + 1, index4);
    String strBIPthirdnum = str_incoming.substring(index5 + 1, index6);
    String strBIPfourthnum = str_incoming.substring(index7 + 1, index8);
    Serial.print("Board First Number: ");
    Serial.println(strBIPfirstnum);
    //Serial.println();
    Serial.print("Board Second Number: ");
    Serial.println(strBIPsecondnum);
    //Serial.println();
    Serial.print("Board Third Number: ");
    Serial.println(strBIPthirdnum);
    // Serial.println();
    Serial.print("Board Fourth Number: ");
    Serial.println(strBIPfourthnum);
    // Serial.println();
    SDstrBIPAddress = strBIPfirstnum + "." + strBIPsecondnum + "." + strBIPthirdnum + "." + strBIPfourthnum;
    //Destination IP
    String strDIPfirstnum = str_incoming.substring(index9 + 1, index10);
    String strDIPsecondnum = str_incoming.substring(index11 + 1, index12);
    String strDIPthirdnum = str_incoming.substring(index13 + 1, index14);
    String strDIPfourthnum = str_incoming.substring(index15 + 1, index16);
    Serial.print("Dest First Number: ");
    Serial.println(strDIPfirstnum);
    //Serial.println();
    Serial.print("Dest Second Number: ");
    Serial.println(strDIPsecondnum);
    //Serial.println();
    Serial.print("Dest Third Number: ");
    Serial.println(strDIPthirdnum);
    // Serial.println();
    Serial.print("Dest Fourth Number: ");
    Serial.println(strDIPfourthnum);
    // Serial.println();
    SDstrDIPAddress = strDIPfirstnum + "." + strDIPsecondnum + "." + strDIPthirdnum + "." + strDIPfourthnum;
    //Subnet
    String strSUBfirstnum = str_incoming.substring(index17 + 1, index18);
    String strSUBsecondnum = str_incoming.substring(index19 + 1, index20);
    String strSUBthirdnum = str_incoming.substring(index21 + 1, index22);
    String strSUBfourthnum = str_incoming.substring(index23 + 1, index24);
    Serial.print("Sub First Number: ");
    Serial.println(strSUBfirstnum);
    //Serial.println();
    Serial.print("Sub Second Number: ");
    Serial.println(strSUBsecondnum);
    //Serial.println();
    Serial.print("Sub Third Number: ");
    Serial.println(strSUBthirdnum);
    // Serial.println();
    Serial.print("Sub Fourth Number: ");
    Serial.println(strSUBfourthnum);
    // Serial.println();
    SDstrSubnetAddress = strSUBfirstnum + "." + strSUBsecondnum + "." + strSUBthirdnum + "." + strSUBfourthnum;
    //Port
    String strportnum = str_incoming.substring(index25 + 1, index26);
    Serial.print("Port Number: ");
    Serial.println(strportnum);
    SDstrPortnum = strportnum;

    /******SD Card******/
    //Previous Network Settings
    String BoardIP;
    String DestIP;
    String Subnet;
    String Port;
    Networkinfo = SD.open("Netinfo.txt", FILE_READ);
    BoardIP += Networkinfo.readStringUntil('\n');
    DestIP += Networkinfo.readStringUntil('\n');
    Subnet += Networkinfo.readStringUntil('\n');
    Port += Networkinfo.readStringUntil('\n');
    Networkinfo.close();
    //If Board IP is Blank then use Old network Settings
    if (strBIPfirstnum.indexOf("")) {
      SDstrBIPAddress = "";
      SDstrBIPAddress += BoardIP;
    }
    if (strDIPfirstnum.indexOf("")) {
      SDstrDIPAddress = "";
      SDstrDIPAddress += DestIP;
    }
    if (strSUBfirstnum.indexOf("")) {
      SDstrSubnetAddress = "";
      SDstrSubnetAddress += Subnet;
    }
    if (strportnum.indexOf("")) {
      SDstrPortnum = "";
      SDstrPortnum += Port;
    }
    Serial.println("Before printing to Network");
    Serial.print("BoardIP: ");
    Serial.println(SDstrBIPAddress);
    Serial.print("DestIP: ");
    Serial.println(SDstrDIPAddress);
    Serial.print("Subnet: ");
    Serial.println(SDstrSubnetAddress);
    Serial.print("Port: ");
    Serial.println(SDstrPortnum);
    //Delete Old File
    Serial.print("Clearing SD Card...");
    SD.remove("Netinfo.txt");
    delay(5);
    Serial.println("Done");
    //Open New File and Write New Data
    Networkinfo = SD.open("Netinfo.txt", FILE_WRITE);
    Networkinfo.println(SDstrBIPAddress);
    Networkinfo.println(SDstrDIPAddress);
    Networkinfo.println(SDstrSubnetAddress);
    Networkinfo.println(SDstrPortnum);

    Networkinfo.close();
    SD.remove("Reset.txt");
    delay(5);
    Resetorno = SD.open("Reset.txt", FILE_WRITE);
    Resetorno.println("RESET");
    Resetorno.close();
    Serial.print("Done writing to SD");
    delay(500);
    ethernetRESET();
  }
}
