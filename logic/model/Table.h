#ifndef __table_h__
#define __table_h__

#include "iModel.h"
#include <vector>
#include <unordered_map>
using namespace std;

class row;
class mlayout;
typedef vector<row *> rows;
typedef unordered_map<s64, s32> int_key_map;
typedef unordered_map<std::string, s32> string_key_map;

class table : public iTable {
public:
    const std::string _file;
    const s32 _line;
    const std::string _name;

    table(iUnit * host, const mlayout * layout, const dc::layout * key, const char * file, const s32 line);
    virtual ~table();

    virtual iUnit * getHoster();

    virtual s32 rowCount() const;
    virtual void clear();
    
    bool addRowFromBlob(const void * data, const s32 size);

    virtual iRow * findRow(const s64 key) const;
    virtual iRow * findRow(const char * key) const;
    virtual iRow * getRow(const s32 row) const;

    virtual iRow * addRow(const char * file, const s32 line);
    virtual iRow * addRowKeyInt64(const s64 key, const char * file, const s32 line);
    virtual iRow * addRowKeyString(const char * key, const char * file, const s32 line);

    virtual bool delRow(const s32 index);
    virtual bool swapRow(const s32 src, const s32 dst);
private:
    const mlayout * _layout;
    const dc::layout * const _key;
    iUnit * const _host;

    rows _rows;
    int_key_map _key_int;
    string_key_map _key_string;
};

#endif //__table_h__
