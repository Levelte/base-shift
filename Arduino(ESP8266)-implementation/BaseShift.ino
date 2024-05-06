#include <ArduinoCrypto.h>

unsigned char* originalKey = 0;
int originalKeySize = 0;

unsigned char* key = 0;
int keySize;
int chunk = 0;
unsigned long currentChar = 0;
unsigned char shiftBytes[32];

void generateShiftBytes() {
    char chunkStr[12];
    sprintf(chunkStr, "%d", chunk);

    int i;
    for (i = 0; i < 12; i++) {
        char value = chunkStr[i];

        key[keySize + i] = (unsigned char)value;

        if (value == 0) {
            break;
        }
    }

    SHA256 hasher;
    hasher.doUpdate(key, keySize + i);
    byte hash[SHA256_SIZE];
    hasher.doFinal(hash);

    for (i = 0; i < 32; i++) {
        shiftBytes[i] = hash[i];
    }

    chunk++;
}

int encryptChunk(unsigned char* data, unsigned long len, unsigned char* result) {
  generateShiftBytes();

  unsigned int bytesRead = min((int)(len - currentChar), 32);

  for (unsigned long i = 0; i < bytesRead; i++) {
    result[currentChar + 32] = (unsigned char)(data[currentChar] + shiftBytes[i]);
    currentChar++;
  }

  return bytesRead;
}

int encrypt(unsigned char* data, unsigned long len, unsigned long currentTime, unsigned char* result) {
  chunk = 0;
  currentChar = 0;

  unsigned char seed[17];
  seed[16] = '\0';
  seed[0] = (currentTime >> 24) & 0xFF;
  seed[1] = (currentTime >> 16) & 0xFF;
  seed[2] = (currentTime >> 8) & 0xFF;
  seed[3] = currentTime & 0xFF;
  seed[4] = 0;
  seed[5] = 0;
  seed[6] = 0;
  seed[7] = 0;

  randomSeed(currentTime * analogRead(0));

  for (int i = 8; i < 16; i++) {
    seed[i] = (unsigned char)(random(256));
  }

  keySize = originalKeySize;
  key = (unsigned char*)malloc(keySize + 32);
  if (!key) {
    return -1;
  }
  for (int i = 0; i < keySize; i++) {
    key[i] = originalKey[i];
  }

  // Запись сида в ключ
  for (int i = 0; i < 16; i++) {
    key[keySize + i] = seed[i];
  }
  keySize = keySize + 17;
  key[keySize - 1] = 58;

  // Запись сида в начало вывода
  for (int i = 0; i < 16; i++) {
    result[i] = seed[i];
  }

  // Шифрование фразы BaseShiftEncoded, чтобы при расшифровке можно было убедиться в успешности операции
  generateShiftBytes();
  unsigned char watermark[17] = "BaseShiftEncoded";
  for (int i = 0; i < 16; i++) {
      result[i + 16] = (unsigned char)(watermark[i] + shiftBytes[i]);
  }

  // Шифрование по 32 байта
  while (encryptChunk(data, len, result) >= 32);

  // Освобожение памяти выделенной под ключ, и успешное выполнение операции
  free(key);
  return 0;
}

int decryptChunk(unsigned char* data, unsigned long len, unsigned char* result, char* error = 0) {
  generateShiftBytes();

  unsigned int bytesRead = min((int)(len - currentChar), 32);

  for (unsigned long i = 0; i < bytesRead; i++) {
    result[currentChar - 32] = (unsigned char)(data[currentChar] - shiftBytes[i]);
    currentChar++;
  }

  return bytesRead;
}

int decrypt(unsigned char* data, unsigned long len, unsigned char* result) {
  if (len < 32) {
    return -1;
  }

  chunk = 0;
  currentChar = 32;

  keySize = originalKeySize;
  key = (unsigned char*)malloc(keySize + 32);
  if (!key) {
    return -2;
  }
  for (int i = 0; i < keySize; i++) {
    key[i] = originalKey[i];
  }

  // Запись сида в ключ
  for (int i = 0; i < 16; i++) {
    key[keySize + i] = data[i];
  }
  keySize = keySize + 17;
  key[keySize - 1] = 58;

  // Расшифровка фразы BaseShiftEncoded, чтобы можно было убедиться в верности ключа
  generateShiftBytes();
  unsigned char watermark[17] = "BaseShiftEncoded";
  for (int i = 0; i < 16; i++) {
    if ((unsigned char)(data[i + 16] - shiftBytes[i]) != watermark[i]) {
      Serial.println("Error. Unable to decrypt the file");
      return -3;
    }
  }

  // Расшифровка по 32 байта
  while (decryptChunk(data, len, result) >= 32);

  // Освобожение памяти выделенной под ключ, и успешное выполнение операции
  free(key);
  return 0;
}

void setKey(unsigned char* newKey, int newKeySize) {
  if (originalKeySize != 0) {
    free(originalKey);
  }

  originalKey = (unsigned char*)malloc(newKeySize);
  int i;
  for (i = 0; i < newKeySize; i++) {
    originalKey[i] = newKey[i];
  }

  originalKeySize = newKeySize;
}