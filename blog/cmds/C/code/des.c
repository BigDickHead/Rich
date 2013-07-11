DES encryption with OpenSSL: example in C

This is a simple example that shows how to encrypt and decrypt data using the DES implementation provided by OpenSSL.

Encrypt/Decrypt functions

We will define two functions, Encrypt and Decrypt. They both use the function DES_cfb64_encrypt to encrypt and decrypt data. See comments inline for explanations.

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/des.h>


char *Encrypt(char *Key, char *Msg, int size)
{

        static char*    Res;
        int             n=0;
        DES_cblock      Key2;
        DES_key_schedule schedule;

        Res = ( char * ) malloc( size );

        /* Prepare the key for use with DES_cfb64_encrypt */
        memcpy( Key2, Key,8);
        DES_set_odd_parity( &Key2 );
        DES_set_key_checked( &Key2, &schedule );

        /* Encryption occurs here */
        DES_cfb64_encrypt( ( unsigned char * ) Msg, ( unsigned char * ) Res,
                           size, &schedule, &Key2, &n, DES_ENCRYPT );

         return (Res);
}


char *
Decrypt( char *Key, char *Msg, int size)
{

        static char*    Res;
        int             n=0;

        DES_cblock      Key2;
        DES_key_schedule schedule;

        Res = ( char * ) malloc( size );

        /* Prepare the key for use with DES_cfb64_encrypt */
        memcpy( Key2, Key,8);
        DES_set_odd_parity( &Key2 );
        DES_set_key_checked( &Key2, &schedule );

        /* Decryption occurs here */
        DES_cfb64_encrypt( ( unsigned char * ) Msg, ( unsigned char * ) Res,
                           size, &schedule, &Key2, &n, DES_DECRYPT );

        return (Res);

}

Note that the functions above assume a key of 8 characters. If a longer key is provided, only the first 8 bytes will be used. To support longer keys, additional processing and concatenation work must be done to prepare key2.

Example

For testing purpose, this is a main function that calls the Encrypt and Decrypt functions shown above.

int main() {

char key[]="password";
char clear[]="This is a secret message";
char *decrypted;
char *encrypted;

encrypted=malloc(sizeof(clear));
decrypted=malloc(sizeof(clear));

printf("Clear text\t : %s \n",clear);
memcpy(encrypted,Encrypt(key,clear,sizeof(clear)), sizeof(clear));
printf("Encrypted text\t : %s \n",encrypted);
memcpy(decrypted,Decrypt(key,encrypted,sizeof(clear)), sizeof(clear));
printf("Decrypted text\t : %s \n",decrypted);



return (0);
}


To compile this code, copy the three functions Encrypt, Decrypt and main in a file (e.g. test.c), then compile as follows

gcc test.c -o test -lssl

The -lssl flag allows the code to be linked against the OpenSSL library.

If you don't have the OpenSSL library, you can install it using apt

sudo apt-get install libssl-dev

Once the program compiled, you can test it

$./test
Clear text   : This is a secret message
Enecrypted text  : 8��0t�E�E� \@�Z9T�)��a�
Decrypted text   : This is a secret message
