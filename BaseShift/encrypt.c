#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "sha256.c"

unsigned char* key = 0;
int keySize;
int chunk = 0;
unsigned char shiftBytes[32];
unsigned long long randUlong;

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

// Чтение поданных на вход байт, шифрование, и их вывод
int encryptChunk() {
    generateShiftBytes();

    unsigned char fileChunk[32];
    unsigned int bytesRead = read(STDIN_FILENO, &fileChunk, 32);

    int i;
    for (i = 0; i < bytesRead; i++) {
        fileChunk[i] += shiftBytes[i];
    }

    fwrite(fileChunk, 1, bytesRead, stdout);
    return bytesRead;
}

int main() {
    // Получение текущего UNIX времени, и времени в микросекундах
    struct timespec tw = {0,000001000};
    nanosleep (&tw, NULL);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long sec = tv.tv_sec;
    unsigned long usec = tv.tv_usec;

    // Запись времени в сид, разделяя числа на байты
    unsigned char seed[17];
    seed[16] = '\0';
    seed[0] = (sec >> 24) & 0xFF;
    seed[1] = (sec >> 16) & 0xFF;
    seed[2] = (sec >> 8) & 0xFF;
    seed[3] = sec & 0xFF;
    seed[4] = (usec >> 24) & 0xFF;
    seed[5] = (usec >> 16) & 0xFF;
    seed[6] = (usec >> 8) & 0xFF;
    seed[7] = usec & 0xFF;

    // Открытие файла с числом необходимым для более случайной генерации сида
    FILE* randFile = fopen(".rand_ulong", "r");

    if (randFile) {
        // Чтение файла .rand_ulong, перевод в unsigned long, и запись в переменную
        char randStr[16];
        fgets(randStr, 16, randFile);
        randUlong = strtoul(randStr, NULL, 10);

        fclose(randFile);
    } else {
        fprintf(stderr, "%s\n", "Error. Can't open .rand_ulong file");
        return 0;
    }

    // Перемножение текущего времени в микросекундах с числом из файла .rand_ulong, для более случайной генерации байт
    srand(sec * randUlong);

    // Запись случайных байт в сид
    int i;
    for (i = 8; i < 16; i++) {
        seed[i] = (unsigned char)(rand() % 256);
    }

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

    // Запись сида в ключ
    for (i = 0; i < 16; i++) {
        key[keySize + i] = seed[i];
    }
    keySize = keySize + 17;
    key[keySize - 1] = 58;

    // Запись сида в начало вывода
    fwrite(seed, 1, 16, stdout);

    // Шифрование фразы BaseShiftEncoded, чтобы при расшифровке можно было убедиться в успешности операции
    generateShiftBytes();
    unsigned char watermark[16] = "BaseShiftEncoded";

    for (i = 0; i < 16; i++) {
        watermark[i] += shiftBytes[i];
    }

    fwrite(watermark, 1, 16, stdout);

    // Шифрование по 32 байта
    while (encryptChunk() >= 32);

    // Освобожение памяти выделенной под ключ, и успешное выполнение операции
    free(key);
    return 0;
}
