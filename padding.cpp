#include <gmpxx.h>
#include <iostream>
#include <fstream>
#include "padding.h"

using namespace std;
using namespace YansLibrary;

bool YansLibrary::YansPadding_Encrypt(unsigned char* input, int inbufsize, unsigned char* output, int bufsize)
{
    if(bufsize / inbufsize < 2)
        return false;

    // --- INIT RANDOM NUMBER GENERATOR ---
    ifstream rnd_seed_input("/dev/urandom");
    unsigned long rnd_seed = 0;
    for(int i = 0; i < sizeof(rnd_seed); i++)
        rnd_seed = rnd_seed << 8 | rnd_seed_input.get();
    gmp_randclass rnd(gmp_randinit_default);
    rnd.seed(rnd_seed);
    // ------------------------------------

    int byte_len = bufsize / inbufsize;
    int a_len = byte_len / 2;
    int b_len = byte_len / 2;
    if(a_len * 2 < byte_len) a_len++;
    mpz_class a_max = 1;
    a_max <<= a_len * 8;
    for(int i = 0; i < inbufsize; i++)
    {
        mpz_class a, b, c, a_r_max;
        b = rnd.get_z_bits(b_len * 8);
        if(b < 0xf) b += 0xf;
        c = 257;
        if(!mpz_invert(a.get_mpz_t(), b.get_mpz_t(), c.get_mpz_t()))
        {
            b--;
            mpz_invert(a.get_mpz_t(), b.get_mpz_t(), c.get_mpz_t());
        }
        a = (a * input[i]) % 257;
        a_r_max = (a_max - a) / 257;
        a += rnd.get_z_range(a_r_max) * 257;
        mpz_class next;
        for(int i2 = 0; i2 < a_len; i2++)
        {
            next = a & 0xFF;
            output[i * byte_len + i2] = (unsigned char)next.get_si();
            a >>= 8;
        }
        for(int i2 = 0; i2 < b_len; i2++)
        {
            next = b & 0xFF;
            output[i * byte_len + a_len + i2] = (unsigned char)next.get_si();
            b >>= 8;
        }
    }
    return true;
}

bool YansLibrary::YansPadding_Decrypt(unsigned char* input, int inbufsize, unsigned char* output, int bufsize)
{
    if(inbufsize / bufsize < 2)
        return false;
    int byte_len = inbufsize / bufsize;
    int a_len = byte_len / 2;
    int b_len = byte_len / 2;
    if(a_len * 2 < byte_len) a_len++;
    for(int i = 0; i < bufsize; i++)
    {
        mpz_class a,b,c;
        for(int i2 = a_len - 1; i2 >= 0; i2--)
            a = a << 8 | (unsigned char)input[i * byte_len + i2];
        for(int i2 = b_len - 1; i2 >= 0; i2--)
            b = b << 8 | (unsigned char)input[i * byte_len + a_len + i2];
        c = (a * b) % 257;
        output[i] = (char)c.get_si();
    }
    return true;
}
