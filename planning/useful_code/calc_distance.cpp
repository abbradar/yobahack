Matrix<int> 
calc_distance(const Matrix<bool> &field, int row, int col)
{
    if (row < 0 || row >= field.get_rows() 
        || col < 0 || col >= field.get_cols()) {
        throw std::range_error("Invalid indicies");
    }
    HexTopology<Matrix<bool> > hex_f(field);
    std::pair<int,int> cell(row, col);
    std::queue<std::pair<int,int> > q;
    q.push(cell);
    std::vector<std::pair<int,int> > tmp_cells;
    Matrix<int> mtx(field.get_rows(), field.get_cols(), std::numeric_limits<int>::max());
    mtx.at(row, col) = 0;
    while (!q.empty()) {
        cell = q.front();
        q.pop();
        if (field.at(cell.first, cell.second)) {
            mtx.at(cell.first, cell.second) = std::numeric_limits<int>::max();
            continue;
        }
        tmp_cells = hex_f(cell.first, cell.second);
        std::vector<std::pair<int,int> >::const_iterator ii;
        for (ii = tmp_cells.begin(); ii != tmp_cells.end(); ++ii) {
            if (mtx.at(ii->first, ii->second) != std::numeric_limits<int>::max() ||
                field.at(ii->first, ii->second)) {
                continue;
            }
            q.push(*ii);
            mtx.at(ii->first, ii->second) = mtx.at(cell.first, cell.second) + 1;
        }
    }
    return mtx;
}
