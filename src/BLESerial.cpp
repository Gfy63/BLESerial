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
 * @file    BLESerial.cpp
 * @author  Gfy63 (mrgoofy@gmx.net)
 * 
 * @copyright 2025-26
 **********************************/

#include "BLESerial.h"

class BLESerialServerCallbacks: public BLEServerCallbacks {
	friend class BLESerial; 
	BLESerial* bleSerial;
	
	void onConnect(BLEServer* pServer) {
		// do anything needed on connection
		delay(1000); // wait for connection to complete or messages can be lost

 		ESP_LOGI( bleSerial->LOG_TAG, "Device connected." );

		if( bleSerial->custom_spp_callback ) (bleSerial->custom_spp_callback)(ESP_SPP_SRV_OPEN_EVT, NULL);

	};

	void onDisconnect(BLEServer* pServer) {
		delay(500);						// give the bluetooth stack the chance to get things ready
		pServer->startAdvertising();	// restart advertising
		ESP_LOGI( bleSerial->LOG_TAG, "Started advertising." );
	
		if( bleSerial->custom_spp_callback ) (bleSerial->custom_spp_callback)(ESP_SPP_CLOSE_EVT, NULL);
	}

	void onMtuChanged(BLEServer* pServer, esp_ble_gatts_cb_param_t* param) {
		bleSerial->negotiatedMTU = param->mtu.mtu;
		ESP_LOGI( bleSerial->LOG_TAB, "Negotiated MTU: %d", param->mtu.mtu);
	}

};

class BLESerialCharacteristicCallbacks: public BLECharacteristicCallbacks {
	friend class BLESerial; 
	BLESerial* bleSerial;
	
	void onWrite(BLECharacteristic *pCharacteristic) {
 
	  bleSerial->receiveBuffer = bleSerial->receiveBuffer + pCharacteristic->getValue();
	}

};

////////////////////////////////////

/**
 * --- PUBLIC FUNCTIONS ---
*/

// Constructor

/**
 * @brief Constructors.
 */
BLESerial::BLESerial()
{
  // create instance  
  receiveBuffer = "";

}

// Destructor

BLESerial::~BLESerial(void) {}  // clean up

////////////////////////////////////

// Begin bluetooth serial

/**
 * @brief Use if constructor is empty.
 * @param localName     Name of the BLE connection.
*/
bool BLESerial::begin(const char* localName)
{
	pLocalServer = true;		// Local server.

	// Create the BLE Device
	BLEDevice::init(localName);

	// Create the BLE Server
	pServer = BLEDevice::createServer();
	if (pServer == nullptr)
		return false;

	return begin( pServer );		// Sart BLESerial.

} // begin()

/**
 * @brief Use if constructor is empty.
 * @param server     Use of a existing server.
*/
bool BLESerial::begin( BLEServer *server )
{
	// Use existing BLE Server
	pServer = server;
	if (pServer == nullptr)
		return false;
	
	BLESerialServerCallbacks* bleSerialServerCallbacks =  new BLESerialServerCallbacks(); 
	bleSerialServerCallbacks->bleSerial = this;      
	pServer->setCallbacks(bleSerialServerCallbacks);

	// Create the BLE Service
	pService = pServer->createService(SERVICE_UUID);
	if (pService == nullptr)
		return false;

	// Create a BLE Characteristic
	pTxCharacteristic = pService->createCharacteristic(
											CHARACTERISTIC_UUID_TX,
											BLECharacteristic::PROPERTY_NOTIFY
										);
	if (pTxCharacteristic == nullptr)
		return false;                    
	pTxCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
												CHARACTERISTIC_UUID_RX,
												BLECharacteristic::PROPERTY_WRITE
											);
	if (pRxCharacteristic == nullptr)
		return false; 

	BLESerialCharacteristicCallbacks* bleSerialCharacteristicCallbacks =  new BLESerialCharacteristicCallbacks(); 
	bleSerialCharacteristicCallbacks->bleSerial = this;  
	pRxCharacteristic->setCallbacks(bleSerialCharacteristicCallbacks);

	// Start the service
	pService->start();
	ESP_LOGI( ESP_LOG, "starting service" );

	// Start advertising
	pServer->getAdvertising()->addServiceUUID(pService->getUUID()); 
	pServer->getAdvertising()->setMinPreferred( 0x00 );
	pServer->getAdvertising()->start();
	ESP_LOGI( LOG_TAG, "Waiting a client connection to notify..." );
	return true;

} // begin()

