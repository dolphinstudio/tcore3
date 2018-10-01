#include "xmlmaker.h"

bool createxml(const std::string & excel, const std::string & name, const std::string & xml) {
    Excel::Book book(excel);
    Excel::Sheet * sheet = book.sheet(0);

    std::string value;
    std::vector<std::string> column_names;

    value << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
    value << "<root>\n";

    if (sheet->rowsCount() < 2 && sheet->columnsCount() >= 1) {
        tassert(false, "file %s row or column error", excel.c_str());
        return false;
    }

    for (s32 i = 0; i < sheet->columnsCount(); i++) {
        std::string name = tools::toMultiString(sheet->cell(config_name_row, i).getString().c_str());
        column_names.push_back(name);
    }

    for (s32 row = config_data_begin; row < sheet->rowsCount(); row++) {
        value << "    <" << name;
        std::vector<std::string> datas;
        for (s32 clm = 0; clm < sheet->columnsCount(); clm++) {
            std::string data;
            const Excel::Cell & cell = sheet->cell(row, clm);
            switch (cell.dataType()) {
            case Excel::Cell::DataType::String:
                data = tools::toMultiString(cell.getString().c_str());
                break;
            case Excel::Cell::DataType::Double:
                data = tools::floatAsString(cell.getDouble());
                break;
            case Excel::Cell::DataType::Formula:
                switch (cell.getFormula().valueType()) {
                case Excel::Formula::StringValue:
                    data = tools::toMultiString(cell.getFormula().getString().c_str());
                    break;
                case Excel::Formula::DoubleValue:
                    data = tools::floatAsString(cell.getFormula().getDouble());
                    break;
                default:
                    tassert(false, "unknown formula res type");
                    return false;
                }
                break;
            }
            value << " " << column_names[clm] << "=\"" << tools::toUtf8(data.c_str()) << "\"";
        }
        value << " />\n";
    }

    value << "</root>\n";

    std::string xmlpath;
    xmlpath << xml << "/xmlconfig/" << name << ".xml";
    tlib::cfile file(xmlpath.c_str(), true);
    file << value.c_str();
    file.save();
    file.close();

    return true;
}
