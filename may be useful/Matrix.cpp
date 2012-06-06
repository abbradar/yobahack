template <typename T>
class Matrix
{
    int rows;
    int cols;
    T *m;
public:
    static const int ROWS_MAX = 16384;
    static const int COLS_MAX = 16384;
    static const int MEM_MAX = 1073741824;
    Matrix(int rows_, int cols_, T def);
    Matrix(const Matrix &mx);
    void swap(Matrix &mx);
    int get_rows(void) const;
    int get_cols(void) const;
    const T &at(int row, int col) const;
    T &at(int row, int col);
    const T &at(const IntCoord &c) const;
    T &at(const IntCoord &c);
    Matrix &operator = (Matrix mx);
    ~Matrix(void);
    
    typedef T value_type;
};

template <typename T>
Matrix<T>::Matrix(int rows_, int cols_, T def = T()): rows(), cols(), m()
{
    if (rows_ < 1 || rows_ > ROWS_MAX || cols_ < 1 || cols_ > COLS_MAX) {
        throw std::invalid_argument("invalid matrix size");
    }
    if (sizeof(def) > (MEM_MAX / double(rows_)) / double(cols_)) {
        throw std::invalid_argument("error");
    }
    rows = rows_;
    cols = cols_;
    int size = rows * cols;
    m = new T[size];
    for (int i = 0; i < size; ++i) {
        m[i] = def;
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T> &mx): rows(), cols(), m()
{
    rows = mx.rows;
    cols = mx.cols;
    int size = rows * cols;
    m = new T[size];
    for (int i = 0; i < size; ++i) {
        m[i] = mx.m[i];
    }
}

template <typename T>
void
Matrix<T>::swap(Matrix<T> &mx)
{
    int t = rows;
    rows = mx.rows;
    mx.rows = t;
    t = cols;
    cols = mx.cols;
    mx.cols = t;
    T *tmp = m;
    m = mx.m;
    mx.m = tmp;
}

template <typename T>
int
Matrix<T>::get_rows(void) const
{
    return rows;
}

template <typename T>
int
Matrix<T>::get_cols(void) const
{
    return cols;
}

template <typename T>
const T &
Matrix<T>::at(int row, int col) const
{
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        cout << "row: " << row << " col: " << col << endl;
        throw std::range_error("invalid cell");
    }
    return m[row * cols + col];
}

template <typename T>
T &
Matrix<T>::at(int row, int col)
{
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        cout << "row: " << row << " col: " << col << endl;
        throw std::range_error("invalid cell");
    }
    return m[row * cols + col];
}

template <typename T>
const T &
Matrix<T>::at(const IntCoord &c) const
{
    return at(c.get_row(), c.get_col());
}

template <typename T>
T &
Matrix<T>::at(const IntCoord &c)
{
    return at(c.get_row(), c.get_col());
}

template <typename T>
Matrix<T> &
Matrix<T>::operator = (Matrix<T> mx)
{
    swap(mx);
    return *this;
}

template <typename T>
Matrix<T>::~Matrix(void)
{
    delete [] m;
}