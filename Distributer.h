#ifndef DISTRIBUTER_H
#define DISTRIBUTER_H
#include <iostream>
#include <string>
#include <mpir.h>
#include "Seed.h"
#include "Crypt.h"
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
using namespace std;

class Distributer{
public:
	Distributer();
	Distributer(Seed &ranGen, int size, Crypt &secret);
	mpz_class getShare(mpz_class x);
	int getSize();
private:
	mpz_class *coeffs;
	Seed *ranGen;
	mpz_class secretNum;
	int size;
	void setCoeffs();
	
};

#endif