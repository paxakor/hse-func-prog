#include "core.h"

#include <cassert>

TVector VectorMult(TVector vec, const double scalar) {
    for (double& x : vec) {
        x *= scalar;
    }
    return vec;
}

TVector VectorSum(const TVector& lhs, const TVector& rhs) {
    assert(lhs.size() == rhs.size());
    TVector result = lhs;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] += rhs[i];
    }
    return result;
}

TMatrix MatrixMult(TMatrix mat, const double scalar) {
    for (TVector& x : mat) {
        VectorMult(x, scalar);
    }
    return mat;
}

TMatrix MatrixSum(const TMatrix& lhs, const TMatrix& rhs) {
    assert(lhs.size() == rhs.size());
    const size_t n = lhs.size();
    TMatrix result;
    result.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        result.emplace_back(VectorSum(lhs[i], rhs[i]));
    }
    return result;
}
