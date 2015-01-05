#include "Keeper.h"
#include <iostream>
#include <string>
using namespace std;

Keeper::Keeper(Seed &ranGen, Distributer &distributer, int necesAuth){
	this->ranGen = &ranGen;
	this->necessAuth = necesAuth;
	this->distributer = &distributer;
	input = 0;
	output = 0;
}

void Keeper::setBallot(pair<mpz_class, mpz_class> vote){
	ballot = vote;
	setShare();
}

mpz_class Keeper::getMV(){
	return input;
}

void Keeper::setShare(){
	input = randomNum(mpz_class(ranGen->getQ(10)));
	output = distributer->getShare(input);
	mpz_t temp;
	mpz_init_set(temp, ballot.first.get_mpz_t());
	mpz_powm(temp, temp, output.get_mpz_t(), mpz_class(ranGen->getQ(10)).get_mpz_t());
	output = mpz_class(temp);
}

mpz_class Keeper::getShare(){
	return output;
}

mpz_class Keeper::randomNum(mpz_class upper){
	mpz_class f;
	do{
		long int x = getSeed();
		gmp_randclass r1(gmp_randinit_mt);
		r1.seed(x);
		f = r1.get_z_range(upper);
	} while (mpz_cmp_ui(f.get_mpz_t(), 0) == 0);
	return f;
}

mpz_class Keeper::getSumForExhaustiveSearch(Keeper list[], int size){
	mpz_t x, y;
	mpz_class xV=0;
	try{
		xV = getXValue(list, size);
	}
	catch (const invalid_argument &e){
		cout << e.what() << endl;
	}
	mpz_init_set(x, xV.get_mpz_t());
	mpz_init_set(y, ballot.second.get_mpz_t());
	mpz_invert(x, x, mpz_class(ranGen->getP(10)).get_mpz_t()); // Pye göre çevir, bölüyoruz y'yi burda
	mpz_mul(y, y, x);
	mpz_mod(y, y, mpz_class(ranGen->getP(10)).get_mpz_t());
	return mpz_class(y);
}

mpz_class Keeper::getXValue(Keeper list[], int size){
	mpz_class result = 1;
	if (size < necessAuth)
		throw std::invalid_argument("Less authority than expected");
	for (int i = 0; i < necessAuth; i++){
		mpz_class pow = getLagrangeSum(list, list[i], size);
		mpz_t temp;
		mpz_init_set(temp, list[i].getShare().get_mpz_t());
		mpz_powm(temp, temp, pow.get_mpz_t(), mpz_class(ranGen->getP(10)).get_mpz_t());
		result *= mpz_class(temp);
		result %= mpz_class(ranGen->getP(10));

		cout << i << " vote seysi " << mpz_get_str(NULL, 10, result.get_mpz_t()) << endl << endl;
	}
	return result;
}

mpz_class Keeper::getLagrangeSum(Keeper list[], Keeper keeper, int size){
	mpz_class as = keeper.getShare();
	mpz_class mult = 1;
	for (int i = 0; i < necessAuth; i++){
		if (list[i].getShare() == as)
			continue;
		else{
			mpz_t inverse; mpz_init(inverse);
			mpz_class temp = list[i].getMV();
			temp = temp - keeper.getMV();
			mpz_invert(inverse, temp.get_mpz_t(), mpz_class(ranGen->getQ(10)).get_mpz_t());
			temp = list[i].getMV() *mpz_class(inverse);
			mult *= temp;
		}
	}
	mult %= mpz_class(ranGen->getQ(10));
	return mult;
}