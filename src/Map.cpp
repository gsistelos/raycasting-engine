#include "Map.hpp"

#include <fstream>

Map::Map(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw Map::FailedToOpenTheMap();

    std::string line;

    // Read lines until "MAP" line is found

    while (std::getline(file, line, '\n')) {
        if (line == "MAP")
            break;
    }

    if (file.eof())
        throw Map::InvalidMapGrid();

    // Read lines as a 2D vector to grid

    while (std::getline(file, line)) {
        std::vector<char> row(line.begin(), line.end());
        grid.push_back(row);
    }

    validate();

    file.close();
}

Map::~Map() {
}

void Map::validate(void) {
    size_t x = 0;
    size_t y = 0;

    // First line

    validateOnlyZeroLine(y);
    y++;

    // Middle lines

    while (y < grid.size() - 1) {
        if (grid[y][0] == '0')
            throw Map::InvalidMapGrid();

        x = 0;
        while (++x < grid[y].size() - 1) {
            if (grid[y][x] != '0')
                continue;

            if (grid[y - 1].size() < x || grid[y + 1].size() < x ||
                grid[y - 1][x] == ' ' || grid[y + 1][x] == ' ' ||
                grid[y][x - 1] == ' ' || grid[y][x + 1] == ' ')
                throw Map::InvalidMapGrid();
        }

        if (grid[y][x] == '0')
            throw Map::InvalidMapGrid();
        y++;
    }

    // Last line

    validateOnlyZeroLine(y);
}

const char* Map::FailedToOpenTheMap::what() const throw() {
    return "Failed to open the map";
}

const char* Map::InvalidMapGrid::what() const throw() {
    return "Invalid map grid";
}

void Map::validateOnlyZeroLine(size_t y) {
    size_t x = 0;

    while (x < grid[y].size()) {
        if (grid[y][x] == '0')
            throw Map::InvalidMapGrid();
        x++;
    }
}
