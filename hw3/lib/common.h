#pragma once

#include <core.h>

#include <erl_nif.h>

extern ErlNifResourceType* VectorType;

TVector ListToVectorImpl(ErlNifEnv* env, ERL_NIF_TERM term);
ERL_NIF_TERM VectorToListImpl(ErlNifEnv* env, const TVector& vector);
TVector TermToVector(ErlNifEnv* env, ERL_NIF_TERM term);
ERL_NIF_TERM VectorToTerm(ErlNifEnv* env, TVector vector);
