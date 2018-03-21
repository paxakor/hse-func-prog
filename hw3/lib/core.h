#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <cassert>

using TData = std::vector<double>;
using TVector = std::vector<double>;

/* --- Matrix --- */
class TMatrix {
public: // TODO (@paxakor)
    size_t Height;
    size_t Width;

    size_t Count = 0;
    size_t Offset = 0;
    size_t Step = 1;
    std::shared_ptr<TData> Data;

public:
    TMatrix(size_t n, size_t m, TData data)
        : Height(n)
        , Width(m)
        , Count(Height * Width)
        , Data(std::make_shared<TData>(std::move(data))) {
    }

    TMatrix(size_t n, size_t m, double val = double())
        : TMatrix(n, m, TData(Width * Height, val)) {
    }

    TMatrix(const TMatrix&) = default;
    TMatrix(TMatrix&&) = default;
    TMatrix& operator=(const TMatrix&) = default;
    TMatrix& operator=(TMatrix&&) = default;

    double& At(size_t i) {
        Detach();
        return Data->at(i);
    }

    double At(size_t i) const {
        return Data->at(i);
    }

    // double& At(size_t i, size_t j) {
    //     return Data->at(GetWidth() * i + j);
    // }

    // double At(size_t i, size_t j) const {
    //     return Data->at(GetWidth() * i + j);
    // }

    size_t GetHeight() const {
        return Height;
    }

    size_t GetWidth() const {
        return Width;
    }

    TMatrix& Detach() {
        if (Data.use_count() > 1) {
            Data = std::make_shared<TData>(*Data);
        }
        return *this;
    }

    TMatrix Clone() const {
        TMatrix clone = *this;
        clone.Detach();
        return clone;
    }

    /* --- Base of bidirectional iterator --- */
    template <typename TDerived>
    class TIteratorBase {
    protected:
        size_t Index = 0;

    public:
        TIteratorBase(size_t index)
            : Index(index) {
        }

        TIteratorBase& operator++() {
            ++Index;
            return *this;
        }

        TIteratorBase operator++(int) {
            TIteratorBase copy = *this;
            ++Index;
            return copy;
        }

        TIteratorBase& operator--() {
            --Index;
            return *this;
        }

        TIteratorBase operator--(int) {
            TIteratorBase copy = *this;
            --Index;
            return copy;
        }

        TDerived* This() {
            return reinterpret_cast<TDerived*>(this);
        }

        const TDerived* This() const {
            return reinterpret_cast<const TDerived*>(this);
        }

        bool operator==(const TIteratorBase& other) {
            return This()->Matrix == other.This()->Matrix && Index == other.Index;
        }

        bool operator!=(const TIteratorBase& other) {
            return !(*this == other);
        }
    };

    /* --- Bidirectional iterator --- */
    class TIterator : public TIteratorBase<TIterator> {
        using TBase = TIteratorBase<TIterator>;
        friend TBase;

    private:
        TMatrix* Matrix;

    public:
        TIterator(TMatrix& matrix, size_t index = 0)
            : TBase(index)
            , Matrix(&matrix) {
        }

        double& operator*() const {
            return Matrix->At(Matrix->Offset + Matrix->Step * Index);
        }
    };

    /* --- Constant bidirectional iterator --- */
    class TConstIterator : public TIteratorBase<TConstIterator> {
        using TBase = TIteratorBase<TConstIterator>;
        friend TBase;

    private:
        const TMatrix* Matrix;

    public:
        TConstIterator(const TMatrix& matrix, size_t index = 0)
            : TBase(index)
            , Matrix(&matrix) {
        }

        double operator*() const {
            return Matrix->At(Matrix->Offset + Matrix->Step * Index);
        }
    };

    TIterator begin() {
        Detach();
        return TIterator(*this);
    }

    TIterator end() {
        Detach();
        return TIterator(*this, Count);
    }

    TConstIterator begin() const {
        return TConstIterator(*this);
    }

    TConstIterator end() const {
        return TConstIterator(*this, Count);
    }

    TMatrix Slice(size_t offset, size_t step, size_t count) const {
        TMatrix slice = *this;
        slice.Offset = offset;
        slice.Step = step;
        slice.Count = count;
        return slice;
    }
};

bool CompareShapes(const TMatrix& lhs, const TMatrix& rhs);
TMatrix MatrixMultImpl(TMatrix mat, const double scalar);
TMatrix MatrixMultImpl(const TMatrix& lhs, const TMatrix& rhs);
TMatrix MatrixVectorMultImpl(const TMatrix& lhs, const TMatrix& rhs);
TMatrix MatrixSumImpl(const TMatrix& lhs, const TMatrix& rhs);

TMatrix GetRowImpl(TMatrix matrix, size_t i);
TMatrix GetColumnImpl(TMatrix matrix, size_t i);
TMatrix GetDiagImpl(TMatrix matrix, ssize_t i);

TMatrix TransposeRowToColumnImpl(TMatrix row);
