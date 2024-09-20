# BaseShift

SHA256 based encryption / decryption algorithm for linux

## Basic usage

1) Clone or download this repository.
2) Go to BaseShift directory inside the repository.
3) Open `.key` file in any text editor, and enter your key there.
4) Run a terminal inside the current folder.
5) The example command below will encrypt the file `text.txt` using your key, and save it as "encrypted" in current directory (you need to create `text.txt` first).
```
./encrypt < ./text.txt > ./encrypted
```
6) Similarly, you can decrypt your encrypted file back, using the `decrypt` program, it will also use the key from `.key` file to decrypt an encrypted data.
```
./decrypt < ./encrypted > ./decrypted.txt
```
7) If you are using this algorithm inside some program, and it often encrypts data, you are recommended to execute the program `update_rand` before each encryption, it will help randomize the seed, and will make the encryption more secure.

## Node.js example

1) Make sure that Node.js is installed on your system, you can check it with:
```
node --version
```
2) Clone or download this repository.
3) Open the terminal and change directory to NodeJS-usage directory inside the repository.
4) Start the script with:
```
node .
```
5) You will be asked if you want to encrypt or decrypt some file, enter "e" for encryption, or "d" for decryption mode.
6) Follow further script instructions.

## ESP8266 example

Operation on other microcontrollers is not guaranteed.

1) Clone or download this repository.
2) Open file `Arduino(ESP8266)-implementation.ino` with Arduino IDE.
3) Flash your ESP8266 board with opened sketch.
4) Open serial monitor, set baud rate to 115200 baud, line ending to "No Line Ending" and reset your ESP8266.
5) You should see an info messages being printed to serial monitor.
6) Enter "e" if you want to encrypt data, or "d" to decrypt it.
7) Follow further program instructions.
