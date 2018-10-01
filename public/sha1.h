#ifndef __sha1_h__
#define __sha1_h__

namespace tlib {
    class sha1 {
    public:
        sha1();
        virtual ~sha1();

        /*
        *  Re-initialize the class
        */
        void reset();

        /*
        *  Returns the message digest
        */
        bool result(unsigned *message_digest_array);

        /*
        *  Provide input to sha1
        */
        void input(const unsigned char *message_array,
            unsigned            length);
        void input(const char  *message_array,
            unsigned    length);
        void input(unsigned char message_element);
        void input(char message_element);
        sha1& operator<<(const char *message_array);
        sha1& operator<<(const unsigned char *message_array);
        sha1& operator<<(const char message_element);
        sha1& operator<<(const unsigned char message_element);

    private:

        /*
        *  Process the next 512 bits of the message
        */
        void processMessageBlock();

        /*
        *  Pads the current message block to 512 bits
        */
        void padMessage();

        /*
        *  Performs a circular left shift operation
        */
        inline unsigned circularShift(int bits, unsigned word);

        unsigned _h[5];                      // Message digest buffers

        unsigned _length_low;                // Message length in bits
        unsigned _length_high;               // Message length in bits

        unsigned char _message_block[64];    // 512-bit message blocks
        int _message_block_Index;            // Index into message block array

        bool _computed;                      // Is the digest computed?
        bool _corrupted;                     // Is the message digest corruped?
    };
}

    
#endif // __sha1_h__
