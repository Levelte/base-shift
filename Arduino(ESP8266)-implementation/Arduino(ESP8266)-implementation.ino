#include <EEPROM.h>

int inputLength = 0;
unsigned char* inputData = 0;

int menu = 0;
 
void setup() {
    Serial.begin(115200);
    EEPROM.begin(4096);

    delay(10);
    Serial.println();
    Serial.println("BaseShift arduino example");
    Serial.println("Please, specify 'No Line Ending' setting before sending input messages");
    Serial.println("Encrypt and save data to EEPROM(e) or load from EEPROM and decrypt it(d)?");
}

int i;

void loop() {
    inputLength = Serial.available();

    if (inputLength > 0) {
        inputData = (unsigned char*)malloc(Serial.available());

        for (i = 0; i < inputLength; i++) {
          inputData[i] = (unsigned char)Serial.read();
        }

        if (menu == 0) {
          if (inputLength == 1) {
            if (inputData[0] == 101) {
              menu = 1;
              Serial.println("Enter key to encrypt new data");
            } else if (inputData[0] == 100) {
              menu = 2;
              Serial.println("Enter key to decrypt existing data");
            }
          }
        } else if (menu == 1) {
          menu = 3;
          setKey(inputData, inputLength);
          Serial.println("Enter data you want to encrypt");
        } else if (menu == 2) {
          menu = 4;
          setKey(inputData, inputLength);
          uint16_t dataLength;
          EEPROM.get(0, dataLength);

          if (dataLength < 32) {
            Serial.println("Not enough data to decrypt");
          } else {
            unsigned char* dataArray = (unsigned char*)malloc(dataLength);
            for (int i = 0; i < dataLength; i++) {
              unsigned char get;
              EEPROM.get(i + 2, get);
              dataArray[i] = get;
            }

            int decryptedLength = dataLength - 32;
            unsigned char* decrypted = (unsigned char*)malloc(decryptedLength);
            int error = decrypt(dataArray, dataLength, decrypted);
            free(dataArray);

            if (error != 0) {
              Serial.print("Decryption error ");
              Serial.print(error);
              Serial.println();
            } else {
              Serial.println("Successful decryption, printing decrypted data...");
              for (i = 0; i < decryptedLength; i++) {
                Serial.print((char)decrypted[i]);
              }
              Serial.println("");
            }

            free(decrypted);
          }
        } else if (menu == 3) {
          menu = -1;
          int encryptedLength = inputLength + 32;

          if (encryptedLength > 4094) {
            Serial.println("Not enough memory in the EEPROM to save your data, try again");
          } else {
            unsigned char* encrypted = (unsigned char*)malloc(encryptedLength);
            int error = encrypt(inputData, inputLength, millis(), encrypted);
            if (error != 0) {
              Serial.print("Encryption error ");
              Serial.print(error);
              Serial.println();
            } else {
              Serial.println("Successful encryption");

              EEPROM.put(0, (uint16_t)encryptedLength);
              for (i = 0; i < encryptedLength; i++) {
                Serial.println(encrypted[i], DEC);
                EEPROM.put(i + 2, encrypted[i]);
              }
              EEPROM.commit();

              Serial.println("Saved to EEPROM");
            }

            free(encrypted);
          }
        }

        free(inputData);
    }

    delay(100);
}