![alt text](https://img.icons8.com/officel/2x/security-aes.png)
# GCM-C
## Schoolar Project Don't use in Production
### Basic implementation of GCM encryption algorithm in C.

Algorithm description:<br>
This implementation reads data from an input file, generates the crypted text file, and decrypte it in other file.<br>
It uses MD5 instead of Galois Field Multiplication for initial hash generating. I tried to implement bitwise GF Multiplication in byte_multiplication.c but I was not successful.

 Done:
- [x] generate auth tag
- [x] checks auth tag
- [x] generates ecnrypted file
- [x] generates decrypted file

 To do:
- [ ] export AES_KEY to file
- [ ] multiplication in GF
- [ ] function encapsulation

### How to run
#### This program was developed in Linux

* compile using -lcrypto
   <br> ex: gcc ./gcm.c -o gcm -lcrypto


Using PKCS#7 padding
