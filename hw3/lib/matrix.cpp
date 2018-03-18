#include "common.h"
#include "core.h"

#include <erl_nif.h>

#include <vector>
#include <utility>

#include <cassert>
#include <cstdio>

ErlNifResourceType* MatrixType = NULL;

namespace {
    TMatrix ListToMatrixImpl(ErlNifEnv* env, ERL_NIF_TERM term) {
        assert(enif_is_list(env, term));
        unsigned len = 0;
        assert(enif_get_list_length(env, term, &len));

        TMatrix result;
        result.reserve(len);

        for (unsigned i = 0; i < len; ++i) {
            ERL_NIF_TERM head;
            assert(enif_get_list_cell(env, term, &head, &term));
            result.emplace_back(ListToVectorImpl(env, head));
            if (i > 0) {
                assert(result.back().size() == result.front().size());
            }
        }
        assert(enif_is_empty_list(env, term));
        return result;
    }

    ERL_NIF_TERM MatrixToListImpl(ErlNifEnv* env, const TMatrix& matrix) {
        std::vector<ERL_NIF_TERM> terms;
        size_t len = matrix.size();
        terms.reserve(len);
        for (const TVector& x : matrix) {
            terms.emplace_back(VectorToListImpl(env, x));
        }
        return enif_make_list_from_array(env, terms.data(), len);
    }

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
} // namespace

void _MatrixDtor(ErlNifEnv* env, void* vec) {
    reinterpret_cast<TMatrix*>(vec)->~TMatrix();
}

int Load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info) {
    if (MatrixType == NULL) {
        MatrixType = enif_open_resource_type(env, "array", "Matrix", _MatrixDtor, ERL_NIF_RT_CREATE, NULL);
    }
    return MatrixType == NULL ? -1 : 0;
}

int Upgrade(ErlNifEnv* env, void** priv_data, void** old_priv_data, ERL_NIF_TERM load_info) {
    return Load(env, priv_data, load_info);
}

ERL_NIF_TERM ListToMatrix(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return MatrixToTerm(env, ListToMatrixImpl(env, argv[0]));
}

ERL_NIF_TERM MatrixToList(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return MatrixToListImpl(env, TermToMatrix(env, argv[0]));
}

ERL_NIF_TERM MatrixMult(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    TMatrix matrix = TermToMatrix(env, argv[0]);
    double scalar = 0;
    assert(enif_get_double(env, argv[1], &scalar));
    return MatrixToTerm(env, MatrixMult(std::move(matrix), scalar));
}

ERL_NIF_TERM MatrixSum(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    const TMatrix lhs = TermToMatrix(env, argv[0]);
    const TMatrix rhs = TermToMatrix(env, argv[1]);
    return MatrixToTerm(env, MatrixSum(lhs, rhs));
}

static ErlNifFunc Functions[] = {
    {"listToMatrix", 1, ListToMatrix},
    {"matrixToList", 1, MatrixToList},
    {"matrixMult", 2, MatrixMult},
    {"matrixSum", 2, MatrixSum},
};

ERL_NIF_INIT(Elixir.Matrix, Functions, Load, NULL, Upgrade, NULL);
