#include "core.h"

#include <tuple>
#include <utility>
#include <vector>

#include <erl_nif.h>

#include <cassert>

static ErlNifResourceType* MatrixType = NULL;

namespace {

    TData ListToVectorImpl(ErlNifEnv* env, ERL_NIF_TERM term) {
        assert(enif_is_list(env, term));
        unsigned len = 0;
        assert(enif_get_list_length(env, term, &len));

        TData result(len, 0.0);

        for (double& x : result) {
            ERL_NIF_TERM head;
            assert(enif_get_list_cell(env, term, &head, &term));
            assert(enif_get_double(env, head, &x));
        }
        assert(enif_is_empty_list(env, term));
        return result;
    }

    std::tuple<unsigned, unsigned> GetMatrixTermSize(ErlNifEnv* env, ERL_NIF_TERM term) {
        unsigned rows = 0;
        assert(enif_get_list_length(env, term, &rows));

        ERL_NIF_TERM firstRow;
        assert(enif_get_list_cell(env, term, &firstRow, &term));

        unsigned cols = 0;
        enif_get_list_length(env, firstRow, &cols);

        return std::make_tuple(rows, cols);
    }

    TMatrix ListToMatrixImpl(ErlNifEnv* env, ERL_NIF_TERM term) {
        assert(enif_is_list(env, term));
        unsigned rows, cols;
        std::tie(rows, cols) = GetMatrixTermSize(env, term);

        if (cols != 0) {
            /* [[1.0], [2.0], [3.0]] -> `rows, cols = 3, 1` -> this term is matrix */
            TData result;
            result.reserve(rows * cols);

            for (unsigned i = 0; i < rows; ++i) {
                ERL_NIF_TERM head;
                assert(enif_get_list_cell(env, term, &head, &term));
                TData row = ListToVectorImpl(env, head);
                assert(row.size() == cols);
                result.insert(result.end(), row.begin(), row.end());
            }
            assert(enif_is_empty_list(env, term));
            return TMatrix(rows, cols, result);
        } else {
            /* [1.0, 2.0, 3.0] -> `rows, cols = 3, 0` -> this term is vector */
            return TMatrix(rows, 1, ListToVectorImpl(env, term));
        }
    }

    ERL_NIF_TERM VectorToListImpl(ErlNifEnv* env, const TMatrix& matrix) {
        std::vector<ERL_NIF_TERM> terms;
        for (const double x : matrix) {
            terms.emplace_back(enif_make_double(env, x));
        }
        return enif_make_list_from_array(env, terms.data(), terms.size());
    }

    ERL_NIF_TERM MatrixToListImpl(ErlNifEnv* env, const TMatrix& matrix) {
        std::vector<ERL_NIF_TERM> terms;
        const size_t rows = matrix.GetHeight();
        terms.reserve(rows);
        for (unsigned i = 0; i < rows; ++i) {
            terms.emplace_back(VectorToListImpl(env, GetRowImpl(matrix, i)));
        }
        return enif_make_list_from_array(env, terms.data(), terms.size());
    }
} // namespace

TMatrix TermToMatrix(ErlNifEnv* env, ERL_NIF_TERM term) {
    void* obj;
    assert(enif_get_resource(env, term, MatrixType, &obj));
    return *reinterpret_cast<TMatrix*>(obj);
}

ERL_NIF_TERM MatrixToTerm(ErlNifEnv* env, TMatrix matrix) {
    void* obj = enif_alloc_resource(MatrixType, sizeof(TMatrix));
    new (obj) TMatrix(std::move(matrix));

    ERL_NIF_TERM term = enif_make_resource(env, obj);
    enif_release_resource(obj);
    return term;
}

void _MatrixDtor(ErlNifEnv* /* env */, void* vec) {
    reinterpret_cast<TMatrix*>(vec)->~TMatrix();
}

int Load(ErlNifEnv* env, void** /* priv_data */, ERL_NIF_TERM /* load_info */) {
    if (MatrixType == NULL) {
        MatrixType = enif_open_resource_type(env, "array", "Matrix", _MatrixDtor, ERL_NIF_RT_CREATE, NULL);
    }
    return MatrixType == NULL ? -1 : 0;
}

int Upgrade(ErlNifEnv* env, void** priv_data, void** /* old_priv_data */, ERL_NIF_TERM load_info) {
    return Load(env, priv_data, load_info);
}

ERL_NIF_TERM ListToMatrix(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    return MatrixToTerm(env, ListToMatrixImpl(env, argv[0]));
}

ERL_NIF_TERM MatrixToList(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    return MatrixToListImpl(env, TermToMatrix(env, argv[0]));
}

ERL_NIF_TERM ListToVector(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    return MatrixToTerm(env, ListToMatrixImpl(env, argv[0]));
}

ERL_NIF_TERM VectorToList(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    return VectorToListImpl(env, TermToMatrix(env, argv[0]));
}

ERL_NIF_TERM MatrixMult(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    double scalar = 0;
    if (enif_get_double(env, argv[1], &scalar)) {
        return MatrixToTerm(env, MatrixMultImpl(TermToMatrix(env, argv[0]), scalar));
    } else {
        return MatrixToTerm(env, MatrixMultImpl(TermToMatrix(env, argv[0]), TermToMatrix(env, argv[1])));
    }
}

ERL_NIF_TERM MatrixVectorMult(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    return MatrixToTerm(env, MatrixVectorMultImpl(TermToMatrix(env, argv[0]), TermToMatrix(env, argv[1])));
}

ERL_NIF_TERM MatrixSum(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    const TMatrix lhs = TermToMatrix(env, argv[0]);
    const TMatrix rhs = TermToMatrix(env, argv[1]);
    return MatrixToTerm(env, MatrixSumImpl(lhs, rhs));
}

ERL_NIF_TERM GetRow(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    unsigned n;
    assert(enif_get_uint(env, argv[1], &n));
    return MatrixToTerm(env, GetRowImpl(TermToMatrix(env, argv[0]), n));
}

ERL_NIF_TERM GetColumn(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    unsigned n;
    assert(enif_get_uint(env, argv[1], &n));
    return MatrixToTerm(env, GetColumnImpl(TermToMatrix(env, argv[0]), n));
}

ERL_NIF_TERM GetDiag(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    int n;
    assert(enif_get_int(env, argv[1], &n));
    return MatrixToTerm(env, GetDiagImpl(TermToMatrix(env, argv[0]), n));
}


ERL_NIF_TERM TransposeRowToColumn(ErlNifEnv* env, int /* argc */, const ERL_NIF_TERM argv[]) {
    return MatrixToTerm(env, TransposeRowToColumnImpl(TermToMatrix(env, argv[0])));
}

static ErlNifFunc Functions[] = {
    {"listToMatrix", 1, ListToMatrix},
    {"listToVector", 1, ListToVector},

    {"matrixToList", 1, MatrixToList},
    {"vectorToList", 1, VectorToList},

    {"matrixMult", 2, MatrixMult},
    {"matrixVectorMult", 2, MatrixVectorMult},
    {"matrixSum", 2, MatrixSum},

    {"getRow", 2, GetRow},
    {"getColumn", 2, GetColumn},
    {"getDiag", 2, GetDiag},

    {"transposeRowToColumn", 1, TransposeRowToColumn},
};

ERL_NIF_INIT(Elixir.Matrix, Functions, Load, NULL, Upgrade, NULL);
