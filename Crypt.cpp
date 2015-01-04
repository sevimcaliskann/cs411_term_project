#include "Crypt.h"
#include <iostream>
#include <string>
#include <mpir.h>
#include "Seed.h"
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
#include <map>
#include <random>
using namespace std;

Crypt::Crypt(){
	ranGen = new Seed();
	mpz_init(publicKey);
	initRandomR(ranGen->getQ(10), privateKey);
	mpz_class g(ranGen->getG(10));
	mpz_class p(ranGen->getP(10));
	mpz_powm(publicKey, g.get_mpz_t(), privateKey, p.get_mpz_t());
}

Crypt::Crypt(Seed &r){
	ranGen = &r;
	mpz_init(publicKey);
	initRandomR(ranGen->getQ(10), privateKey);
	mpz_class g(ranGen->getG(10));
	mpz_class p(ranGen->getP(10));
	mpz_powm(publicKey, g.get_mpz_t(), privateKey, p.get_mpz_t());
}

mpz_class Crypt::getPublicKey(){
	return mpz_class(publicKey);
}

bool Crypt::isExist(mpz_class *gList, int size, string element){
	mpz_t x;
	mpz_init_set_str(x, element.c_str(), 10);
	for (int i = 0; i < size && gList[i] != NULL; i++){
		if (mpz_cmp(gList[i].get_mpz_t(), x) == 0)
			return true;
	}
	return false;
}

mpz_class *Crypt::getGenerators(int size){
	mpz_class *generators = new mpz_class[size];
	for (int i = 0; i < size; i++){
		string str;
		do{
			str = ranGen->getExternalG();
		} while (isExist(generators, size, str));
		generators[i] = str;
	}
	return generators;
}

string Crypt::printMpzClass(const mpz_class &t){
	mpz_t a;
	mpz_init(a);
	mpz_set(a, t.get_mpz_t());
	return mpz_get_str(NULL, 10, a);
}

void Crypt::printGenerators(mpz_class *gList, int size){
	for (int i = 0; i < size; i++){
		if (gList[i] != NULL)
			cout << printMpzClass(gList[i]) << endl;
		else
			cout << i + 1 << "th generator is NULL, there must be problem occured in generating numbers!" << endl;
	}
}

void Crypt::matchGeneratos(mpz_class *gList, map<string, mpz_class> &candidates){
	map<string, mpz_class>::iterator a = candidates.begin();
	for (int i = 0; i < candidates.size(); i++){
		if (gList[i] != NULL){
			a->second = gList[i];
		}
		else{
			string str;
			do{
				str = ranGen->getExternalG();
			} while (isExist(gList, candidates.size(), str));
			a->second = str;
		}
		a++;
	}
}

pair<mpz_class, mpz_class> Crypt::voteCast(const pair<string, mpz_class> &candidate){
	pair<mpz_class, mpz_class> a;
	mpz_t ri, xi, yi, p;
	mpz_init(xi);
	mpz_init(yi);
	mpz_init_set_str(p, ranGen->getP(10).c_str(), 10);
	initRandomR(ranGen->getG(10), ri);
	mpz_set(r, ri);
	mpz_class g(ranGen->getG(10));
	mpz_powm(xi, g.get_mpz_t(), ri, p);//Bozulduğu nokta
	mpz_powm(yi, publicKey, ri, p);
	mpz_mul(yi, yi, candidate.second.get_mpz_t());
	mpz_mod(yi, yi, p);
	a.first = mpz_class(xi);
	a.second = mpz_class(yi);
	return a;
}

void Crypt::initRandomR(string q, mpz_t &num){
	mpz_class f;
	mpz_init(num);
	do{
		long int x = getSeed();
		gmp_randclass r1(gmp_randinit_mt);
		r1.seed(x);
		f = r1.get_z_range(mpz_class(q));
	} while (mpz_cmp_ui(f.get_mpz_t(), 0) == 0);
	mpz_set(num, f.get_mpz_t());
}

pair<mpz_class, mpz_class> Crypt::voteTally(map<mpz_class, mpz_class> votes){
	pair<mpz_class, mpz_class>result(1, 1);
	map<mpz_class, mpz_class>::iterator it = votes.begin();
	for (int i = 0; i < votes.size(); i++){
		result.first = (result.first * it->first) % mpz_class(ranGen->getP(10));
		result.second = (result.second * it->second) % mpz_class(ranGen->getP(10));
		it++;
	}
	return result;
}

map<mpz_class, mpz_class> Crypt::getVoteSeries(map<string, mpz_class> candidates, string *votes, int size){
	map<mpz_class, mpz_class> results;
	for (int i = 0; i < size; i++){
		if (votes[i].c_str()==NULL)
			cout << "NULL vote!!!" << endl;
		else{
			if (candidates.find(votes[i]) != candidates.end()){
				results.insert(voteCast(make_pair(candidates.find(votes[i])->first, candidates.find(votes[i])->second)));
			}
			else
				cout << "Invalid Vote!!" << endl;
		}
	}
	return results;
}

mpz_class Crypt::getVoteSUm(const pair<mpz_class, mpz_class> &tall){
	mpz_t a, p;
	mpz_init_set_str(p, ranGen->getP(10).c_str(), 10);
	mpz_init(a);
	mpz_powm(a, tall.first.get_mpz_t(), privateKey, p);
	mpz_invert(a, a, p);
	mpz_mul(a, a, tall.second.get_mpz_t());
	mpz_mod(a, a, p);
	return mpz_class(a);
}

map<string, mpz_class> Crypt::returnVoteResults(map<string, mpz_class> candidates, mpz_class sum, int voteSize){
	mpz_t x,p, g1,g2,g3, g4, mult;
	mpz_init(g1); mpz_init(g2); mpz_init(g3); mpz_init(g4); mpz_init(mult);
	mpz_init_set_str(p, ranGen->getP(10).c_str(), 10);
	map<string, mpz_class>::iterator it, it1, it2, it3;
	it = candidates.begin(); it1 = ++candidates.begin(); it2 = ++(++candidates.begin()); it3 = --candidates.end(); //CAUTIOOOONNNN
	map<string, mpz_class> results;
	results.insert(make_pair(it->first, mpz_class(0)));
	results.insert(make_pair(it1->first, mpz_class(0)));
	results.insert(make_pair(it2->first, mpz_class(0)));
	results.insert(make_pair(it3->first, mpz_class(0)));
	for (int i = 0; i <= voteSize; i++){
		for (int j = 0; j <= voteSize - i; j++){
			for (int k = 0; k <voteSize-i-j ; k++){
				int lV = voteSize - i - j - k;
				mpz_powm_ui(g1, it->second.get_mpz_t(), i, p);
				mpz_powm_ui(g2, it1->second.get_mpz_t(), j, p);
				mpz_powm_ui(g3, it2->second.get_mpz_t(), k, p);
				mpz_powm_ui(g4, it3->second.get_mpz_t(), lV, p);
				mpz_mul(mult, g1, g2);
				mpz_mul(mult, mult, g3);
				mpz_mul(mult, mult, g4);
				mpz_mod(mult, mult, p);
				if (mpz_cmp(sum.get_mpz_t(), mult) == 0){
					results.find(it->first)->second = i;
					results.find(it1->first)->second = j;
					results.find(it2->first)->second = k;
					results.find(it3->first)->second = lV;
					return results;
				}
			}
		}
	}
	return results;
}