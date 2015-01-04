#include "Voter.h"
#include "HonestVerifier.h"
#include "Crypt.h"
#include <iostream>
#include <string>
#include <mpir.h>
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
#include <map>
#include <random>
using namespace std;
//XY Ballot'unu gönder

Voter::Voter(){
	ranGen = new Seed();
	c = mpz_class(0);
	publicKey = mpz_class(0);
	f = mpz_class(ranGen->getExternalG());
	r = mpz_class(0);
	w = mpz_class(ranGen->getExternalG());
	generators = NULL;
	czList = NULL;
	randomList = NULL;
	candidateNum = 0;
}

Voter::Voter(mpz_class *gens, Seed &seed, int candNum){
	generators = gens;
	ranGen = &seed;
	c = mpz_class(0);
	publicKey = mpz_class(0);
	f = mpz_class(ranGen->getExternalG());
	r = mpz_class(0);
	w = mpz_class(ranGen->getExternalG());
	candidateNum = candNum;
	czList = new pair<mpz_class, mpz_class>[candNum];
	randomList = new pair<mpz_class, mpz_class>[candNum];
}

void Voter::setSeed(Seed &seed){
	ranGen = &seed;
}

void Voter::setC(mpz_class cValue){
	c = cValue;
	mpz_class num = c;
	for (int i = 0; i < candidateNum; i++){
		num = (num - czList[i].first);
	}
	mpz_class num2 = (w - r*num);
	czList[0].first = num;
	czList[0].second = num2;	
}

void Voter::setCZList(){
	if (!generators)
		throw invalid_argument("Candidates are empty");
	czList[0] = make_pair(mpz_class(0), mpz_class(0));
	for (int i = 1; i < candidateNum; i++){
		mpz_class cV = mpz_class(ranGen->getExternalG());
		mpz_class zV = mpz_class(ranGen->getExternalG());
		czList[i] = make_pair(cV, zV);
	}
}

void Voter::setRandomList(){
	if (!generators)
		throw invalid_argument("Candidates are empty");
	mpz_t g, h, p, q;
	mpz_t num1, num2, tempnum;
	mpz_init(num1); mpz_init(num2); mpz_init(tempnum);
	mpz_init_set_str(g, ranGen->getG(10).c_str(), 10);
	mpz_init_set_str(p, ranGen->getP(10).c_str(), 10);
	mpz_init_set_str(q, ranGen->getQ(10).c_str(), 10);
	mpz_init_set(h, publicKey.get_mpz_t()); // PUBLIC KEY AL BURDA!!!
	for (int i = 0; i < candidateNum; i++){
		if (generators[i] != canGen){
			mpz_powm(tempnum, g, czList[i].second.get_mpz_t(), p);
			mpz_powm(num1, ballot.first.get_mpz_t(), czList[i].first.get_mpz_t(), p);
			mpz_mul(num1, num1, tempnum); mpz_mod(num1, num1, p);

			mpz_powm(tempnum, h, czList[i].second.get_mpz_t(), p);

			mpz_invert(num2, generators[i].get_mpz_t(), p);
			mpz_mul(num2, num2, ballot.second.get_mpz_t()); mpz_mod(num2, num2, p);
			mpz_powm(num2, num2, czList[i].first.get_mpz_t(), p);

			mpz_mul(num2, num2, tempnum); mpz_mod(num2, num2, p);
			randomList[i] = make_pair(mpz_class(num1), mpz_class(num2));
		}
	}
	mpz_powm(num1, g, w.get_mpz_t(), p);
	mpz_powm(num2, h, w.get_mpz_t(), p);
	randomList[0] = make_pair(mpz_class(num1), mpz_class(num2));
}

void Voter::setBallot(Crypt cv, pair<string, mpz_class> vote){
	ballot = cv.voteCast(vote);
	r = mpz_class(cv.r);
}

void Voter::setGenerators(mpz_class *gens, int candNum){
	generators = gens;
	candidateNum = candNum;
	randomList = new pair<mpz_class, mpz_class>[candNum];
	czList = new pair<mpz_class, mpz_class>[candNum];
}

pair<mpz_class, mpz_class> *Voter::getRandomList(){
	return randomList;
}

pair<mpz_class, mpz_class> *Voter::getczList(){
	return czList;
}

pair<mpz_class, mpz_class> Voter::getFinalBallot(){
	return ballot;
}

mpz_class Voter::getF(){
	return f;
}

mpz_class Voter::getRandomizedNum(mpz_class upper, mpz_class down){
	mpz_class f;
	long int x = getSeed();
	gmp_randclass r1(gmp_randinit_mt);
	r1.seed(x);
	f = r1.get_z_range(upper);
	if (mpz_cmp(f.get_mpz_t(), down.get_mpz_t()) < 0)
		f += down;
	return f;
}

void Voter::setFinalBallot(pair<mpz_class, mpz_class> uv){ // değiştir bunu, t'den değil, uv pairinden alacak
	mpz_t x, y, q;
	mpz_init_set(x, ballot.first.get_mpz_t());
	mpz_init_set(y, ballot.second.get_mpz_t());
	mpz_init_set_str(q, ranGen->getQ(10).c_str(), 10);
	mpz_mul(x, uv.first.get_mpz_t(), x); mpz_mod(x, x, q);
	mpz_mul(y, uv.second.get_mpz_t(), y); mpz_mod(y, y, q);
	ballot.first = mpz_class(x);
	ballot.second = mpz_class(y);
}

bool Voter::isValidFromHV(HonestVerifier &hv){
	hv.setRandomList(randomList);
	mpz_class tempC = hv.getC();
	setC(tempC);
	hv.setCZList(czList);
	if (!hv.isValidVote()){
		cout << "NOT VALID!" << endl;
		return false;
	}
	pair<mpz_class, mpz_class> uv = hv.getUV();
	hv.setF(f);
	mpz_class teta = hv.getTetaValue();
	pair<mpz_class, mpz_class> de = hv.getDE();
	mpz_t g, h, p, num1, num2;
	mpz_init_set_str(g, ranGen->getG(10).c_str(), 10);
	mpz_init_set_str(p, ranGen->getP(10).c_str(), 10);
	mpz_init_set(h, publicKey.get_mpz_t());
	mpz_powm(g, g, teta.get_mpz_t(), p);
	mpz_powm(h, h, teta.get_mpz_t(), p);
	mpz_init(num1); mpz_init(num2);
	mpz_powm(num1, uv.first.get_mpz_t(), f.get_mpz_t(), p);
	mpz_powm(num2, uv.second.get_mpz_t(), f.get_mpz_t(), p);
	mpz_mul(num1, de.first.get_mpz_t(), num1); mpz_mod(num1, num1, p);
	mpz_mul(num2, de.second.get_mpz_t(), num2); mpz_mod(num2, num2, p);
	if (mpz_cmp(num1, g) == 0 && mpz_cmp(num2, h) == 0){
		setFinalBallot(uv);
		cout << "VALID!" << endl;
		return true;
	}
	else{
		cout << "NOT VALID!" << endl;
		return false;
	}
}

void Voter::setPublicKey(mpz_class publ){
	publicKey = publ;
	setCZList();
	setRandomList();
}

void Voter::setCandidateGenerator(mpz_class gen){
	canGen = gen;
}