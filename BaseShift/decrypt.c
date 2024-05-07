#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.c"

unsigned char* key = 0;
int keySize;
int chunk = 0;
unsigned char shiftBytes[32];

// Генерация 32 сдвиговых байт путём получения SHA256 хэша ключа с номером текущего чанка
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

    unsigned char* hash = SHA256(key, keySize + i);

    for (i = 0; i < 32; i++) {
        shiftBytes[i] = hash[i];
    }

    free(hash);
    chunk++;
}

unsigned int currentByte = 32;

// Чтение поданных на вход байт, расшифровка, и их вывод
int decryptChunk() {
    generateShiftBytes();

    unsigned char fileChunk[32];
    unsigned int bytesRead = read(STDIN_FILENO, &fileChunk, 32);

    int i;
    for (i = 0; i < bytesRead; i++) {
        fileChunk[i] -= shiftBytes[i];
    }

    fwrite(fileChunk, 1, bytesRead, stdout);
    return bytesRead;
}

int main() {
    // Открытие файла с ключом
    FILE* keyFile = fopen(".key", "r");

    if (!keyFile) {
        fprintf(stderr, "%s\n", "Error. Can't open .key file");
        return 0;
    }

    // Получение размера файла с ключом в байтах
    fseek(keyFile, 0L, SEEK_END);
    keySize = ftell(keyFile);
    rewind(keyFile);

    // Выделение памяти для ключа с запасом под сид
    key = malloc(keySize + 32);
    if (!key) {
        fprintf(stderr, "%s\n", "Error. Can't allocate memory for key data");
        return 0;
    }

    // Чтение файла с ключом и его закрытие
    fread (key, 1, keySize, keyFile);
    fclose(keyFile);

    // Чтение данных для расшифровки и проверка на количество байт
    unsigned char seed[16];
    if (read(STDIN_FILENO, &seed, 16) < 16) {
        fprintf(stderr, "%s\n", "Error. Not enough bytes in the file to decrypt it");
        return 0;
    }
    unsigned char encodedWatermark[16];
    if (read(STDIN_FILENO, &encodedWatermark, 16) < 16) {
        fprintf(stderr, "%s\n", "Error. Not enough bytes in the file to decrypt it");
        return 0;
    }

    // Запись сида в ключ
    int i;
    for (i = 0; i < 16; i++) {
        key[keySize + i] = seed[i];
    }
    keySize = keySize + 17;
    key[keySize - 1] = 58;

    // Расшифровка фразы BaseShiftEncoded, чтобы можно было убедиться в верности ключа
    generateShiftBytes();
    unsigned char watermark[16] = "BaseShiftEncoded";

    for (i = 0; i < 16; i++) {
        encodedWatermark[i] -= shiftBytes[i];

        if (encodedWatermark[i] != watermark[i]) {
            fprintf(stderr, "%s\n", "Error. Unable to decrypt the file");
            return 0;
        }
    }

    // Расшифровка по 32 байта
    while (decryptChunk() >= 32);

    // Освобожение памяти выделенной под ключ, и успешное выполнение операции
    free(key);
    return 0;
}