////////////////////////////////////

/**
 * @brief Data are available if not 0.
 * @return Number of char in read buffer.
*/
int BLESerial::available(void)
{
	// reply with data available
	return receiveBuffer.length();

} // available()

////////////////////////////////////

/**
 * @brief Read a char without deleting it from buffer.
 * @return ASCII code of the char.
*/
int BLESerial::peek(void)
{
	// return first character available
	// but don't remove it from the buffer
	if ((receiveBuffer.length() > 0)){
		uint8_t c = receiveBuffer[0];
		return c;
	}
	else
		return -1;

} // peek()

////////////////////////////////////

/**
 * @brief Is a device is connected.
 * @return True if a device is connected.
*/
bool BLESerial::connected(void)
{
	// true if connected
	if (pServer->getConnectedCount() > 0)
		return true;
	else 
		return false;     

} // connected()

////////////////////////////////////

/**
 * @brief Read a char.
 * @return ASCII code of the char.
*/
int BLESerial::read(void)
{
	// read a character
	if ((receiveBuffer.length() > 0)){
		uint8_t c = receiveBuffer[0];
		receiveBuffer.erase(0,1); // remove it from the buffer
		return c;
	}
	else
		return -1;

} // read()

////////////////////////////////////

/**
 * @brief Write data to BLE.
 * @param c         Byte to send.
 * @return Number of byte send.
*/
size_t BLESerial::write(uint8_t c)
{
	// write a character
	uint8_t _c = c;
	pTxCharacteristic->setValue(&_c, 1);
	pTxCharacteristic->notify();
	delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	return 1;

} // Write()

/**
 * @brief Write data to BLE.
 * @param buffer    Data to send.
 * @param size      Number of bytes to send.
 * @return Number of byte send.
*/
size_t BLESerial::write(const uint8_t *buffer, size_t size)
{
	// write a buffer
	for(int i=0; i < size; i++){
		write(buffer[i]);
  }
  return size;

} // write()

/**
 * @brief Write data to BLE.
 * @param buffer    Data to send.
 * @param size      Number of bytes to send.
 * @return Number of byte send.
*/
size_t BLESerial::write(char *buffer, size_t size)
{
	// write a buffer
	for(int i=0; i < size; i++){
		write(buffer[i]);
  }
  return size;

} // write()

/**
 * @brief Write data to BLE.
 * @param buffer    Data to send.
 * @return Number of byte send.
*/
size_t BLESerial::write(char *buffer)
{
	// write a buffer until NULL
	int i=0;
	while( buffer[i] != '\0')
	{
		write(buffer[i]);
		i++;
	}

  return i;

} // write()

/**
 * @brief Write data to BLE. Send packages with maximal MTU length.
 * @param buffer    Data to send.
 * @return Number of byte send.
*/
size_t BLESerial::write( String buffer)
{
	size_t total = buffer.length();
	uint16_t maxPayload = (negotiatedMTU > 3) ? (negotiatedMTU - 3) : 20;
	size_t sent = 0;

	while (sent < total) {
		size_t chunkLen = min((size_t)maxPayload, total - sent);
		pTxCharacteristic->setValue((uint8_t*)(buffer.c_str() + sent), chunkLen);
		pTxCharacteristic->notify();
		sent += chunkLen;
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}
	return sent;

} // write()

////////////////////////////////////

/**
 * @brief Remove buffer data.
*/
void BLESerial::flush()
{
	// remove buffered data
	receiveBuffer.clear();

} // flush()

////////////////////////////////////

/**
 * @brief Close connection.
*/
void BLESerial::end()
{
	if( pLocalServer )
	{
		// Local server. Close and stop BLE.
		// close connection
		pService->executeDelete();
		BLEDevice::deinit();
	}
	else
	{
		// Extern server. Stop BLE service.
		pServer->getAdvertising()->stop();
		if( pService != NULL )
		{
			pService->stop();
			pService->executeDelete();
			pService = NULL;
		}
		pServer->getAdvertising()->start();
	}

} // end()

////////////////////////////////////

/**
 * @brief Event callback.
 *        For compability with BluetoothSerial.
 * @param callback Private callback function.
 * @return ESP_OK
*/
esp_err_t BLESerial::register_callback(esp_spp_cb_t callback)
{
	custom_spp_callback = callback;
	return ESP_OK;
}

// End of 'BLESerial.cpp'.