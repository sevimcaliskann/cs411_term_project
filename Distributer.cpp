#include "Distributer.h"
#include <iostream>
#include <string>
using namespace std;

Distributer::Distributer(){
	ranGen = new Seed();
	size = 2;
	secretNum = ranGen->getExternalG();
	coeffs = new mpz_class[this->size];
	setCoeffs();
}

Distributer::Distributer(Seed &ranGen, int size, Crypt &secret, mpz_class x){
	this->ranGen = &ranGen;
	this->size = size;

	mpz_t temp; mpz_init(temp);
	mpz_powm(temp, x.get_mpz_t(), secret.privateKey, mpz_class(this->ranGen->getP(10)).get_mpz_t());
	secretNum = mpz_class(temp);

	/*secretNum = mpz_class(secret.privateKey);*/
	coeffs = new mpz_class[this->size];
	setCoeffs();
}

void Distributer::setCoeffs(){
	for (int i = 0; i < size; i++){
		/*mpz_class f;
		do{
			long int x = getSeed();
			gmp_randclass r1(gmp_randinit_mt);
			r1.seed(x);
			f = r1.get_z_range(mpz_class(ranGen->getQ(10)));
		} while (mpz_cmp_ui(f.get_mpz_t(), 0) == 0);*/
		coeffs[i] = ranGen->getExternalG();
		/*coeffs[i] = f;*/
	}
}

int Distributer::getSize(){
	return size;
}

mpz_class Distributer::getShare(mpz_class x){
	mpz_class p(ranGen->getP(10));
	mpz_class sum = secretNum;
	for (int i = 0; i < size; i++){
		mpz_t temp, pow;
		mpz_init_set(temp, x.get_mpz_t());
		mpz_init_set_ui(pow, i+1);
		mpz_powm(temp, temp, pow, p.get_mpz_t()); // powering kısmı oldu
		mpz_mul(temp, temp, coeffs[i].get_mpz_t()); // coeff ile çarpıyok
		sum += mpz_class(temp);
	}
	sum %= p;

	return sum;
}