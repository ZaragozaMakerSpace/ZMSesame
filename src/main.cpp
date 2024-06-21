#include "config.h"
#include "wifiController.cpp"
#include <Arduino.h>
#include <Bounce2.h>
#include <FirebaseArduino.h>
#include <Ticker.h>

Bounce debouncer = Bounce();

void sendFirebase(String tag, bool state) {
	Firebase.setBool(tag, state);

	if (Firebase.failed()) {
		Serial.print("Error al enviar datos a Firebase: ");
		Serial.println(Firebase.error());
	} else {

		Serial.print("Estado de la puerta: ");
		Serial.println(state ? "Abierta" : "Cerrada");
	}
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
			sendFirebase("ZMSDoor", true);
		}
		if (data == 'C') {
			sendFirebase("ZMSDoor", false);
		}
	}
}

Ticker buttonTicker(checkButtonState, 100, 0, MILLIS);
Ticker serialTicker(checkSerial, 100, 0, MILLIS);
bool doorState = false;

void init() {
	Serial.begin(115200);
	pinMode(SWITCH_PIN, INPUT);
	Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

	debouncer.attach(SWITCH_PIN);
	debouncer.interval(5);
	buttonTicker.start();
	serialTicker.start();
}

void wifiConnect() {
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Conectando a WiFi...");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("Conectado a la red WiFi");
}

void setup() {
	init();
	wifiConnect();
}

void loop() {
	buttonTicker.update();
	serialTicker.update();
}