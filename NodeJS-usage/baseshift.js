import * as fs from "fs";
import * as stream from "stream";
import { exec } from "child_process";

const baseShiftFolder = "../BaseShift";

function getRandomInt(max) {
    return Math.floor(Math.random() * max);
}

function updateRand() {
    try {
        fs.writeFileSync(baseShiftFolder + "/.rand_ulong", getRandomInt(4294967296).toString());
    } catch (err) {
        return(`Error writing to .rand_ulong: ${err}`);
    }

    return true;
}

export function encrypt(decrypted) {
    return new Promise((resolve, reject) => {
        const updateRandRes = updateRand();
        if (updateRandRes != true) {
            reject(`Error updating rand: ${updateRandRes}`);
            return;
        }
    
        let program = exec("./encrypt", { cwd: baseShiftFolder, encoding: "buffer" }, (err, stdout, stderr) => {
            if (err) {
                reject(`Exec error: ${err}`);
                return;
            }
    
            if (stderr.length > 0) {
                reject(`Encryption error: ${stderr.toString().slice(0, -1)}`);
                return;
            }
    
            resolve(stdout);
        });
    
        let stdinStream = new stream.Readable();
        stdinStream.push(decrypted);
        stdinStream.push(null);
        stdinStream.pipe(program.stdin);
    });
}

export function decrypt(encrypted) {
    return new Promise((resolve, reject) => {
        let program = exec("./decrypt", { cwd: baseShiftFolder, encoding: "buffer" }, (err, stdout, stderr) => {
            if (err) {
                reject(`Exec error: ${err}`);
                return;
            }
    
            if (stderr.length > 0) {
                reject(`Decryption error: ${stderr.toString().slice(0, -1)}`);
                return;
            }
    
            resolve(stdout);
        });
    
        let stdinStream = new stream.Readable();
        stdinStream.push(encrypted);
        stdinStream.push(null);
        stdinStream.pipe(program.stdin);
    });
}

export function setKey(newKey) {
    return new Promise((resolve, reject) => {
        fs.writeFile(baseShiftFolder + "/.key", newKey, (err) => {
            if (err) {
                reject(`Reset key error: ${err}`);
                return;
            }

            resolve();
        });
    });
}