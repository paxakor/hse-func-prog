#pragma once

#include <vector>

/* --- Vector --- */
class TVector : public std::vector<double> {
    using TBase = std::vector<double>;
    using TBase::TBase;
};

TVector VectorMult(TVector vec, const double scalar);
TVector VectorSum(const TVector& lhs, const TVector& rhs);

/* --- Matrix --- */
class TMatrix : public std::vector<TVector> {
    using TBase = std::vector<TVector>;
    using TBase::TBase;
};

TMatrix MatrixMult(TMatrix mat, const double scalar);
TMatrix MatrixSum(const TMatrix& lhs, const TMatrix& rhs);
