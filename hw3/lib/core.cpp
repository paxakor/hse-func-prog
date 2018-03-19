#include "core.h"

#include <utility>

#include <cassert>

bool CompareShapes(const TMatrix& lhs, const TMatrix& rhs) {
    return lhs.GetHeight() == rhs.GetHeight() && lhs.GetWidth() == rhs.GetWidth();
}

bool AreDotable(const TMatrix& lhs, const TMatrix& rhs) {
    return lhs.GetWidth() == rhs.GetHeight();
}

TMatrix MatrixMultImpl(TMatrix mat, const double scalar) {
    for (double& x : mat) {
        x *= scalar;
    }
    return mat;
}

TMatrix MatrixMultImpl(const TMatrix& lhs, const TMatrix& rhs) {
    assert(AreDotable(lhs, rhs));
    TData data;
    data.reserve(lhs.GetHeight() * rhs.GetWidth());
    for (size_t i = 0; i < lhs.GetHeight(); ++i) {
        for (size_t k = 0; k < rhs.GetWidth(); ++k) {
            const TMatrix rCol = GetColumn(rhs, k);
            TMatrix::TConstIterator b = rCol.begin();
            double val = 0;
            for (const double a : GetRow(lhs, i)) {
                val += *b * a;
                ++b;
            }
            assert(b == rCol.end());
            data.push_back(val);
            fflush(stdout);
        }
    }
    return TMatrix(lhs.GetHeight(), rhs.GetWidth(), std::move(data));
}

TMatrix MatrixSumImpl(const TMatrix& lhs, const TMatrix& rhs) {
    assert(CompareShapes(lhs, rhs));
    TMatrix result = lhs;
    TMatrix::TConstIterator rhsIter = rhs.begin();
    for (double& x : result) {
        x += *rhsIter;
        ++rhsIter;
    }
    return result;
}

TMatrix GetRow(TMatrix matrix, size_t i) {
    matrix.Offset = i * matrix.GetWidth();
    matrix.Step = 1;
    matrix.Count = matrix.GetWidth();
    matrix.Height = 1;
    return matrix;
}

TMatrix GetColumn(TMatrix matrix, size_t i) {
    matrix.Offset = i;
    matrix.Step = matrix.GetWidth();
    matrix.Count = matrix.GetHeight();
    matrix.Width = 1;
    return matrix;
}

TMatrix GetDiag(TMatrix matrix, ssize_t i) {
    assert(i < 0 ? -i < static_cast<ssize_t>(matrix.GetHeight()) : i < static_cast<ssize_t>(matrix.GetWidth()));
    matrix.Offset = i < 0 ? i * matrix.GetWidth() : i;
    matrix.Step = matrix.GetWidth() + 1;
    matrix.Count = (matrix.Data->size() - matrix.Offset) / matrix.Step + 1;
    matrix.Height = matrix.Count;
    matrix.Width = 1;
    return matrix;
}
