#include <stdexcept>
#include <vector>

template <typename T>
class HexTopology
{
    int row_count, col_count;
public:
    HexTopology(const T &m);
    HexTopology(int row_count_, int col_count_);
    std::vector<std::pair<int,int> > operator() (int row, int col) const;
};
 
template <typename T>
HexTopology<T>::HexTopology(const T &m): row_count(), col_count()
{
    row_count = m.get_rows();
    col_count = m.get_cols();
}

template <typename T>
HexTopology<T>::HexTopology(int row_count_, int col_count_):
    row_count(row_count_), col_count(col_count_) {}

template <typename T>
std::vector<std::pair<int,int> > 
HexTopology<T>::operator() (int row, int col) const
{
    if (row < 0 || row >= row_count || col < 0 || col >= col_count) {
        throw std::range_error("Invalid indicies");
    }
    char shift = 1 - (col % 2);
    std::vector<std::pair<int,int> > neighbour;
    int row_min = row > 0 ? row - 1 + shift : 0;
    int row_max = row < row_count - 1 ? row + 1 + shift : row_count;
    int col_min = col > 0 ? col - 1 : 0;
    int col_max = col < col_count - 1 ? col + 1 : col_count - 1;
    if (shift && row > 0) {
        neighbour.push_back(std::pair<int, int>(row - 1, col));
    }
    for (int i = row_min; i < row_max; ++i) {
        for (int j = col_min; j <= col_max; ++j) {
            if (i == row && j == col) {
                continue;
            }
            neighbour.push_back(std::pair<int, int>(i, j));
        }
    }
    if (!shift && row < row_count - 1) {
        neighbour.push_back(std::pair<int, int>(row + 1, col));
    }
    return neighbour;
}