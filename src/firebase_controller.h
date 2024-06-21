#include "credentials.h"
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
LegacyToken dbSecret(DATABASE_SECRET);

void initFirebase() {
	Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);
	ssl.setInsecure();
#if defined(ESP8266)
	ssl.setBufferSizes(1024, 1024);
#endif
	initializeApp(client, app, getAuth(dbSecret));

	app.getApp<RealtimeDatabase>(Database);

	Database.url(DATABASE_URL);
	client.setAsyncResult(result);
}

void printError(int code, const String &msg) {
	Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void sendFirebase(String tag, bool state, bool pushTime = true) {
	bool status = Database.set<bool>(client, "/" + tag, state);
	if (status)
		Serial.println("ok");
	else
		printError(client.lastError().code(), client.lastError().message());

	if (pushTime) {
		String name = Database.push<bool>(client, tag + "/timestamp", state);
		if (client.lastError().code() == 0)
			Firebase.printf("SENT, status: %s\n", name.c_str());
	}
}
