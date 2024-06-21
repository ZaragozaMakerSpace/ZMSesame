#ifndef _WIFI_UTILS_H
#define _WIFI_UTILS_H

#include "config.h"
#include <Ticker.h>

#if defined(ESP8266)
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
#endif

void checkWifiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		DUMPSLN("Error: Board could not connect to WiFi.");
	} else {
		DUMPSLN("Wifi Connected");
	}
}

void defineWifiEvents() {
	// WiFi.onEvent(connected_to_ap, ARDUINO_EVENT_WIFI_STA_CONNECTED);
	// WiFi.onEvent(got_ip_from_ap, ARDUINO_EVENT_WIFI_STA_GOT_IP);
}
Ticker wifiTicker(checkWifiConnection, 1000, 0, MILLIS);
Ticker wifiEventsTicker(checkWifiConnection, 1000, 0, MILLIS);

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

#if defined(ESP8266)
void wifiEvents(const WiFiEventSoftAPModeStationConnected &evt) {
	DUMPSLN("\nConnected to the WiFi network");
}

void testWifiEvents() {
	DUMPSLN("Wi-Fi ...");
	WiFi.mode(WIFI_STA);
	defineWifiEvents();

	WiFi.begin(WIFI_SSID, WIFI_PASS);
	if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET, PRIMARYDNS, SECONDARYDNS)) {
		DUMPSLN("STA Failed to configure");
	}
	wifiEventsTicker.start();
	wifiConnectHandler = WiFi.onSoftAPModeStationConnected(&wifiEvents);
}
#endif

#endif