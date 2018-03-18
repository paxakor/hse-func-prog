#include "common.h"
#include "core.h"

#include <erl_nif.h>

#include <vector>
#include <utility>

#include <cassert>
#include <cstdio>

ErlNifResourceType* VectorType = NULL;

TVector ListToVectorImpl(ErlNifEnv* env, ERL_NIF_TERM term) {
    assert(enif_is_list(env, term));
    unsigned len = 0;
    assert(enif_get_list_length(env, term, &len));

    TVector result(len, 0.0);

    for (double& x : result) {
        ERL_NIF_TERM head;
        assert(enif_get_list_cell(env, term, &head, &term));
        assert(enif_get_double(env, head, &x));
    }
    assert(enif_is_empty_list(env, term));
    return result;
}

ERL_NIF_TERM VectorToListImpl(ErlNifEnv* env, const TVector& vector) {
    std::vector<ERL_NIF_TERM> terms;
    size_t len = vector.size();
    terms.reserve(len);
    for (const double x : vector) {
        terms.emplace_back(enif_make_double(env, x));
    }
    return enif_make_list_from_array(env, terms.data(), len);
}

TVector TermToVector(ErlNifEnv* env, ERL_NIF_TERM term) {
    void* obj;
    assert(enif_get_resource(env, term, VectorType, &obj));
    return *reinterpret_cast<TVector*>(obj);
}

ERL_NIF_TERM VectorToTerm(ErlNifEnv* env, TVector vector) {
    void* obj = enif_alloc_resource(VectorType, sizeof(TVector));
    new (obj) TVector(std::move(vector));

    ERL_NIF_TERM term = enif_make_resource(env, obj);
    enif_release_resource(obj);
    return term;
}
