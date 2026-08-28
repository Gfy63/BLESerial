// Copyright 2019 Ian Archbell / oddWires
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/***********************************
 * @file    BLEserial.h
 * @author  Gfy63 (mrgoofy@gmx.net) 
 *          Original: Copyright 2019 Ian Archbell / oddWires (https://github.com/iot-bus/BLESerial.git)
 * @brief   Serial over BLE. (UART) (Functions are compatible with BluetoothSerial)
 * @version 0.4.1
 * @date 2026-08-28
 * 
 * @copyright 2025-26
 **********************************/

#ifndef _BLE_SERIAL_H_
#define _BLE_SERIAL_H_

#include "Arduino.h"
#include "Stream.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <esp_spp_api.h>

class BLESerial: public Stream
{
	public:

		/**
		 * --- CONSTRUCTOR & BEGIN ---
		*/

		BLESerial(void);
		~BLESerial(void);

		bool begin(const char* localName="UART Service");
		bool begin( BLEServer* sharedServer );

		/**
		 * --- PUBLIC FUNCTIONS ---
		*/

		int available(void);
		int peek(void);
		bool connected(void);
		int read(void);

		size_t write(uint8_t c);
		size_t write(const uint8_t *buffer, size_t size);
		size_t write(char *buffer, size_t size);
		size_t write(char *buffer);
		size_t write( String buffer);

		void flush();
		void end(void);

		esp_err_t register_callback(esp_spp_cb_t callback);

	private:
		const char* LOG_TAG = "BLESerial (Class)";		// ESP_LOG tag

		const char* SERVICE_UUID           = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";	 // UART service UUID
		const char* CHARACTERISTIC_UUID_RX = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
		const char* CHARACTERISTIC_UUID_TX = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

		bool pLocalServer = false;			// Local BLE server if true.
		String local_name;
		BLEServer *pServer = NULL;
		BLEService *pService;
		BLECharacteristic * pTxCharacteristic;
		bool deviceConnected = false;
		uint8_t txValue = 0;

		uint16_t negotiatedMTU = 23;		// Default MTU.

		// For compability with BluetoothSerial.
		esp_spp_cb_t custom_spp_callback = NULL;
		esp_spp_cb_param_t param;

		std::string receiveBuffer;

		friend class BLESerialServerCallbacks;
		friend class BLESerialCharacteristicCallbacks;

};

#endif
