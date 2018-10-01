#ifndef __header_h__
#define __header_h__

#include "multisys.h"
#include "tools.h"
#include "cfile.h"
#include "excel/book.hpp"
#include "excel/exceptions.hpp"
#include "excel/compoundfile/compoundfile_exceptions.hpp"

#include <map>
using namespace tools;

#define config_name_row 0
#define config_type_row 1
#define config_struct_row 2
#define config_desc_row 3
#define config_data_begin 4

#define struct_name_column 1
#define struct_count_column 1

#define member_type_column 0
#define member_name_column 1
#define member_desc_column 2

#endif //__header_h__
