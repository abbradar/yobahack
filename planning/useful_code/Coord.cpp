#include <iostream>

namespace Game
{

    template <typename T>
    class Coord
    {
        T row, col;
public:
        Coord(): row(), col() {}
        Coord(T row_, T col_): row(row_), col(col_) {}
        Coord(const Coord &c): row(c.row), col(c.col) {}
        Coord &operator = (const Coord &c);
        bool operator == (const Coord &c);
        bool operator != (const Coord &c);
        bool operator < (const Coord &c);
        bool operator <= (const Coord &c);
        bool operator >= (const Coord &c);
        bool operator > (const Coord &c);
        int get_row(void) const;
        int get_col(void) const;

        template <typename R>
        friend std::ostream &operator << (std::ostream &f, const Coord<R> &c);
        
        typedef T value_type;
    };

    typedef Coord<int> IntCoord;
}

template <typename R>
std::ostream &
operator << (std::ostream &f, const Game::Coord<R> &c)
{
    f << '(' << c.row << ',' << c.col << ')';
    return f;
}

template <typename T>
Game::Coord<T> &
Game::Coord<T>::operator = (const Game::Coord<T> &c)
{
    if (this != &c) {
        row = c.row;
        col = c.col;
    }
    return *this;
}

template <typename T>
bool
Game::Coord<T>::operator == (const Game::Coord<T> &c)
{
    return (row == c.row) && (col == c.col);
}

template <typename T>
bool
Game::Coord<T>::operator != (const Game::Coord<T> &c)
{
    return (row != c.row) || (col != c.col);
}

template <typename T>
bool
Game::Coord<T>::operator < (const Game::Coord<T> &c)
{
    if (row != c.row) {
        return row < c.row;
    }
    return col < c.col;
}

template <typename T>
bool
Game::Coord<T>::operator <= (const Game::Coord<T> &c)
{
    if (row != c.row) {
        return row < c.row;
    }
    return col <= c.col;
}

template <typename T>
bool
Game::Coord<T>::operator > (const Game::Coord<T> &c)
{
    if (row != c.row) {
        return row > c.row;
    }
    return col > c.col;
}

template <typename T>
bool
Game::Coord<T>::operator >= (const Game::Coord<T> &c)
{
    if (row != c.row) {
        return row > c.row;
    }
    return col >= c.col;
}

template <typename T>
int
Game::Coord<T>::get_row(void) const
{
    return row;
}

template <typename T>
int
Game::Coord<T>::get_col(void) const
{
    return col;
}
