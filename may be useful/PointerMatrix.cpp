#include <stdexcept>

template <typename T>
class ObjPtr
{
    T *obj;
public:
    ObjPtr(T *obj_);
    ObjPtr(ObjPtr &p);
    ObjPtr &operator = (const ObjPtr &p);
    T *& operator -> (void);
    const T * operator -> (void) const;
    ~ObjPtr(void);
};

template <typename T>
class PointerMatrix
{
    int rows;
    int cols;
    ObjPtr<T> *m;
    PointerMatrix operator = (PointerMatrix mx);
    PointerMatrix(const PointerMatrix &mx);
public:
    static const int ROWS_MAX = 16384;
    static const int COLS_MAX = 16384;
    PointerMatrix(int rows_, int cols_, T *def);
    int get_rows(void) const;
    int get_cols(void) const;
    const ObjPtr<T> &at(int row, int col) const;
    ObjPtr<T> &at(int row, int col);
    ~PointerMatrix(void);
};

template <typename T>
ObjPtr<T>::ObjPtr(T *obj_ = NULL): obj(obj_) {}

template <typename T>
ObjPtr<T> &
ObjPtr<T>::operator = (const ObjPtr &p)
{
    if (&p != this) {
        delete obj;
        obj = p.obj->clone();
    }
    return *this;
}

template <typename T>
T *&
ObjPtr<T>::operator -> (void)
{
    return obj;
}

template <typename T>
const T *
ObjPtr<T>::operator -> (void) const
{
    return obj;
}

template <typename T>
ObjPtr<T>::~ObjPtr(void)
{
    delete obj;
}

template <typename T>
PointerMatrix<T>::PointerMatrix(int rows_, int cols_, T *def): rows(), cols(), m()
{
    if (rows_ < 1 || rows_ > ROWS_MAX || cols_ < 1 || cols_ > COLS_MAX) {
        throw std::invalid_argument("invalid PointerMatrix size");
    }
    rows = rows_;
    cols = cols_;
    int size = rows * cols;
    m = new ObjPtr<T> [size];
    for (int i = 0; i < size; ++i) {
        m[i] = def->clone();
    }
}

template <typename T>
int
PointerMatrix<T>::get_rows(void) const
{
    return rows;
}

template <typename T>
int
PointerMatrix<T>::get_cols(void) const
{
    return cols;
}

template <typename T>
const ObjPtr<T> &
PointerMatrix<T>::at(int row, int col) const
{
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        throw std::range_error("error");
    }
    return m[row * cols + col];
}

template <typename T>
ObjPtr<T> &
PointerMatrix<T>::at(int row, int col)
{
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        throw std::range_error("error");
    }
    return m[row * cols + col];
}

template <typename T>
PointerMatrix<T>::~PointerMatrix(void)
{
    delete [] m;
}