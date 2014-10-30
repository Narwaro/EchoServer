#ifndef PADDING_H_INCLUDED
#define PADDING_H_INCLUDED

namespace YansLibrary
{
    bool YansPadding_Encrypt(unsigned char* input, int inbufsize, unsigned char* output, int bufsize);
    bool YansPadding_Decrypt(unsigned char* input, int inbufsize, unsigned char* output, int bufsize);
}

#endif // PADDING_H_INCLUDED
