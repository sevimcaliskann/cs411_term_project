#include "Seed.h"
#include <iostream>
#include <string>
#include <mpir.h>
#include <math.h>
#include <random>
#pragma warning(disable: 4800)
#include <gmpxx.h>
#pragma warning(default: 4800)
using namespace std;

Seed::Seed(const mpz_t &pN, const mpz_t &qN, const mpz_t &g){
	mpz_init(p);
	mpz_init(q);
	mpz_init(a);
	mpz_set(p, pN);
	mpz_set(q, qN);
	mpz_set(a, g);
}

Seed::Seed(){
	mpz_init(p);
	mpz_init(q);
	mpz_init(a);
	do{
		setRandomQ(160);
	} while (mpz_cmp_ui(q, 0) == 0);
	cout << " Q IS CREATED!" << endl;
	do{
		setRandomP(1024);
	} while (mpz_cmp_ui(p, 0) == 0);
	cout << "P IS CREATED! " << endl;
	setGenerator(1024);
	cout << "G IS CREATED!" << endl;
}

void Seed::createRandomNumber(mpz_t &num, int length){
	mpz_class f;
	do{
		long int x = getSeed();
		gmp_randclass r1(gmp_randinit_mt);
		r1.seed(x);
		f = r1.get_z_bits(length);
	} while (mpz_cmp_ui(f.get_mpz_t(), 0) == 0);
	mpz_set(num, f.get_mpz_t());
}

bool Seed::fermatTest(const mpz_t &num, long long int iterations){
	mpz_t rand, upperBound;
	mpz_init(rand);
	mpz_init(upperBound);
	mpz_sub_ui(upperBound, num, 1);//upperBound = num-1
	gmp_randstate_t state;
	gmp_randinit_mt(state);
	for (int i = 0; i < iterations; i++){
		mpz_urandomm(rand, state, num); // Random number between 0 and p-1
		mpz_powm(rand, rand, upperBound, num);
		if (mpz_cmp_ui(rand, 1) != 0)
			return false;
	}
	return true;
}

bool Seed::millerRabinTest(const mpz_t &num, long long int iterations){
	bool x = false;
	mpz_t r, s, rand, temp, upperBound;
	mpz_init(r);
	mpz_init(s);
	mpz_init(rand);
	mpz_init(temp);
	mpz_init(upperBound);
	mpz_sub_ui(upperBound, num, 1);
	gmp_randstate_t state; // For creating random number
	gmp_randinit_mt(state);
	mpz_sub_ui(s, num, 1);
	if (mpz_cmp_ui(num, 2) < 0)
		return false;
	mpz_mod_ui(r, num, 2);
	if (mpz_cmp_ui(num, 2) != 0 && mpz_cmp_ui(r, 0) == 0)
		return false;
	do{
		mpz_fdiv_q_ui(s, s, 2);
		mpz_mod_ui(r, s, 2);
	} while (mpz_cmp_ui(r, 0) == 0);
	for (int i = 0; i < iterations; i++){
		mpz_urandomm(rand, state, num);
		mpz_set(temp, s);
		mpz_powm(rand, rand, temp, num);
		if (mpz_cmp_ui(rand, 1) == 0 || mpz_cmp(rand, upperBound) == 0) continue;
		while (mpz_cmp(temp, upperBound) != 0 && mpz_cmp_ui(rand, 1) != 0 && mpz_cmp(rand, upperBound) != 0){
			mpz_powm_ui(rand, rand, 2, num);
			mpz_mul_ui(temp, temp, 2);
		}
		if (mpz_cmp(temp, upperBound) != 0 && mpz_cmp(rand, upperBound) != 0) return false;
	}

	return true;
}

bool Seed::solovayStrassenTest(mpz_t num, int iterations){
	mpz_t temp, rand, jacobian;
	mpz_init(temp);
	mpz_init(rand);
	mpz_init(jacobian);
	gmp_randstate_t state;
	gmp_randinit_mt(state);
	mpz_mod_ui(temp, num, 2);
	if (mpz_cmp_ui(num, 2) < 0) return false;
	if (mpz_cmp_ui(num, 2) == 0 || mpz_cmp_ui(temp, 0) == 0) return false;
	for (int i = 0; i < iterations; i++){
		mpz_urandomm(rand, state, num);
		mpz_add_ui(jacobian, num, calculateJacobian(a, num));
		mpz_mod(jacobian, jacobian, num);
		mpz_sub_ui(temp, num, 1);
		mpz_fdiv_q_ui(temp, temp, 2);
		mpz_powm(temp, a, temp, num);
		if (mpz_cmp_ui(jacobian, 0) != 0 || mpz_cmp(temp, jacobian) != 0) return false;
	}
	return true;
}

void Seed::setRandomQ(int iterationNum, int length){
	int num = iterationNum;
	int iterations = 100;
	mpz_t qNumber, modulo;
	mpz_init(qNumber); mpz_init(modulo);
	do{
		createRandomNumber(qNumber, length);
		mpz_mod_ui(modulo, qNumber, 2);
	} while (mpz_cmp_ui(qNumber, 0) == 0 || mpz_cmp_ui(modulo, 0)==0);
	while (!isPrime(qNumber)){
		mpz_add_ui(qNumber, qNumber, 2);
	}
	mpz_set(q, qNumber);
}

