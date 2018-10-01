#include "sha1.h"

namespace tlib {
    sha1::sha1() {
        reset();
    }

    sha1::~sha1() {
    }

    void sha1::reset() {
        _length_low = 0;
        _length_high = 0;
        _message_block_Index = 0;

        _h[0] = 0x67452301;
        _h[1] = 0xEFCDAB89;
        _h[2] = 0x98BADCFE;
        _h[3] = 0x10325476;
        _h[4] = 0xC3D2E1F0;

        _computed = false;
        _corrupted = false;
    }

    bool sha1::result(unsigned *message_digest_array) {
        int i;                                  // Counter

        if (_corrupted) {
            return false;
        }

        if (!_computed) {
            padMessage();
            _computed = true;
        }

        for (i = 0; i < 5; i++) {
            message_digest_array[i] = _h[i];
        }

        return true;
    }

    void sha1::input(const unsigned char * message_array, unsigned length) {
        if (!length) {
            return;
        }

        if (_computed || _corrupted) {
            _corrupted = true;
            return;
        }

        while (length-- && !_corrupted) {
            _message_block[_message_block_Index++] = (*message_array & 0xFF);

            _length_low += 8;
            _length_low &= 0xFFFFFFFF;               // Force it to 32 bits
            if (_length_low == 0) {
                _length_high++;
                _length_high &= 0xFFFFFFFF;          // Force it to 32 bits
                if (_length_high == 0) {
                    _corrupted = true;               // Message is too long
                }
            }

            if (_message_block_Index == 64) {
                processMessageBlock();
            }

            message_array++;
        }
    }

    void sha1::input(const char  *message_array, unsigned length) {
        input((unsigned char *)message_array, length);
    }

    void sha1::input(unsigned char message_element) {
        input(&message_element, 1);
    }

    void sha1::input(char message_element) {
        input((unsigned char *)&message_element, 1);
    }

    sha1 & sha1::operator<<(const char *message_array) {
        const char *p = message_array;

        while (*p) {
            input(*p);
            p++;
        }

        return *this;
    }

    sha1& sha1::operator<<(const unsigned char *message_array) {
        const unsigned char *p = message_array;

        while (*p) {
            input(*p);
            p++;
        }

        return *this;
    }

    sha1 & sha1::operator << (const char message_element) {
        input((unsigned char *)&message_element, 1);
        return *this;
    }

    sha1 & sha1::operator << (const unsigned char message_element) {
        input(&message_element, 1);
        return *this;
    }

    void sha1::processMessageBlock() {
        const unsigned K[] = {               // Constants defined for SHA-1
            0x5A827999,
            0x6ED9EBA1,
            0x8F1BBCDC,
            0xCA62C1D6
        };
        int         t;                          // Loop counter
        unsigned    temp;                       // Temporary word value
        unsigned    W[80];                      // Word sequence
        unsigned    A, B, C, D, E;              // Word buffers

                                                /*
                                                *  initialize the first 16 words in the array W
                                                */
        for (t = 0; t < 16; t++) {
            W[t] = ((unsigned)_message_block[t * 4]) << 24;
            W[t] |= ((unsigned)_message_block[t * 4 + 1]) << 16;
            W[t] |= ((unsigned)_message_block[t * 4 + 2]) << 8;
            W[t] |= ((unsigned)_message_block[t * 4 + 3]);
        }

        for (t = 16; t < 80; t++) {
            W[t] = circularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
        }

        A = _h[0];
        B = _h[1];
        C = _h[2];
        D = _h[3];
        E = _h[4];

        for (t = 0; t < 20; t++) {
            temp = circularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
            temp &= 0xFFFFFFFF;
            E = D;
            D = C;
            C = circularShift(30, B);
            B = A;
            A = temp;
        }

        for (t = 20; t < 40; t++) {
            temp = circularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
            temp &= 0xFFFFFFFF;
            E = D;
            D = C;
            C = circularShift(30, B);
            B = A;
            A = temp;
        }

        for (t = 40; t < 60; t++) {
            temp = circularShift(5, A) +
                ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
            temp &= 0xFFFFFFFF;
            E = D;
            D = C;
            C = circularShift(30, B);
            B = A;
            A = temp;
        }

        for (t = 60; t < 80; t++) {
            temp = circularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
            temp &= 0xFFFFFFFF;
            E = D;
            D = C;
            C = circularShift(30, B);
            B = A;
            A = temp;
        }

        _h[0] = (_h[0] + A) & 0xFFFFFFFF;
        _h[1] = (_h[1] + B) & 0xFFFFFFFF;
        _h[2] = (_h[2] + C) & 0xFFFFFFFF;
        _h[3] = (_h[3] + D) & 0xFFFFFFFF;
        _h[4] = (_h[4] + E) & 0xFFFFFFFF;

        _message_block_Index = 0;
    }

    void sha1::padMessage() {
        if (_message_block_Index > 55) {
            _message_block[_message_block_Index++] = 0x80;
            while (_message_block_Index < 64) {
                _message_block[_message_block_Index++] = 0;
            }

            processMessageBlock();

            while (_message_block_Index < 56) {
                _message_block[_message_block_Index++] = 0;
            }
        } else {
            _message_block[_message_block_Index++] = 0x80;
            while (_message_block_Index < 56) {
                _message_block[_message_block_Index++] = 0;
            }

        }

        _message_block[56] = (_length_high >> 24) & 0xFF;
        _message_block[57] = (_length_high >> 16) & 0xFF;
        _message_block[58] = (_length_high >> 8) & 0xFF;
        _message_block[59] = (_length_high) & 0xFF;
        _message_block[60] = (_length_low >> 24) & 0xFF;
        _message_block[61] = (_length_low >> 16) & 0xFF;
        _message_block[62] = (_length_low >> 8) & 0xFF;
        _message_block[63] = (_length_low) & 0xFF;

        processMessageBlock();
    }

    unsigned sha1::circularShift(int bits, unsigned word) {
        return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32 - bits));
    }
}
