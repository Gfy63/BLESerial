 # BLESerial
 BLE serial class compatible with NORDIC serial and BluetoothSerial.

## DOWNLOAD
Download from [github](https://github.com/Gfy63/BLESerial.git).

## BASIC USAGE

### Setup

```cpp
BLESerial bleSerial;
void BT_EventHandler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);  // Callback function.

bleSerial.begin("BLE Serial");  // Set BLE server name.
// or
bleSerial.begin( pServer );  // Use existing BLEserver.

bleSerial.register_callback(BT_EventHandler);  // Register callback function.

loop
{
    // Echo to Serial.
    if ( bleSerial.available() ) { Serial.write( bleSerial.read() ); }

    if ( Serial.available() ) { bleSerial.write( Serial.read() ); }

}
```

 ## Credits

 BLESerial was written by Ian Archbell of oddWires. It is based on the BLE implementation
 originally created by Neil Kolban and included in the Espressif esp32 distribution.

[BLESerial](https://github.com/lemio/BLESerial)
