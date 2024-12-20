#include <fstream>
#include <vector>
#include <string>

namespace CsvUtil {
    void save_csv(const std::string& filename, const std::vector<std::string> fields, const std::vector<std::vector<std::string>>& data) {
        std::ofstream file(filename);

        for (const auto& field : fields) {
            file << field << ",";
        }
        file << std::endl;

        for (const auto& row : data) {
            for (const auto& cell : row) {
                file << cell << ",";
            }
            file << std::endl;
        }

        file.close();
    }

    void add_csv_row(std::string filename, std::vector<std::string> row) {
        std::ofstream file(filename
            , std::ios_base::app | std::ios_base::out);

        for (const auto& cell : row) {
            file << cell << ",";
        }
        file << std::endl;

        file.close();
    }
}