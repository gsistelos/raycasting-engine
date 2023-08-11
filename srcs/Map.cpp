#include "Map.hpp"

#include <fstream>

Map::Map(void) {
}

Map::Map(const std::string& filename) {
    std::ifstream file(filename);
    if (!file)
        throw Map::FailedToOpenTheMap();

    // Read file until "MAP" is found

    std::string line;
    while (std::getline(file, line, '\n')) {
        if (line == "MAP")
            break;
    }

    // Read file into a 2D vector

    char ch;
    std::vector<char> row;
    while (file.get(ch)) {
        if (ch != '\n')
            row.push_back(ch);
        else {
            grid.push_back(row);
            row.clear();
        }
    }
    if (!row.empty())
        grid.push_back(row);

    validate();
}

Map::~Map() {
}

void Map::validate(void) {
    size_t y = 0;
    size_t x = 0;

    // First line

    while (x < grid[y].size()) {
        if (grid[y][x] == '0')
            throw Map::InvalidMapGrid();
        x++;
    }
    y++;

    // Middle lines

    while (y < grid.size() - 1) {
        if (grid[y][0] == '0')
            throw Map::InvalidMapGrid();

        x = 0;
        while (++x < grid[y].size() - 1) {
            if (grid[y][x] != '0')
                continue;
            if (grid[y - 1].size() < x || grid[y + 1].size() < x || grid[y - 1][x] == ' ' || grid[y + 1][x] == ' ' || grid[y][x - 1] == ' ' || grid[y][x + 1] == ' ')
                throw Map::InvalidMapGrid();
        }

        if (grid[y][x] == '0')
            throw Map::InvalidMapGrid();
        y++;
    }

    // Last line

    x = 0;
    while (x < grid[y].size()) {
        if (grid[y][x] == '0')
            throw Map::InvalidMapGrid();
        x++;
    }
}

void Map::load(const std::string& filename) {
    grid.clear();

    std::ifstream file(filename);
    if (!file)
        throw Map::FailedToOpenTheMap();

    // Read file until "MAP" is found

    std::string line;
    while (std::getline(file, line, '\n')) {
        if (line == "MAP")
            break;
    }

    // Read file into a 2D vector

    char ch;
    std::vector<char> row;
    while (file.get(ch)) {
        if (ch != '\n')
            row.push_back(ch);
        else {
            grid.push_back(row);
            row.clear();
        }
    }
    if (!row.empty())
        grid.push_back(row);

    validate();
}

const char* Map::FailedToOpenTheMap::what() const throw() {
    return "Failed to open the map";
}

const char* Map::InvalidMapGrid::what() const throw() {
    return "Invalid map grid";
}
