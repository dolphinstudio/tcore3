#ifndef __Row_h__
#define __Row_h__
#include "Header.h"

class Row : public IRow {
public:
    const std::string _file;
    const s32 _line;
    s32 _row;

    Row(const s32 row, const MemonyLayout * layout, const char * file, const s32 line);
    ~Row();

    virtual const void * GetData() const {
        return _memony->GetData();
    }

    virtual const s32 GetDataSize() const {
        return _memony->GetSize();
    }

    virtual bool SetData(const void * data, const s32 len);

    virtual s32 GetRowIndex() const;
    virtual void SetRowIndex(const s32 index);

    virtual s64 GetDataInt64(const s32 column) const;
    virtual float GetDataFloat(const s32 column) const;
    virtual bool GetDataBool(const s32 column) const;
    virtual const char * GetDataString(const s32 column) const;

    virtual bool SetDataInt64(const s32 column, const s64 value);
    virtual bool SetDataFloat(const s32 column, const float value);
    virtual bool SetDataBool(const s32 column, const bool value);
    virtual bool SetDataString(const s32 column, const char * value);

private:
    const MemonyLayout * _layout;
    Memony * _memony;
};

#endif //__Row_h__
