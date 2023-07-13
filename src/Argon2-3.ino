#include "Particle.h"
#include "MQTT.h"

SYSTEM_THREAD(ENABLED);

void callback(char *topic, byte *payload, unsigned int length) {};
MQTT client("lab.thewcl.com", 1883, callback);


SerialLogHandler logHandler;

BleUuid serviceUuid("afe7acc5-33a9-478f-bbe1-8944aa08e884");

unsigned long lastScan;
BleAddress peripheralAddr;
int rssi;
String string_rssi;

void scanResultCallback(const BleScanResult *scanResult, void *context);

void setup() {

	BLE.on();
	//BLE.setAdvertisingInterval(20);

}

void loop() {
	rssi = 0;
	BLE.scan(scanResultCallback, NULL);

	if (rssi) {

		if (client.isConnected()) {
			client.loop();
			string_rssi = String(rssi);
			client.publish("BAR/argon3/RSSI", string_rssi);
		} else {
			client.connect(System.deviceID());
		}
	}
	
}


void scanResultCallback(const BleScanResult *scanResult, void *context) {

	BleUuid foundServiceUuid;
	size_t svcCount = scanResult->advertisingData().serviceUUID(&foundServiceUuid, 1);
	if (svcCount == 0 || !(foundServiceUuid == serviceUuid)) {
		/*
		Log.info("ignoring %02X:%02X:%02X:%02X:%02X:%02X, not our service",
				scanResult->address()[0], scanResult->address()[1], scanResult->address()[2],
				scanResult->address()[3], scanResult->address()[4], scanResult->address()[5]);
		*/
		return;
	}

	Log.info("rssi=%d server=%02X:%02X:%02X:%02X:%02X:%02X",
			scanResult->rssi(),
			scanResult->address()[0], scanResult->address()[1], scanResult->address()[2],
			scanResult->address()[3], scanResult->address()[4], scanResult->address()[5]);

	peripheralAddr = scanResult->address();
	rssi = scanResult->rssi();
	
	BLE.stopScanning();
}

