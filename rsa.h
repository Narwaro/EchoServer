#ifndef RSA_H_INCLUDED
#define RSA_H_INCLUDED

#include <gmpxx.h>

namespace YansLibrary
{
    struct RSA_KeyPair
    {
        unsigned int L;     //KeyLen
        mpz_class p, q;     //Primes
        mpz_class N, phi;   //Modulus, Phi( (p-1)*(q-1) )
        mpz_class e, d;     //Encipher, Decipher
    };

    struct RSA_Key
    {
        unsigned int L;     //KeyLen
        mpz_class N, e;     //Modulus, Exponent
    };

    bool RSA_GenerateKeyPair(struct RSA_KeyPair* keypair, unsigned int keylen, unsigned long rnd_seed);
    void RSA_GetPublicKey(struct RSA_KeyPair* keypair, struct RSA_Key* key);
    void RSA_GetPrivateKey(struct RSA_KeyPair* keypair, struct RSA_Key* key);
    void RSA_Encrypt(struct RSA_Key* key, char* input, char* output);

    void RSA_AtoPublicKey(char* buf, struct RSA_Key* key);
    void RSA_PublicKeyToA(struct RSA_Key* key, char* buf);
    void RSA_AtoKeyPair(char* buf, struct RSA_Key* keypair);
    void RSA_KeyPairToA(struct RSA_KeyPair* keypair, char* buf);

    //NOTE: N >= 2^(L-32) => Maximum Data Length = L - 32 Bit
}

#endif // RSA_H_INCLUDED