bool Seed::isPrime(mpz_t qNumber){
	mpz_t q; mpz_init_set(q, qNumber);
	if (fermatTest(q, 300)){
		if (millerRabinTest(q, 300)){
			if (solovayStrassenTest(qNumber, 300)){
				return true;
			}
		}
	}
	return false;
}

int Seed::calculateJacobian(mpz_t a, mpz_t n){
	mpz_t s, temp;
	mpz_init(s);
	mpz_init(temp);
	if (mpz_cmp_ui(a, 0) == 0) return 0;
	int answer = 1;
	if (mpz_cmp_ui(a, 0) < 0){
		mpz_ui_sub(a, 0, a);
		mpz_mod_ui(s, n, 4);
		if (mpz_cmp_ui(s, 3) == 0) answer *= -1;
	}
	if (mpz_cmp_ui(a, 1) == 0) return answer;
	while (mpz_cmp_ui(a, 0) != 0){
		if (mpz_cmp_ui(a, 0) < 0){
			mpz_ui_sub(a, 0, a);
			mpz_mod_ui(s, n, 4);
			if (mpz_cmp_ui(s, 3) == 0) answer *= -1;
		}
		mpz_mod_ui(s, a, 2);
		while (mpz_cmp_ui(s, 0) == 0){
			mpz_fdiv_q_ui(a, a, 2);
			mpz_mod_ui(s, n, 8);
			if (mpz_cmp_ui(s, 3) == 0 || mpz_cmp_ui(s, 5) == 0) answer *= -1;
			mpz_mod_ui(s, a, 2);
		}
		mpz_set(temp, a);
		mpz_set(a, n);
		mpz_set(n, temp);
		mpz_mod_ui(s, a, 4);
		mpz_mod_ui(temp, n, 4);
		if (mpz_cmp_ui(s, 3) == 0 || mpz_cmp_ui(temp, 3) == 0) answer *= -1;
		mpz_mod(a, a, n);
		mpz_fdiv_q_ui(temp, n, 2);
		if (mpz_cmp(a, temp) > 0) mpz_sub(a, a, n);
	}
	if (mpz_cmp_ui(n, 1) == 0) return answer;
	return 0;
}

void Seed::setRandomP(int iterationNum, int length){
	int num = iterationNum;
	mpz_t randNum, modulo, temp;
	mpz_init(randNum); mpz_init(modulo); mpz_init(temp);
	int iterations = 100;
	do{
		createRandomNumber(randNum, length - 160 + 1);
		mpz_mod_ui(modulo, randNum, 2);
	} while (mpz_cmp_ui(randNum, 0) == 0 || mpz_cmp_ui(modulo, 0) == 1);
	mpz_mul(temp, randNum, q);
	mpz_add_ui(temp, temp, 1);
	while (!isPrime(temp)){
		mpz_add_ui(randNum, randNum, 2);
		mpz_mul(temp, randNum, q);
		mpz_add_ui(temp, temp, 1);
	}
	mpz_set(p, temp);
}

void Seed::setGenerator(int iteration){
	mpz_t power, rand, temp;
	mpz_class f;
	string qStr = mpz_get_str(NULL, 2, q);
	int size = qStr.length();
	mpz_init(rand);
	mpz_init(power);
	mpz_init(temp);
	mpz_sub_ui(power, p, 1);
	mpz_fdiv_q(power, power, q);
	for (int i = 0; i < iteration; i++){
		long int x = getSeed();
		gmp_randclass r1(gmp_randinit_mt);
		r1.seed(x);
		f = r1.get_z_bits(size);
		mpz_set(rand, f.get_mpz_t());
		mpz_powm(temp, rand, power, p);
		if (mpz_cmp_ui(temp, 1) != 0){
			mpz_set(a, rand);
			break;
		}
	}
}

string Seed::getExternalGenerator(const mpz_t &pNum, const mpz_t &qNum, int iteration){
	mpz_t power, rand, temp; // qNum size 
	mpz_class f;
	string qStr = mpz_get_str(NULL, 2, qNum);
	int size = qStr.length();
	mpz_init(rand);
	mpz_init(power);
	mpz_init(temp);
	mpz_sub_ui(power, pNum, 1);
	mpz_fdiv_q(power, power, q);
	for (int i = 0; i < iteration; i++){
		long int x = getSeed();
		gmp_randclass r1(gmp_randinit_mt);
		r1.seed(x);
		f = r1.get_z_bits(size);
		mpz_set(rand, f.get_mpz_t());
		mpz_powm(temp, rand, power, pNum);
		if (mpz_cmp_ui(temp, 1) != 0){
			return mpz_get_str(NULL, 10, rand);
		}
	}
	return "No generator found";
}

string Seed::getExternalG(){
	return getExternalGenerator(p, q, 1000);
}

string Seed::getExternalGenerator(string pstr, string qstr){
	mpz_t pNum, qNum;
	mpz_init_set_str(pNum, pstr.c_str(), 10);
	mpz_init_set_str(qNum, qstr.c_str(), 10);
	return getExternalGenerator(pNum, qNum, 1000);
}

string Seed::getG(int base){
	if (base > 32)
		throw invalid_argument("received greater than 32");
	return mpz_get_str(NULL, base, a);
}

string Seed::getP(int base){
	if (base > 32)
		throw invalid_argument("received greater than 32");
	return mpz_get_str(NULL, base, p);
}

string Seed::getQ(int base){
	if (base > 32)
		throw invalid_argument("received greater than 32");
	return mpz_get_str(NULL, base, q);
}