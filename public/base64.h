#ifndef __base64_h__
#define __base64_h__
#include "multisys.h"

namespace tools {
    void base64DecodeBlock(u8 * dst, const s8 * src);
    s32 base64Decode(u8 * dst, s32 maxSize, const s8 * src, s32 size);
    s32 base64Encode(s8 * dst, s32 maxSize, const u8 * src, s32 size);
}
#endif //
