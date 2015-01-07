#include "Crypt.h"
#include "Voter.h"
#include "HonestVerifier.h"
#include <iostream>
#include <string>
#include <mpir.h>
#include "Seed.h"
#include "Distributer.h"
#include "Keeper.h"
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
using namespace std;

extern long int getSeed(){
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, RAND_MAX);
	return dis(gen);
}

void createCandidates(map<string, mpz_class> &candidates){
	candidates.insert(make_pair("a", mpz_class(0)));
	candidates.insert(make_pair("b", mpz_class(0)));
	candidates.insert(make_pair("c", mpz_class(0)));
	candidates.insert(make_pair("d", mpz_class(0)));
}

void voteGeneration(map<string, mpz_class> &candidates, Crypt &cy, string v[100]){
	map<mpz_class, mpz_class> voteSeries = cy.getVoteSeries(candidates, v, 100);
	pair<mpz_class, mpz_class> voteTally = cy.voteTally(voteSeries);
	mpz_class voteSum = cy.getVoteSUm(voteTally);
	cout << "Vote Sum : " << cy.printMpzClass(voteSum) << endl;
	map<string, mpz_class> results = cy.returnVoteResults(candidates, voteSum, 100);
	map<string, mpz_class>::iterator it = results.begin();
	for (int i = 0; i < results.size(); i++){
		cout << "The candidate : " << it->first << " the vote he gets is: " << cy.printMpzClass(it->second) << endl;
		it++;
	}
}

void checkVote(Crypt &cy, mpz_class *list, mpz_class candGen, Seed &ranGen, pair<string, mpz_class> vote){
	Voter voter(list, ranGen, 4);
	voter.setBallot(cy, vote);
	voter.setPublicKey(cy.getPublicKey()); // set public key setballot sonrasında çağırılmalı, randomizer'ı vote'u oluştururken alacak
	voter.setCandidateGenerator(candGen);

	HonestVerifier hv;
	hv.setGHQP(mpz_class(ranGen.getG(10)), cy.getPublicKey(), mpz_class(ranGen.getQ(10)), mpz_class(ranGen.getP(10)));
	hv.setGenerators(list, 4);
	hv.setBallot(voter);
	hv.setCandidateGenerator(candGen);

	voter.isValidFromHV(hv);
}

void voteCheck(Crypt &cy, mpz_class *list, Seed &ranGen){
	cout << endl<<"Vote Checking, respectively, a,b,c,d, invalid votes: " << endl;
	checkVote(cy, list, list[0], ranGen, make_pair("a", list[0]));
	checkVote(cy, list, list[1], ranGen, make_pair("b", list[1]));
	checkVote(cy, list, list[2], ranGen, make_pair("c", list[2]));
	checkVote(cy, list, list[3], ranGen, make_pair("d", list[3]));
	mpz_class square = mpz_class(ranGen.getExternalG());
	checkVote(cy, list, square, ranGen, make_pair("b", square));
}

mpz_class *getList(mpz_class *list, int index1, int index2, mpz_class p,mpz_class res,  bool inv){
	mpz_t temp; mpz_init(temp);
	mpz_class *list1 = new mpz_class[100];
	mpz_class *list2 = new mpz_class[100];
	for (int i = 0; i < 100; i++){
		mpz_powm(temp, list[index1].get_mpz_t(), mpz_class(i).get_mpz_t(), p.get_mpz_t());
		if (inv)
			mpz_invert(temp, temp, p.get_mpz_t());
		list1[i] = mpz_class(temp);
		mpz_powm(temp, list[index2].get_mpz_t(), mpz_class(i).get_mpz_t(), p.get_mpz_t());
		if (inv)
			mpz_invert(temp, temp, p.get_mpz_t());
		list2[i] = mpz_class(temp);
	}
	mpz_class *first = new mpz_class[10000]; int c = 0;
	for (int i = 0; i < 100; i++){
		for (int j = 0; j < 100; j++){
			mpz_class a = (list1[i] * list2[j]) % p;
			if (inv){
				a *= res;
				a %= p;
			}
			first[c++] = a;
		}
	}
	return first;
}

void babyStep_giantStep(mpz_class *list, mpz_class res, mpz_class p){
	cout <<endl<< "Baby Step - Giant Step Phase: " << endl;
	mpz_class *first = getList(list, 0, 1, p, res, false);
	mpz_class *second = getList(list, 2, 3, p, res, true);
	for (int i = 0; i < 10000; i++){
		for (int j = 0; j < 10000; j++){
			if (second[i] == first[j] && (i!=0 && j!=0)){
				/*cout << "DONE, i: "<<i<<" j: "<< j << endl;*/
				cout << "a-Vote: " << j / 100 << endl;
				cout << "b-Vote: " << j % 100 << endl;
				cout << "c-Vote: " << i / 100<<endl;
				cout << "d-Vote: " << i % 100 << endl;
				return;
			}
		}
	}

}



int main(){
	Seed ranGen;
	Crypt cy(ranGen);
	cout << "generator g: " << ranGen.getG(10) << endl << endl;

	map<string, mpz_class> candidates;
	createCandidates(candidates);

	mpz_class *list = cy.getGenerators(4);
	cy.matchGeneratos(list, candidates);

	string v[100];
	for (int i = 0; i < 100; i++){
		long int num = getSeed() % 4;
		char vote = (char)(97 + num);
		v[i] = vote;
	}

	/*voteGeneration(candidates, cy, v);*/
	voteCheck(cy, list, ranGen);
	map<mpz_class, mpz_class> voteSeries = cy.getVoteSeries(candidates, v, 100);
	pair<mpz_class, mpz_class> voteTally = cy.voteTally(voteSeries);
	Distributer dist(ranGen, 2, cy, voteTally.first);
	Keeper klist[5] = { Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3) };

	for (int i = 0; i < 5; i++)
		klist[i].setBallot(voteTally);
	mpz_class vSum = klist[0].getSumForExhaustiveSearch(klist, 5);

	cout << endl<<"Vote Tallying Phase by older exhaustive search: " << endl;
	map<string, mpz_class> results = cy.returnVoteResults(candidates, vSum, 100);
	map<string, mpz_class>::iterator it = results.begin();
	for (int i = 0; i < results.size(); i++){
		cout << "The candidate : " << it->first << " the vote he gets is: " << cy.printMpzClass(it->second) << endl;
		it++;
	}

	babyStep_giantStep(list, vSum, mpz_class(ranGen.getP(10)));

	cin.get();
	cin.ignore();
	return 0;
}