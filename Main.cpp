// RNG.cpp : Defines the entry point for the console application.
//

#define _CRT_RAND_S
#include "stdafx.h"
#include <codecvt>
#include <locale>
#include <stdio.h>
#include <windows.h>

#include "RNG.h"
#include "QuantumComputation.h"


#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdint>
#include <stdlib.h>

using namespace std;

void testRNG(int numbers) {
	std::ofstream fs("sequence.bin", std::ios::out | std::ios::binary | std::ios::app);
	for (int i = 0; i < numbers; i++) {
		bool result = generateRandomNumber(16);
		fs.write(reinterpret_cast<const char*>(&result), sizeof(bool));
	}
	fs.close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	wcout.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8_utf16<wchar_t>()));

	SetConsoleOutputCP(CP_UTF8);

	/*
	testRNG(1000);
	Vector<int> vec1(2);
	vec1[0] = 3;
	vec1[1] = 1;

	Vector<int> vec2(2);
	vec2[0] = 1;
	vec2[1] = 2;

	Matrix<int> mat(2, 2);
	mat[0][0] = 2;
	mat[0][1] = 7;
	mat[1][0] = 1;
	mat[1][1] = 9;

	Matrix<int> vec_vec = vec1 * vec2;
	std::cout << vec_vec << '\n';
	std::cout << vec_vec * mat;

	Complex c1(2, 2);
	Complex c2(3, 1);
	Complex c3 = !c2;

	
	std::cout << c1;
	std::cout << c2;
	std::cout << c3;

	std::cout << c1 - c2;
	std::cout << c2 + c3;

	int* qbits = new int[2];
	qbits[0] = 1;
	qbits[1] = 0;
	char* had = qbitToHadamard(qbits);
	std::cout << had << '\n';
	char* cnot = cnotToHadamard(had);
	std::cout << cnot << '\n';
	
	Complex c1(1, 3);
	Complex c2(5, 7);
	std::cout << (c1^c2) << '\n';
	*/

	Qubit qu1(0);
	Qubit qu2(0);
	QuExpression epr(&qu1, &qu2, '*');
	std::vector<int> target;
	target.push_back(0);
	Vector<Complex> state(2);
	state[0] = Complex(1, 0);
	state[1] = Complex(0, 0);
	epr.applyUnitary(Haddamard, target);
	epr = *epr.normalize();
	epr = *epr.collapse();
	epr.setParent(NULL);
	epr.applyCNOT();
	std::wcout << epr.output() << '\n';
	epr.applyCNOT();

	std::vector<int> target1;
	target1.push_back(0);
	target1.push_back(0);
	epr.applyUnitary(Haddamard, target1);

	std::vector<int> target2;
	target2.push_back(0);
	target2.push_back(1);
	epr.applyUnitary(Haddamard, target2);

	epr = *epr.normalize();
	epr = *epr.collapse();
	epr.tryCollapseMembers();

	std::wcout << epr.output() << '\n';
	system("PAUSE");
	return 0;
}

