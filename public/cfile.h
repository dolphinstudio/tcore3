#ifndef __cfile_h__
#define __cfile_h__

#include "multisys.h"
#include "tools.h"
#include <string.h>

using namespace tools;

#define max_path_len 512

namespace tlib {
	struct cdata {
		void * _date;
		s32 _len;
		cdata(void * date, s32 len) :_date(date), _len(len) {};
	};

	class cfile {
	public:
		cfile() :_file(NULL) {};

		cfile(const char * path, bool clear = false) :_file(NULL) {
            if (clear) {
                _file = fopen(path, "w+");
            } else {
                _file = fopen(path, "ab+");
            }
            tassert(_file, "wtf");
		}

		~cfile() {
            if (_file) {
                fflush(_file);
                fclose(_file);
            }
		}

        inline bool open(const char * path, bool clear = false) {
            if (clear) {
                _file = fopen(path, "w+");
            }
            else {
                _file = fopen(path, "ab+");
            }
            tassert(_file, "wtf");
            return _file != NULL;
        }

        inline bool readtostring(std::string & data) {
            if (_file) {
                fseek(_file, 0, SEEK_END);
                long lsize = ftell(_file);
                rewind(_file);

                char * temp = (char *)alloca(lsize * sizeof(char));
                memset(temp, 0, lsize * sizeof(char));
                s32 result = fread(temp, 1, lsize, _file);//��_file�����ݶ���preadָ���ڴ���
                if (result == lsize * sizeof(char)) {
                    data.append(temp, lsize * sizeof(char));
                    return true;
                }
            }

            return false;
        }

		inline void save() {
            if (_file) {
                fflush(_file);
            }
        }

        inline void clear() {
            if (_file) {
            }
        }

        inline void close() {
            if (_file) {
                fclose(_file);
                _file = NULL;
            }
        }

        cfile & operator << (const s8 data) {
            tassert(_file, "wtf");
            if (_file) {
                fwrite(&data, sizeof(data), 1, _file);
            }
        }

        cfile & operator << (const s16 data) {
            tassert(_file, "wtf");
            if (_file) {
                fwrite(&data, sizeof(data), 1, _file);
            }
            return *this;
        }

        cfile & operator << (const s32 data) {
            tassert(_file, "wtf");
            if (_file) {
                fwrite(&data, sizeof(data), 1, _file);
            }
            return *this;
        }

        cfile & operator << (const s64 data) {
            tassert(_file, "wtf");
            if (_file) {
                fwrite(&data, sizeof(data), 1, _file);
            }
            return *this;
        }

        cfile & operator << (const float data) {
            tassert(_file, "wtf");
            if (_file) {
                fwrite(&data, sizeof(data), 1, _file);
            }
            return *this;
        }

        cfile & operator << (const char * data) {
            tassert(_file, "wtf");
            if (_file) {
                fwrite(data, strlen(data), 1, _file);
            }
            return *this;
        }

        cfile & operator << (const cdata & data) {
            tassert(_file, "wtf");
            if (_file) {
                fwrite(data._date, data._len, 1, _file);
            }
            return *this;
		}

    private:
        FILE * _file;
	};
}

#endif __cfile_h__
