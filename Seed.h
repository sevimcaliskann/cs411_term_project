#ifndef SEED_H
#define SEED_H
#include <iostream>
#include <string>
#include <mpir.h>
#include <random>
using namespace std;
extern long int getSeed();

class Seed{
public:
	Seed();
	Seed(const mpz_t &p, const mpz_t &q, const mpz_t &g);
	void setRandomQ(int iterations, int length = 165);
	void setRandomP(int iterations, int length = 1024);
	void setGenerator(int iteration);
	string getExternalGenerator(const mpz_t &p, const mpz_t &q, int iteration);
	string getQ(int base);
	string getP(int base);
	string getG(int base);
	string getExternalG();
	string getExternalGenerator(string p, string q);
private:
	mpz_t p, q, a;
	int calculateJacobian(mpz_t a, mpz_t b);
	void createRandomNumber(mpz_t &num, int length);
	bool fermatTest(const mpz_t &num, long long int iterations);
	bool millerRabinTest(const mpz_t &num, long long int iterations);
	bool solovayStrassenTest(mpz_t num, int iterations);
	bool isPrime(mpz_t q);
};


#endif