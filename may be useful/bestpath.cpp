#include <vector>
#include <stdexcept>
#include <limits>
#include <algorithm>

namespace Game {
    class Cell;
    bool get_first(const Matrix<Cell> &mtx, IntCoord &c);
    std::vector<IntCoord> reconstruct_path(const Matrix<Cell> &field, 
        const IntCoord &c1, const IntCoord &c2);
    template<typename M, typename F, typename T> std::vector<IntCoord> 
        bestpath(const M &mtx, const IntCoord &c1, const IntCoord &c2, 
        const F &func, const T &topology);
    template<typename M, typename F> std::vector<IntCoord> 
        bestpath(const M &mtx, const IntCoord &c1, const IntCoord &c2, 
        const F &func);
    template<typename M> std::vector<IntCoord> 
        bestpath(const M &mtx, const IntCoord &c1, const IntCoord &c2);
};

class Game::Cell
{
    int dist;
    int num;
    bool set;
    Game::IntCoord from;
public:
    Cell(double dist_ = std::numeric_limits<double>::max(), int num_ = 0): 
        dist(dist_), num(num_), set(false), from(IntCoord(-1, -1)) {}
    Cell(const Cell &c): dist(c.dist), num(c.num), set(c.set), from(c.from) {}
    int get_dist(void) const;
    void set_dist(double val);
    int get_num(void) const;
    void set_num(int val);
    bool is_closed(void) const;
    void close(void);
    void open(void);
    IntCoord get_from(void) const;
    void set_from(IntCoord from_);
    
    friend bool operator < (const Cell &c1, const Cell &c2);
    friend bool operator == (const Cell &c1, const Cell &c2);
};



template<typename M, typename F, typename T>
std::vector<Game::IntCoord> 
Game::bestpath(const M &mtx, const Game::IntCoord &c1, const Game::IntCoord &c2, 
    const F &func, const T &topology)
{
    if (c1.get_row() < 0 || c1.get_row() >= mtx.get_rows() 
        || c1.get_col() < 0 || c1.get_col() >= mtx.get_cols()) {
        throw std::range_error("Invalid start");
    }
    if (c2.get_row() < 0 || c2.get_row() >= mtx.get_rows() 
        || c2.get_col() < 0 || c2.get_col() >= mtx.get_cols()) {
        throw std::range_error("Invalid finish");
    }
    std::vector<IntCoord> tmp_cells;
    Matrix<Cell> field(mtx.get_rows(), mtx.get_cols());
    IntCoord c;
    field.at(c1).set_dist(0);
    field.at(c1).open();
    int count = 0;
    int cur_dist = 0;
    while (get_first(field, c)) {
        //cout << c << endl;
        field.at(c).close();
        if (c == c2) {
            return reconstruct_path(field, c1, c2);
        }
        cur_dist = field.at(c).get_dist();
        tmp_cells = topology(c);
        std::vector<IntCoord>::const_iterator ii;
        for (ii = tmp_cells.begin(); ii != tmp_cells.end(); ++ii) {
            if (field.at(*ii).get_dist() < cur_dist + func(mtx, c, *ii)) {
                continue;
            }
            IntCoord from = field.at(*ii).get_from();
            if ((field.at(*ii).get_dist() == cur_dist + func(mtx, c, *ii)) &&
                (field.at(c).get_num() > field.at(from).get_num())) {
                continue;
            }
            field.at(*ii).set_num(++count);
            field.at(*ii).set_dist(cur_dist + func(mtx, c, *ii));
            field.at(*ii).set_from(c);
            field.at(*ii).open();
            //cout << "COUNT: " << count << " " << *ii << endl;
        }
    }
    return std::vector<IntCoord>();
}

std::vector<Game::IntCoord> 
Game::reconstruct_path(const Matrix<Game::Cell> &field, 
    const IntCoord &start, const IntCoord &finish)
{
    IntCoord cur = finish;
    std::vector<IntCoord> path;
    while (cur != start) {
        path.push_back(cur);
        cur = field.at(cur).get_from();
    }
    path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}

bool
Game::get_first(const Matrix<Game::Cell> &field, Game::IntCoord &c)
{
    int i, j;
    IntCoord coord;
    Cell min;
    bool fl = false;
    for (i = 0; i < field.get_rows(); ++i) {
        for (j = 0; j < field.get_cols(); ++j) {
            if (!field.at(i, j).is_closed()) {
                fl = true;
                coord = IntCoord(i, j);
                min = field.at(coord);
                break;
            }
        }
        if (fl) {
            break;
        }
    }
    if (!fl) {
        return false;
    }
    for ( ; i < field.get_rows(); ++i) {
        for (j = 0 ; j < field.get_cols(); ++j) {
            if (field.at(i, j).is_closed()) {
                continue;
            }
            if (field.at(i, j) < min) {
                min = field.at(i, j);
                coord = IntCoord(i, j);
            }
        }
    }
    
    c = coord;
    return true;
}

template<typename M, typename F>
std::vector<Game::IntCoord> 
Game::bestpath(const M &mtx, const Game::IntCoord &c1, 
    const Game::IntCoord &c2, const F &func)
{
    return bestpath(mtx, c1, c2, func, Game::HexTopology<M>(mtx));
}

template<typename M>
std::vector<Game::IntCoord> 
Game::bestpath(const M &mtx, const Game::IntCoord &c1, const Game::IntCoord &c2)
{
    return bestpath(mtx, c1, c2, OneDistance<M>(), 
        Game::HexTopology<M>(mtx));
}

int
Game::Cell::get_dist(void) const
{
    return dist;
}

void 
Game::Cell::set_dist(double val)
{
    dist = val;
}

int
Game::Cell::get_num(void) const
{
    return num;
}

void 
Game::Cell::set_num(int val)
{
    num = val;
}

bool 
Game::Cell::is_closed(void) const
{
    return !set;
}

void
Game::Cell::close(void)
{
    set = false;
}

void
Game::Cell::open(void)
{
    set = true;
}

Game::IntCoord 
Game::Cell::get_from(void) const
{
    return from;
}

void 
Game::Cell::set_from(Game::IntCoord from_)
{
    from = from_;
}

bool 
Game::operator < (const Game::Cell &c1, const Game::Cell &c2)
{
    if (c1.dist != c2.dist) {
        return c1.dist < c2.dist;
    }
    return c1.num < c2.num;
}

bool 
Game::operator == (const Game::Cell &c1, const Game::Cell &c2)
{
    return c1.num == c2.num;
}