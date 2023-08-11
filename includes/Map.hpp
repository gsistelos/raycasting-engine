#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <vector>

class Map {
   public:
    std::vector<std::vector<char>> grid;

    Map(void);
    Map(const std::string& filename);
    ~Map();

    void validate(void);
    void load(const std::string& filename);

    class FailedToOpenTheMap : public std::exception {
       public:
        const char* what() const throw();
    };

    class InvalidMapGrid : public std::exception {
       public:
        const char* what() const throw();
    };
};

#endif /* MAP_HPP */
