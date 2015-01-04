#include "HonestVerifier.h"
#include "Voter.h"
#include <iostream>
#include <string>
#include <mpir.h>
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
#include <map>
#include <random>
using namespace std;

HonestVerifier::HonestVerifier(){
	generators = NULL;
	randomList = NULL;
	czList = NULL;
	candidateNum = 0;
	//c, g, h, p,q, t, w2, teta, f, candGen;
	c = 0; // getRandomizedNum(q, mpz_class(0));
	g = 0; q = 0; h = 0; p = 0;
	t = 0; // getRandomizedNum(q, mpz_class(0));
	w2 = 0;
	teta = 0;
	candGen = 0;
	f = 0;
}

mpz_class HonestVerifier::getRandomizedNum(mpz_class upper, mpz_class downlimit){
	mpz_class f;
	long int x = getSeed();
	gmp_randclass r1(gmp_randinit_mt);
	r1.seed(x);
	f = r1.get_z_range(upper);
	if (mpz_cmp(f.get_mpz_t(), downlimit.get_mpz_t()) < 0)
		f += downlimit;
	return f;
}

mpz_class HonestVerifier::getC(){
	return c;
}

void HonestVerifier::setGHQP(mpz_class gV, mpz_class hV, mpz_class qV, mpz_class pV){
	g = gV;
	p = pV;
	h = hV;
	q = qV;
	c = getRandomizedNum(q, mpz_class(0));
	t = getRandomizedNum(q, mpz_class(0));
	w2 = getRandomizedNum(q, mpz_class(0));
}

void HonestVerifier::setRandomList(pair<mpz_class, mpz_class> *list){
	randomList = list;
}

void HonestVerifier::setCZList(pair<mpz_class, mpz_class> *list){
	czList = list;
}

bool HonestVerifier::isValidVote(){
	mpz_t pow, sec;
	mpz_init(pow); mpz_init(sec);
	mpz_class num=0, check = 0;
	for (int i = 0; i < candidateNum; i++) check = (check + czList[i].first)%q;
	if (check != c) return false;
	for (int i = 0; i < candidateNum; i++){
		num = randomList[i].first;
		mpz_powm(pow, g.get_mpz_t(), czList[i].second.get_mpz_t(), p.get_mpz_t());
		mpz_powm(sec, ballot.first.get_mpz_t(), czList[i].first.get_mpz_t(), p.get_mpz_t());
		mpz_mul(pow, pow, sec); mpz_mod(pow, pow, p.get_mpz_t());
		mpz_class temp(pow);
		if (temp!=num) return false;
		num = randomList[i].second;
		mpz_powm(pow, h.get_mpz_t(), czList[i].second.get_mpz_t(), p.get_mpz_t());
		if (i == 0){
			int j;
			for (j = 0; j < candidateNum; j++){
				mpz_t yeter; mpz_init(yeter);
				mpz_invert(sec, generators[j].get_mpz_t(), p.get_mpz_t());/// BURAYA DİKKAT ET, tersi patlayabilir
				mpz_mul(sec, ballot.second.get_mpz_t(), sec); mpz_mod(sec, sec, p.get_mpz_t());
				//cout << j << "x " << mpz_get_str(NULL, 10, num.get_mpz_t()) << endl << endl;
				mpz_powm(sec, sec, czList[i].first.get_mpz_t(), p.get_mpz_t());
				mpz_mul(yeter, pow, sec); mpz_mod(yeter, yeter, p.get_mpz_t());
				temp = mpz_class(yeter);
				if (num == temp) break;
			}
			if (j == candidateNum) return false;
		}
		else if (generators[i - 1] != candGen){
			mpz_mul(sec, ballot.second.get_mpz_t(), sec); mpz_mod(sec, sec, p.get_mpz_t());
			mpz_powm(sec, sec, czList[i].first.get_mpz_t(), p.get_mpz_t());
			mpz_mul(pow, pow, sec); mpz_mod(pow, pow, p.get_mpz_t());
			temp = mpz_class(num);
			if (num != temp) return false;
		}
	}
	return true;
}

void HonestVerifier::setCandidateGenerator(mpz_class candidateGen){
	candGen = candidateGen;
}

void HonestVerifier::setF(mpz_class fV){
	f = fV;
}

pair<mpz_class, mpz_class> HonestVerifier::getUV(){
	mpz_t pV, hV, gV;
	mpz_init_set(gV, g.get_mpz_t());
	mpz_init_set(pV, p.get_mpz_t());
	mpz_init_set(hV, h.get_mpz_t());
	mpz_powm(gV, gV, t.get_mpz_t(), pV);
	mpz_powm(hV, hV, t.get_mpz_t(), pV);
	return pair<mpz_class, mpz_class>(mpz_class(gV), mpz_class(hV));
}

pair<mpz_class, mpz_class> HonestVerifier::getDE(){
	mpz_t pV, hV, gV;
	mpz_init_set(gV, g.get_mpz_t());
	mpz_init_set(pV, p.get_mpz_t());
	mpz_init_set(hV, h.get_mpz_t());
	mpz_powm(gV, gV, w2.get_mpz_t(), pV);
	mpz_powm(hV, hV, w2.get_mpz_t(), pV);
	return pair<mpz_class, mpz_class>(mpz_class(gV), mpz_class(hV));
}

mpz_class HonestVerifier::getTetaValue(){
	teta = w2 + f*t;
	return teta;
}

void HonestVerifier::setBallot(Voter voter){
	ballot = voter.getFinalBallot();
}

void HonestVerifier::setGenerators(mpz_class *gens, int candNum){
	generators = gens;
	candidateNum = candNum;
}
