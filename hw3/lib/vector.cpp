#include "common.h"
#include "core.h"

#include <erl_nif.h>

#include <vector>
#include <utility>

#include <cassert>
#include <cstdio>

void _VectorDtor(ErlNifEnv* env, void* vec) {
    reinterpret_cast<TVector*>(vec)->~TVector();
}

int Load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info) {
    if (VectorType == NULL) {
        VectorType = enif_open_resource_type(env, "array", "Vector", _VectorDtor, ERL_NIF_RT_CREATE, NULL);
    }
    return VectorType == NULL ? -1 : 0;
}

int Upgrade(ErlNifEnv* env, void** priv_data, void** old_priv_data, ERL_NIF_TERM load_info) {
    return Load(env, priv_data, load_info);
}

ERL_NIF_TERM ListToVector(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return VectorToTerm(env, ListToVectorImpl(env, argv[0]));
}

ERL_NIF_TERM VectorToList(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return VectorToListImpl(env, TermToVector(env, argv[0]));
}

ERL_NIF_TERM VectorMult(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    TVector vector = TermToVector(env, argv[0]);
    double scalar = 0;
    assert(enif_get_double(env, argv[1], &scalar));
    return VectorToTerm(env, VectorMult(std::move(vector), scalar));
}

ERL_NIF_TERM VectorSum(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    const TVector lhs = TermToVector(env, argv[0]);
    const TVector rhs = TermToVector(env, argv[1]);
    return VectorToTerm(env, VectorSum(lhs, rhs));
}

static ErlNifFunc Functions[] = {
    {"listToVector", 1, ListToVector},
    {"vectorToList", 1, VectorToList},
    {"vectorMult", 2, VectorMult},
    {"vectorSum", 2, VectorSum},
};

ERL_NIF_INIT(Elixir.Vector, Functions, Load, NULL, Upgrade, NULL);
