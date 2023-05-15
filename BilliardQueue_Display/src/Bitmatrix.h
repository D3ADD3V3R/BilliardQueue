#include <initializer_list>

using std::initializer_list;

class Bitmatrix {
public:
    Bitmatrix(initializer_list<initializer_list<bool>> values)
    {
        data = new bool*[values.size()];
        int i = 0;
        for (initializer_list<bool> column : values) {
            data[i] = new bool[column.size()];
            int j = 0;
            for (bool value : column) {
                data[i][j++] = value;
            }
            i++;
        }
    }

    bool& operator()(int row, int col) {
        return data[row][col];
    }
    // rest of the class definition
private:
    bool** data;
};
