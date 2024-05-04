import { encrypt, decrypt, setKey } from "./baseshift.js";
import * as readline from "readline";
import * as fs from "fs";

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

rl.question("Encrypt(e) or decrypt(d)?: ", operation => {
    if (operation == "e") {
        rl.question("Path to file: ", filePath => {
            fs.readFile(filePath, (err, data) => {
                if (err) {
                    console.error(err);
                    process.exit()
                }

                rl.question("Key for encryption: ", key => {
                    // Saving user key into BaseShift folder
                    setKey(key).then(() => {
                        encrypt(data).then((encrypted) => {
                            console.log("\nFile is successfully encrypted!");
                            
                            // Resetting key to empty string
                            setKey("").then(() => {
                                rl.question("Where do you want to save it?: ", newFilePath => {
                                    fs.writeFile(newFilePath, encrypted, (err) => {
                                        if (err) {
                                            console.error(err);
                                            process.exit()
                                        };
                    
                                        console.log("Encrypted file is successfully saved!");
                                        rl.close();
                                    });
                                });
                            }).catch((err) => {
                                console.error(err);
                                process.exit()
                            });
                        }).catch((err) => {
                            console.error(err);
                            process.exit()
                        });
                    }).catch((err) => {
                        console.error(err);
                        process.exit();
                    });
                });
            });
        });
    } else if (operation == "d") {
        rl.question("Path to encrypted file: ", filePath => {
            fs.readFile(filePath, (err, data) => {
                if (err) {
                    console.error(err);
                    process.exit()
                }
                rl.question("Key for decryption: ", key => {
                    // Saving user key into BaseShift folder
                    setKey(key).then(() => {
                        decrypt(data).then((decrypted) => {
                            console.log("\nFile is successfully decrypted!");

                            // Resetting key to empty string
                            setKey("").then(() => {
                                rl.question("Where do you want to save it?: ", newFilePath => {
                                    fs.writeFile(newFilePath, decrypted, (err) => {
                                        if (err) {
                                            console.error(err);
                                            process.exit()
                                        };
                    
                                        console.log("Decrypted file is successfully saved!");
                                        rl.close();
                                    });
                                });
                            }).catch((err) => {
                                console.error(err);
                                process.exit()
                            });
                        }).catch((err) => {
                            console.error(err);
                            process.exit()
                        });
                    });
                });
            });
        });
    } else {
        rl.close();
    }
});