#ifndef __row_h__
#define __row_h__
#include "header.h"

class row : public iRow {
public:
    const std::string _file;
    const s32 _line;
    s32 _row;

    row(const s32 row, const mlayout * layout, const char * file, const s32 line);
    ~row();

    virtual const void * getData(OUT s32 & size) const {
        size = _memony->getSize();
        return _memony->getData();
    }

    virtual bool setData(const void * data, const s32 len);

    virtual s32 getIndex() const;
    virtual void setIndex(const s32 index);

    virtual s64 getInt64(const s32 column) const;
    virtual float getFloat(const s32 column) const;
    virtual bool getBool(const s32 column) const;
    virtual const char * getString(const s32 column) const;

    virtual bool setInt64(const s32 column, const s64 value);
    virtual bool setFloat(const s32 column, const float value);
    virtual bool setBool(const s32 column, const bool value);
    virtual bool setString(const s32 column, const char * value);

private:
    const mlayout * _layout;
    memony * _memony;
};

#endif //__row_h__
