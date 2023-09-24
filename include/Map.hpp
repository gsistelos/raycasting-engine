#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <vector>

class Map {
   public:
    std::vector<std::vector<char>> grid;

    Map(const std::string& filename);
    ~Map();

    void validate(void);

    class FailedToOpenTheMap : public std::exception {
       public:
        const char* what() const throw();
    };

    class InvalidMapGrid : public std::exception {
       public:
        const char* what() const throw();
    };

   private:
    void validateOnlyZeroLine(size_t y);
};

#endif /* MAP_HPP */
