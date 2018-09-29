#ifndef __Table_h__
#define __Table_h__

#include "IModel.h"
#include <vector>
#include <unordered_map>
using namespace std;

class Row;
class MemonyLayout;
typedef vector<Row *> ROWS;
typedef unordered_map<s64, s32> INT_KEY_MAP;
typedef unordered_map<std::string, s32> STRING_KEY_MAP;

class Table : public ITable {
public:
    const std::string _file;
    const s32 _line;
    const std::string _name;

    Table(IUnit * host, const MemonyLayout * layout, const dc::layout * key, const char * file, const s32 line);
    virtual ~Table();

    virtual IUnit * GetHost();

    virtual s32 RowCount() const;
    virtual void ClearRows();
    
    bool AddRowFromBlob(const void * data, const s32 size);

    virtual IRow * FindRow(const s64 key) const;
    virtual IRow * FindRow(const char * key) const;
    virtual IRow * GetRow(const s32 row) const;

    virtual IRow * AddRow(const char * file, const s32 line);
    virtual IRow * AddRowKeyInt64(const s64 key, const char * file, const s32 line);
    virtual IRow * AddRowKeyString(const char * key, const char * file, const s32 line);

    virtual bool DelRow(const s32 index);
    virtual bool SwapRow(const s32 src, const s32 dst);
private:
    const MemonyLayout * _layout;
    const dc::layout * const _key;
    IUnit * const _host;

    ROWS _rows;
    INT_KEY_MAP _key_int;
    STRING_KEY_MAP _key_string;
};

#endif //__Table_h__
