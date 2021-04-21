#ifndef QUANTUM_COMPUTATION_H
#define QUANTUM_COMPUTATION_H

#include "Math_lib.h"
#include <vector>

typedef Matrix<Complex> Unitary;

extern Unitary Haddamard;
extern Unitary X;
extern Unitary Z;

class Qubit;

enum ExpressionTypes {EXPRESSION, QUBIT, COMPLEX};

bool compareTwoStates(std::vector<Qubit*>* first, std::vector<Qubit*>* second);

class QuExpression {
protected:
	QuExpression* left = NULL;
	QuExpression* right = NULL;
	QuExpression* parent = NULL;
	//TODO: change sign from char to enum
	char sign;
	std::vector<Qubit*>* tryCollapseMultiplication();
	void traverseBack(QuExpression* prev, QuExpression* memberToCollapse, std::vector<Qubit*>* state, char sign);
	bool traversForward(QuExpression* memberToCollapse, std::vector<Qubit*>* state, char sign);
	void checkBackReferences();

public:
	QuExpression(QuExpression* left, QuExpression* right, char sign);
	QuExpression(QuExpression* right, char sign);
	//TODO: Change target vector template from <int> to some enum like {LEFT, RIGHT}
	virtual void applyUnitary(Matrix<Complex> unitary, std::vector<int> target);
	virtual std::wstring output();
	virtual QuExpression* normalize();
	virtual QuExpression* collapse();
	virtual ExpressionTypes getType();
	virtual QuExpression* multiply(Qubit* qubit);
	virtual int evaluate();
	void setParent(QuExpression* parent);
	void applyCNOT();
	void tryCollapseMembers();
	void traverseWholeTreeToConsole();
};

class Qubit:public QuExpression{
private:
	Vector<Complex>* state = NULL;
public:
	Qubit(int state);
	Qubit(Vector<Complex>* state);
	Qubit(char sign, int state);
	Qubit(char sign, Vector<Complex>* state);
	std::wstring output();
	void applyUnitary(Matrix<Complex> unitary, std::vector<int> target);
	QuExpression* normalize();
	ExpressionTypes getType();
	QuExpression* multiply(Qubit* qubit);
	bool compare(Qubit* anotherQubit);
	Vector<Complex>* getState();
	int evaluate();
	void swap();
};

#endif