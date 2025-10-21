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
 * @brief   Serial over BLE. (UART)
 * @version 0.2.0
 * @date 2025-01-16
 * 
 * @copyright 2025
 **********************************/

#ifndef _BLE_SERIAL_H_
#define _BLE_SERIAL_H_

#include "Arduino.h"
#include "Stream.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class BLESerial: public Stream
{
    public:

    	/**
		 * --- CONSTRUCTOR & BEGIN ---
		*/

    	/**
		 * @brief Constructors.
		 */
        BLESerial(void);
        ~BLESerial(void);

		/**
		 * @brief Use if constructor is empty.
		 * @param localName     Name of the BLE connection.
		*/
        bool begin(const char* localName="UART Service");

		/**
		 * --- PUBLIC FUNCTIONS ---
		*/

        /**
		 * @brief Data are available if not 0.
		 * @return Number of char in read buffer.
		*/
        int available(void);

		/**
		 * @brief Read a char without deleting it from buffer.
		 * @return ASCII code of the char.
		*/
        int peek(void);

		/**
		 * @brief Is a device is connected.
		 * @return True if a device is connected.
		*/
        bool connected(void);

		/**
		 * @brief Read a char.
		 * @return ASCII code of the char.
		*/
        int read(void);

		/**
		 * @brief Write data to BLE.
         * @param c         Byte to send.
         * @param buffer    Data to send.
         * @param size      Number of bytes to send.
		 * @return Number of byte send.
		*/
        size_t write(uint8_t c);
        size_t write(const uint8_t *buffer, size_t size);
        size_t write(char *buffer, size_t size);
        size_t write(char *buffer);


		/**
		 * @brief Remove buffer data.
		*/
        void flush();

 		/**
		 * @brief Close connection.
		*/
       void end(void);

    private:
        String local_name;
        BLEServer *pServer = NULL;
        BLEService *pService;
        BLECharacteristic * pTxCharacteristic;
        bool deviceConnected = false;
        uint8_t txValue = 0;
        
        std::string receiveBuffer;

        friend class BLESerialServerCallbacks;
        friend class BLESerialCharacteristicCallbacks;

};

#endif
