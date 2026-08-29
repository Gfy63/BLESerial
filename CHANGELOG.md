# Changelog for BELSerial

## [0.4.2] - 2026-08-29

	CHG		begin() can contailn also callback.
	CHG		register_callback() has no return value.

## [0.4.1] - 2026-08-28

	CHG     Replace #define with constexpr
	CHG		Use ESP_LOGx

## [0.4.0] - 2026-08-22

	ADD		begin( BLEserver * sharedServer ) for multi connections.
	ADD		write( String ), send in packages of MTU length.
	
## [0.3.0] - 2025-11-11

	ADD		'register_callback()' for compabilty with BluetoothSerial.
	ADD		Get end() a working function.
	
## [0.2.0] - 2025-01-16

	Forged by Gfy63
	ADD		overloads for write().

## [0.1.0] - 2019

	Release by Ian Archbell of oddWires


