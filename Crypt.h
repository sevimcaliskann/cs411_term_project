#ifndef CRYPT_H
#define CRYPT_H
#include <iostream>
#include <string>
#include <mpir.h>
#include "Seed.h"
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
#include <map>
using namespace std;

extern long int getSeed();

class Crypt{
	friend class Voter;
	friend class Distributer;
public:
	Crypt();
	Crypt(Seed &ranGen);
	mpz_class getPublicKey();
	bool isExist(mpz_class *gList, int size, string element);
	string printMpzClass(const mpz_class &t);
	mpz_class *getGenerators(int size);
	void printGenerators(mpz_class *gList, int size);
	void matchGeneratos(mpz_class *gList, map<string, mpz_class> &candidates);
	void setGenerators(int size);
	map<mpz_class, mpz_class> getVoteSeries(map<string, mpz_class> candidates, string * votes, int size);
	pair<mpz_class, mpz_class> voteCast(const pair<string, mpz_class> &candidate);
	pair<mpz_class, mpz_class> voteTally(map<mpz_class, mpz_class> votes);
	mpz_class getVoteSUm(const pair<mpz_class, mpz_class> &tall);
	map<string, mpz_class> returnVoteResults(map<string, mpz_class> candidates, mpz_class sum, int voteSize);
	mpz_t privateKey;
protected:
	void initRandomR(string q, mpz_t &num);
private:
	mpz_t /*privateKey,*/ publicKey, r;
	Seed *ranGen;
};

#endif