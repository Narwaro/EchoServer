#include <iostream>
#include <fstream>
#include "rsa.h"

using namespace std;

bool YansLibrary::RSA_GenerateKeyPair(struct RSA_KeyPair* keypair, unsigned int keylen, unsigned long rnd_seed)
{
    if(keylen % 8)
        return false;

    gmp_randclass rnd(gmp_randinit_default);
    rnd.seed(rnd_seed);

    mpz_class N_min = 1;
    N_min <<= keylen - 32;

    keypair->L = keylen;
    //Generate Primes(p,q)
    keypair->p = rnd.get_z_bits(keypair->L / 2);
    keypair->q = rnd.get_z_bits(keypair->L / 2);
    keypair->N = keypair->p * keypair->q;
    if(keypair->N < N_min)
        keypair->p += (N_min - keypair->N) / keypair->q + 1;
    
    mpz_nextprime(keypair->p.get_mpz_t(), keypair->p.get_mpz_t());
    mpz_nextprime(keypair->q.get_mpz_t(), keypair->q.get_mpz_t());

    //Calculate Modulus(N), phi, Encipher(e) and Decipher(d)
    keypair->N = keypair->p * keypair->q;
    keypair->phi = (keypair->p - 1) * (keypair->q - 1);
    keypair->e = 0x10001;
    mpz_invert(keypair->d.get_mpz_t(), keypair->e.get_mpz_t(), keypair->phi.get_mpz_t());
    return true;
}

void YansLibrary::RSA_GetPublicKey(struct RSA_KeyPair* keypair, struct RSA_Key* key)
{
    key->L = keypair->L;
    key->N = keypair->N;
    key->e = keypair->e;
}

void YansLibrary::RSA_GetPrivateKey(struct RSA_KeyPair* keypair, struct RSA_Key* key)
{
    key->L = keypair->L;
    key->N = keypair->N;
    key->e = keypair->d;
}

void YansLibrary::NumberToArray(mpz_class n, char* buf, int bufsize)
{
    mpz_class next;
    for(int i = 0; i < bufsize; i++)
    {
        next = n & 0xFF;
        buf[i] = (unsigned char)next.get_si();
        n >>= 8;
    }
}

void YansLibrary::ArrayToNumber(char* buf, int bufsize, mpz_class &n)
{
    n = 0;
    for(int i = bufsize - 1; i >= 0; i--)
        n = n << 8 | (unsigned char)buf[i];
}

void YansLibrary::RSA_Encrypt(struct RSA_Key* key, char* input, char* output)
{
    mpz_class M, C;
    ArrayToNumber(input, key->L / 8, M);
    mpz_powm(C.get_mpz_t(), M.get_mpz_t(), key->e.get_mpz_t(), key->N.get_mpz_t());
    NumberToArray(C, output, key->L / 8);
}

void YansLibrary::RSA_AtoPublicKey(char* buf, struct RSA_Key* key)
{
    unsigned int* keylen = (unsigned int*)buf;
    buf += sizeof(unsigned int);
    key->L = *keylen;
    ArrayToNumber(buf, key->L / 8, key->N);
    key->e = 65537;
}

void YansLibrary::RSA_PublicKeyToA(struct RSA_Key* key, char* buf)
{
    unsigned int* keylen = (unsigned int*)buf;
    buf += sizeof(unsigned int);
    *keylen = key->L;
    NumberToArray(key->N, buf, key->L / 8);
}
