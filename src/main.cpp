#include "config.h"
#include "credentials.h"

#include "firebase_controller.h"
#include <Bounce2.h>
#include <Ticker.h>

Bounce debouncer = Bounce();
bool doorState = false;

bool connectToWifi() {
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	Serial.print("Connecting to Wi-Fi");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(300);
	}
	Serial.println();
	Serial.print("Connected with IP: ");
	Serial.println(WiFi.localIP());
	Serial.println();

	return WiFi.status() != WL_CONNECTED;
}

void checkButtonState() {
	debouncer.update();
	bool currentState = debouncer.read();
	if (currentState != doorState) {
		doorState = currentState;
		sendFirebase("ZMSDoor", doorState);
	}
}

void checkSerial() {
	if (Serial.available()) {
		char data = Serial.read();
		if (data == 'O') {
			sendFirebase("ZMSDoor", true, false);
		}
		if (data == 'C') {
			sendFirebase("ZMSDoor", false, false);
		}
	}
}

Ticker buttonTicker(checkButtonState, 100, 0, MILLIS);
Ticker serialTicker(checkSerial, 100, 0, MILLIS);

void initSesame() {
	Serial.begin(115200);
	pinMode(SWITCH_PIN, INPUT);
	initFirebase();

	debouncer.attach(SWITCH_PIN);
	debouncer.interval(5);
	buttonTicker.start();
	serialTicker.start();
}

void setup() {
	connectToWifi();
	initSesame();
}

void loop() {
	buttonTicker.update();
	serialTicker.update();
}