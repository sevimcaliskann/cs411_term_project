#ifndef HONESTVERIFIER_H
#define HONESTVERIFIER_H
#include <iostream>
#include <string>
#include <mpir.h>
#include "Seed.h"
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
#include <map>
using namespace std;

class Voter;
//Get ui, vi, d, e functionlarını yaz!!! HV'den çağırcam voter'da yeni oyu oluştururken

class HonestVerifier{
public:
	HonestVerifier();
	////Functions that must be called after the construction
	void setGHQP(mpz_class g, mpz_class h, mpz_class q, mpz_class p);
	void setBallot(Voter voter);
	void setGenerators(mpz_class *generators, int candNum);
	void setCandidateGenerator(mpz_class candidateGen);
	//////////////////////////////////////
	mpz_class getC();
	void setRandomList(pair<mpz_class, mpz_class> *list);
	void setCZList(pair<mpz_class, mpz_class> *list);
	bool isValidVote();
	void setF(mpz_class f); // ilk önce f'i set et, sonra gettetaValue çağır
	pair<mpz_class, mpz_class>getUV();
	pair<mpz_class, mpz_class>getDE();
	mpz_class getTetaValue();
private:
	mpz_class c, g, h, p, q, t, w2, teta, f, candGen;
	mpz_class *generators;
	pair<mpz_class, mpz_class> *randomList;
	pair<mpz_class, mpz_class> *czList;
	pair<mpz_class, mpz_class> ballot;
	int candidateNum;
	mpz_class getRandomizedNum(mpz_class upperLimit, mpz_class downLimit);
};

#endif