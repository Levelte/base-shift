# BaseShift

SHA256 based encryption / decryption algorithm for linux

## Basic usage

1) Clone or download this repository.
2) Go to BaseShift directory inside the repository.
3) Open `.key` file with any text editor, and enter your key there.
4) Run the terminal inside current folder.
5) The example command below will encrypt the file `text.txt` using your key, and save it as "encrypted" in current directory (you need to create `text.txt` first).
```
./encrypt < ./text.txt > ./encrypted
```
6) Similarly, you can decrypt your encrypted file back, using the `decrypt` program, it will also use the key from `.key` file to decrypt an encrypted data.
```
./decrypt < ./encrypted > ./decrypted.txt
```
7) If you are using this algorithm not manually, and your program often encrypts data, you are recommended to execute the program `update_rand` before each encryption, it will help randomize the seed, and will make the encryption more secure.