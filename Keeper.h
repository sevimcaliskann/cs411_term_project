#ifndef KEEPER_H
#define KEEPER_H
#include <iostream>
#include <string>
#include <mpir.h>
#include "Distributer.h"
#include "Seed.h"
#include <map>
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
using namespace std;

extern long int getSeed();

class Keeper{
public:
	Keeper(Seed &ranGen, Distributer &distributer, int necesAut);
	void setBallot(pair<mpz_class, mpz_class> vote);
	mpz_class getShare();
	mpz_class getMV();
	mpz_class getSumForExhaustiveSearch(Keeper list[], int size);
	
private:
	void setShare();
	mpz_class randomNum(mpz_class upperL);
	mpz_class getXValue(Keeper list[], int size);
	mpz_class getLagrangeSum(Keeper list[], Keeper keeper, int size);

	Seed *ranGen;
	mpz_class input, output;
	int necessAuth;
	pair<mpz_class, mpz_class> ballot;
	Distributer *distributer;
};


#endif