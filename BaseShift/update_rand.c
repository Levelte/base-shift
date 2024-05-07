#include <stdio.h>
#include <stdlib.h>

int main() {
    unsigned long long randUlong;
    FILE* randFile = fopen(".rand_ulong", "r");

    if (randFile) {
        char randStr[16];
        fgets(randStr, 16, randFile);
        randUlong = strtoul(randStr, NULL, 10);
        randUlong++;

        fclose(randFile);
    } else {
        randUlong = 1;
    }

    randFile = fopen(".rand_ulong", "w");
    fprintf(randFile, "%lld", randUlong);
    fclose(randFile);

    return 0;
}