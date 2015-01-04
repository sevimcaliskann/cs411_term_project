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
	checkVote(cy, list, list[0], ranGen, make_pair("a", list[0]));
	checkVote(cy, list, list[1], ranGen, make_pair("b", list[1]));
	checkVote(cy, list, list[2], ranGen, make_pair("c", list[2]));
	checkVote(cy, list, list[3], ranGen, make_pair("d", list[3]));
	mpz_class square = mpz_class(ranGen.getExternalG());
	checkVote(cy, list, square, ranGen, make_pair("b", square));
}

int main(){
	Seed ranGen;
	Crypt cy(ranGen);
	Distributer dist(ranGen, 3, cy);
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

	voteGeneration(candidates, cy, v);
	voteCheck(cy, list, ranGen);

	Keeper klist[5] = { Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3), Keeper(ranGen, dist, 3) };
	map<mpz_class, mpz_class> voteSeries = cy.getVoteSeries(candidates, v, 100);
	pair<mpz_class, mpz_class> voteTally = cy.voteTally(voteSeries);
	for (int i = 0; i < 5; i++)
		klist[i].setBallot(voteTally);
	mpz_class vSum = klist[0].getSumForExhaustiveSearch(klist, 5);
	cout << "The second vote sum : " << cy.printMpzClass(vSum) << endl;

	cin.get();
	cin.ignore();
	return 0;
}