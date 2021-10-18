/*
* Initial Author: ryand1011 (https://github.com/ryand1011)
*
* Reads data written by a program such as "rfid_write_personal_data.ino"
*
* See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
*
* Uses MIFARE RFID card using RFID-RC522 reader
* Uses MFRC522 - Library
* -----------------------------------------------------------------------------------------
*             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
*             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
* Signal      Pin          Pin           Pin       Pin        Pin              Pin
* -----------------------------------------------------------------------------------------
* RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
* SPI SS      SDA(SS)      10            53        D10        10               10
* SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
* SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
* SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 53;     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

									//*****************************************************************************************//
void setup() {
	Serial.begin(9600);                                           // Initialize serial communications with the PC
	SPI.begin();                                                  // Init SPI bus
	mfrc522.PCD_Init();                                              // Init MFRC522 card
	Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
}

//*****************************************************************************************//
void loop() {

	// Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
	MFRC522::MIFARE_Key key;
	for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

	//some variables we need
	byte block;
	byte len;
	MFRC522::StatusCode status;

	//-------------------------------------------

	// Look for new cards
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	Serial.println(F("**Card Detected:**"));

	//-------------------------------------------

	mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

													  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

													  //-------------------------------------------

	//----------------------------------------

	Serial.println(F("\n**End Reading**\n"));

	delay(100); //change value if you want to read cards faster

	mfrc522.PICC_HaltA();
	mfrc522.PCD_StopCrypto1();
}
//*****************************************************************************************//
