#include <Arduino.h>
#include <SoftwareSerial.h>
#include <base64.hpp>

#define mySerial Serial1
#define LARGER_SIGN 62

#define TXPIN 27
#define RXPIN 14


struct type_process_t {
	String header = "";
	size_t NumberOfAttribute = 0;
	String Attribute[6];
	String Msg = "";
};

String parse_white_space(String* Info);
size_t parse_tail(String Info, String* Attr);
String parse_buffer_on_terminate(String* Info);
bool check_unexpected_message(String Info);
type_process_t process_parse(String* InfoBuffer);
String IgnoreString[2] = { "\r\n", "OK\r\n" };
size_t LengthOfIgnore = 1;
String TypeOfHeader[10] = { "OK", "+CIPOPEN", "+CIPSEND", "CIPRXGET", "AT+CIPRXGET",
							"+IP ERROR", "ERROR", "+CIPCLOSE", "+IPCLOSE"};
size_t LengthOfTypeHeader = 9;

String CipOpen = "AT+CIPOPEN=3,\"UDP\",,,8000";
String Send = "AT+CIPSEND=3,,\"115.74.105.253\",8000";
String Hello = "Hello World from Embedded 4G SIM \n";
String Hello2 = "This is test function\n\u001A";
String CloseCIP = "AT+CIPCLOSE=3";
String NetClose = "AT+NETCLOSE";
String Ok = "OK\r";

SoftwareSerial SWSer;
String buffer = "";
type_process_t process;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Hello From A7600 Test");
  mySerial.begin(3000000, SERIAL_8N1, 26, 27);
	// mySerial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
//   if(buffer.indexOf('\n') != -1) {
//     process = process_parse(&buffer);
//     if(process.header != "") {
//       Serial.println(process.Msg);
//       Serial.println(process.header);
//       Serial.println(process.NumberOfAttribute);
//       for(int i = 0; i < process.NumberOfAttribute; i++) {
//         Serial.println(process.Attribute[i]);
//       }
//     }
//   }
  if(Serial.available()) {
    int character = Serial.read();
    mySerial.write(character);
    Serial.write(character);
  }
  if(mySerial.available()){
    int character1 = mySerial.read();
    Serial.write(character1);
    // buffer = buffer + String((char)character1);
  }
}

type_process_t process_parse(String* InfoBuffer) {
	type_process_t retval;
	String tempString = "";
	bool Unexpected;
  retval.header = "";
	tempString = parse_buffer_on_terminate(InfoBuffer);
	retval.Msg = tempString;
	Unexpected = check_unexpected_message(tempString);
	if (Unexpected == false) {
		retval.header = parse_white_space(&tempString);
		if (tempString != "") {
			retval.NumberOfAttribute = parse_tail(tempString, retval.Attribute);
		}
	}
	return retval;
}

bool check_unexpected_message(String Info) {
	bool retval = false;
	for (size_t i = 0; i < LengthOfIgnore; i++) {
		if (Info == IgnoreString[i])
			retval = true;
	}
	return retval;
}
String parse_buffer_on_terminate(String* Info) {
	String temp = "";
	size_t pos = Info->indexOf("\r");
	if (pos != (size_t)(-1)) {
		temp = Info->substring(0, pos + 2);
		*(Info) = Info->substring(pos + 2, Info->length());
	}
	return temp;
}
String parse_white_space(String *Info) {
	String temp = "";
	size_t checkColon = Info->indexOf(":");
	if (checkColon != (size_t)(-1)) {
		temp = Info->substring(0, Info->indexOf(":"));
		*(Info) = Info->substring(Info->indexOf(":") + 2, Info->length());
	}
	else {
		temp = Info->substring(0, Info->indexOf("\r"));
		*(Info) = "";
	}
	return temp;
}

size_t parse_tail(String Info, String *Attr) {
	size_t Number = 0;
	while (Info.indexOf(",") != (size_t)(-1) ) {
		Attr[Number] = Info.substring(0, Info.indexOf(","));
		Info = Info.substring(Info.indexOf(",") + 1, Info.length());
		Number++;
	}
	if (Info.indexOf("\r") != 0) {
		Attr[Number] = Info.substring(0, Info.indexOf("\r"));
		Number++;
	}
	return Number;
}
