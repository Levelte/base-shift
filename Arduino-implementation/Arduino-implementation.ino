void setup(){
  Serial.begin(115200);
  delay(10);
  Serial.println("BaseShift arduino example");
}
 
void loop() {
    unsigned char* encrypted = (unsigned char*)malloc(32);
    int error = encrypt(0, 0, encrypted);
    if (error != 0) {
      Serial.print("Encryption error ");
      Serial.print(error);
      Serial.println();
    }

    delay(1000);
}