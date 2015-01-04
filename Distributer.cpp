#include "Distributer.h"
#include <iostream>
#include <string>
using namespace std;

Distributer::Distributer(){
	ranGen = new Seed();
	size = 3;
	secretNum = ranGen->getExternalG();
	coeffs = new mpz_class[this->size];
	setCoeffs();
}

Distributer::Distributer(Seed &ranGen, int size, Crypt &secret){
	this->ranGen = &ranGen;
	this->size = size;
	secretNum = mpz_class(secret.privateKey);
	coeffs = new mpz_class[this->size];
	setCoeffs();
}

void Distributer::setCoeffs(){
	for (int i = 0; i < size; i++){
		coeffs[i] = ranGen->getExternalG();
	}
}

int Distributer::getSize(){
	return size;
}

mpz_class Distributer::getShare(mpz_class x){
	mpz_class q(ranGen->getQ(10));
	mpz_class sum = secretNum;
	for (int i = 0; i < size; i++){
		mpz_t temp, pow;
		mpz_init_set(temp, x.get_mpz_t());
		mpz_init_set_ui(pow, i);
		mpz_powm(temp, temp, pow, q.get_mpz_t());
		sum += mpz_class(temp);
		sum %= q;
	}
	sum -= 1;
	return sum;
}