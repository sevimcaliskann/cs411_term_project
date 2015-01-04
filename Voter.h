#ifndef VOTER_H
#define VOTER_H
#include <iostream>
#include <string>
#include <mpir.h>
#include "Seed.h"
#include "Crypt.h"
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
#include <map>
using namespace std;

class HonestVerifier;
class Voter{
public:
	Voter();
	Voter(mpz_class *generators, Seed &ranGen, int candidateNum);
	//Functions that must be called after the construction
	void setGenerators(mpz_class *generators, int candidateNum);
	void setPublicKey(mpz_class publ);
	void setBallot(Crypt c, pair<string, mpz_class> vote);
	void setCandidateGenerator(mpz_class gen);
	/////////////////////////
	void setSeed(Seed &ranGen);
	pair<mpz_class, mpz_class> *getRandomList();
	pair<mpz_class, mpz_class> *getczList();
	bool isValidFromHV(HonestVerifier &hv); // From hv functions get u,v and d,e pairs
	pair<mpz_class, mpz_class> getFinalBallot();
	mpz_class getF();
	void setC(mpz_class c);
	
private:
	int candidateNum;
	Seed *ranGen;
	mpz_class c, f, r, w, publicKey, canGen; // f will be randomized at contructor
	mpz_class *generators;
	pair<mpz_class, mpz_class> *randomList;
	pair<mpz_class, mpz_class> *czList;
	pair<mpz_class, mpz_class> ballot;
	mpz_class getRandomizedNum(mpz_class upperLimit, mpz_class downLimit);
	void setFinalBallot(pair<mpz_class, mpz_class> uv);
	void setCZList();
	void setRandomList();
};

#endif