#include "base64.h"

namespace tools {
    void base64DecodeBlock(u8 * dst, const s8 * src) {
        u32 x = 0;
        for (s32 i = 0; i < 4; ++i) {
            if (src[i] >= 'A' && src[i] <= 'Z')
                x = (x << 6) + (u32)(src[i] - 'A' + 0);
            else if (src[i] >= 'a' && src[i] <= 'z')
                x = (x << 6) + (u32)(src[i] - 'a' + 26);
            else if (src[i] >= '0' && src[i] <= '9')
                x = (x << 6) + (u32)(src[i] - '0' + 52);
            else if (src[i] == '+')
                x = (x << 6) + 62;
            else if (src[i] == '/')
                x = (x << 6) + 63;
            else if (src[i] == '=')
                x = (x << 6);
        }

        dst[2] = (u8)(x & 255);
        x >>= 8;
        dst[1] = (u8)(x & 255);
        x >>= 8;
        dst[0] = (u8)(x & 255);
    }

    // BASE64解码
    s32 base64Decode(u8 * dst, s32 maxSize, const s8 * src, s32 size) {
        s32 len = 0;
        while ((len < size) && (src[len] != '='))
            ++len;

        s32 equalNum = 0;
        while ((len + equalNum < size) && (src[len + equalNum] == '='))
            ++equalNum;

        s32 blockNum = (len + equalNum) / 4;
        tassert(blockNum * 3 - equalNum <= maxSize, "decode over flow");
        if (blockNum * 3 - equalNum > maxSize)
            return maxSize;

        for (s32 i = 0; i < blockNum - 1; ++i) {
            base64DecodeBlock(dst, src);

            dst += 3;
            src += 4;
        }

        u8 lastBlock[3];
        base64DecodeBlock(lastBlock, src);

        for (s32 i = 0; i < 3 - equalNum; ++i)
            dst[i] = lastBlock[i];

        return blockNum * 3 - equalNum;
    }

    // BASE64编码
    s32 base64Encode(s8 * dst, s32 maxSize, const u8 * src, s32 size) {
        static char table64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        tassert(size > 0, "empty source data");
        tassert(maxSize >= (size / 3 + (size % 3 == 0 ? 0 : 1)) * 4 + 1, "empty source data");
        if (size <= 0)
            return 0;
        if (maxSize < (size / 3 + (size % 3 == 0 ? 0 : 1)) * 4 + 1)
            return maxSize;

        const u8 * in = src;
        s8* out = dst;

        while (size > 0) {
            u8 ibuf[3];
            u8 obuf[4];
            s32 inNum = 0;

            for (s32 i = 0; i < 3; ++i) {
                if (size > 0) {
                    inNum++;
                    ibuf[i] = *(in++);
                    --size;
                }
                else
                    ibuf[i] = 0;
            }

            obuf[0] = (ibuf[0] & 0xFC) >> 2;
            obuf[1] = ((ibuf[0] & 0x03) << 4) | ((ibuf[1] & 0xF0) >> 4);
            obuf[2] = ((ibuf[1] & 0x0F) << 2) | ((ibuf[2] & 0xC0) >> 6);
            obuf[3] = ibuf[2] & 0x3F;

            switch (inNum) {
            case 1:
                out[0] = table64[obuf[0]];
                out[1] = table64[obuf[1]];
                out[2] = '=';
                out[3] = '=';
                break;
            case 2:
                out[0] = table64[obuf[0]];
                out[1] = table64[obuf[1]];
                out[2] = table64[obuf[2]];
                out[3] = '=';
                break;
            default:
                out[0] = table64[obuf[0]];
                out[1] = table64[obuf[1]];
                out[2] = table64[obuf[2]];
                out[3] = table64[obuf[3]];
                break;
            }
            out += 4;
        }
        *out = 0;
        return (s32)(out - dst);
    }
}
