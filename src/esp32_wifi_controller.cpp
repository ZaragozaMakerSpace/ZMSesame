#ifndef _WIFI_UTILS_H
#define _WIFI_UTILS_H

#include "config.h"
#include <Ticker.h>
#include <WiFi.h>

void checkWifiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		DUMPSLN("Error: Board could not connect to WiFi.");
	} else {
		DUMPSLN("Wifi Connected");
	}
}

Ticker wifiTicker(checkWifiConnection, 1000, 0, MILLIS);

bool connectToWifi() {

	bool connected = false;
	DUMPSLN("Wi-Fi ...");
	WiFi.mode(WIFI_STA);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	wifiTicker.start();

	while (wifiTicker.state() == RUNNING) {
		wifiTicker.update();
		DUMPLN("Counter: ", wifiTicker.counter());

		// 10 Times each second = 10 seconds
		if (wifiTicker.counter() >= 10 || (WiFi.status() == WL_CONNECTED)) {
			wifiTicker.pause();
			break;
		};
	}

	connected = WiFi.status() == WL_CONNECTED;
	if (connected) {
		DUMPSLN("");
		DUMPLN("Wifi Connected - Local IP : ", WiFi.localIP());
		DUMPLN("GateWay IP = ", WiFi.gatewayIP());
		DUMPLN("SubnetMask = ", WiFi.subnetMask());
	}
	return connected;
}

#endif