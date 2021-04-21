#include "stdafx.h"
#include "QuantumComputation.h"

Unitary haddamard() {
	Unitary had(2, 2);
	had[0][0] = Complex(1, 0);
	had[0][1] = Complex(1, 0);
	had[1][0] = Complex(1, 0);
	had[1][1] = Complex(-1, 0);
	return had;
}

Unitary x() {
	Unitary x(2, 2);
	x[0][0] = Complex(0, 0);
	x[0][1] = Complex(1, 0);
	x[1][0] = Complex(1, 0);
	x[1][1] = Complex(0, 0);
	return x;
}

Unitary z() {
	Unitary z(2, 2);
	z[0][0] = Complex(1, 0);
	z[0][1] = Complex(0, 0);
	z[1][0] = Complex(0, 0);
	z[1][1] = Complex(-1, 0);
	return z;
}

Unitary Haddamard = haddamard();
Unitary X = x();
Unitary Z = z();


bool compareTwoStates(std::vector<Qubit*>* first, std::vector<Qubit*>* second) {
	if (first->size() != second->size()) {
		return false;
	}

	for (int i = 0; i < first->size(); i++) {
		if (first->at(i)->compare(second->at(i))) {
			return false;
		}
	}

	return true;
}

QuExpression::QuExpression(QuExpression* left, QuExpression* right, char sign){
	this->left = left;
	this->right = right;
	this->sign = sign;
	this->left->setParent(this);
	this->right->setParent(this);
};

QuExpression::QuExpression(QuExpression* right, char sign){
	this->right = right;
	this->sign = sign;
	if (right != NULL) {
		this->right->setParent(this);
	}
};

void QuExpression::applyUnitary(Matrix<Complex> unitary, std::vector<int> target){
	int choice = target.back();
	target.pop_back();
	if (choice == 0){
		left->applyUnitary(unitary, target);
	}
	else{
		right->applyUnitary(unitary, target);
	}
};

QuExpression* QuExpression::normalize(){ 
	if (left != NULL) {
		left = left->normalize();
		if (left != this) {
			left->setParent(this);
		}
	}
	
	if (right != NULL) {
		right = right->normalize();
		if (right != this) {
			right->setParent(this);
		}
	}

	return this;
};

QuExpression* QuExpression::collapse(){
	if (sign == '*' && right->getType() == QUBIT){
		return left->multiply(static_cast<Qubit*>(right));
	}
	else if (sign == '+' || sign == '-') {
		if (left != NULL) {
			QuExpression* collapsed = left->collapse();
			if (collapsed != NULL && collapsed != this) {
				left = collapsed;
				left->setParent(this);
			}
		}

		if (right != NULL) {
			QuExpression* collapsed = right->collapse();
			if (collapsed != NULL && collapsed != this) {
				right = collapsed;
				right->setParent(this);
			}
		}
		return this;
	}
	return NULL;
};

QuExpression* QuExpression::multiply(Qubit* qubit){
	if (sign == '+' || sign == '-') {
		if (left != NULL) {
			QuExpression* multiplied = left->multiply(qubit);
			if (multiplied != NULL && multiplied != this) {
				left = multiplied;
				left->setParent(this);
			}
		}

		if (right != NULL) {
			QuExpression* multiplied = right->multiply(qubit);
			if (multiplied != NULL && multiplied != this) {
				right = multiplied;
				right->setParent(this);
			}
		}
		return this;
	}
	else if (sign == '*') {
		if (right != NULL) {
			QuExpression* multiplied = right->multiply(qubit);
			if (multiplied != NULL && multiplied != this) {
				right = multiplied;
				right->setParent(this);
			}
			return this;
		}
	}
	return NULL;
};

void QuExpression::applyCNOT(){
	if (sign == '+' || sign == '-'){
		if (left != NULL) {
			left->applyCNOT();
		}
		right->applyCNOT();
	}
	else if (sign == '*' && left->getType() == QUBIT && right->getType() == QUBIT && left->evaluate() == 1){
		static_cast<Qubit*>(right)->swap();
	}
};

int QuExpression::evaluate(){
	return -1;
};

ExpressionTypes QuExpression::getType(){
	return EXPRESSION;
};

std::wstring QuExpression::output(){
	std::wstring result;
	if (left != NULL){
		result += left->output();
	} 
	result += sign;
	result += right->output();
	return result;
};

void QuExpression::setParent(QuExpression* parent) {
	this->parent = parent;
}

void QuExpression::checkBackReferences() {
	std::cout << this << '\n';
	if (parent != NULL) {
		parent->checkBackReferences();
	}
	else {
		std::cout << "Got to root this path is OK \n";
	}
}

void QuExpression::traverseWholeTreeToConsole() {
	if (left != NULL) {
		left->traverseWholeTreeToConsole();
	}
	if (right != NULL) {
		left->traverseWholeTreeToConsole();
	}
	
	if (left == NULL && right == NULL) {
		checkBackReferences();
	}
}

bool QuExpression::traversForward(QuExpression* memberToCollapse, std::vector<Qubit*>* state, char sign){
	if(left != NULL && right != NULL && this->sign == '*'){
		std::vector<Qubit*>* newState = tryCollapseMultiplication();
		if (state != NULL){
			if (compareTwoStates(newState, state) && ((parent->sign == sign && parent->right == this) || (parent->parent->sign == sign && parent->left == this))) {
				if (memberToCollapse->parent->left == memberToCollapse){
					memberToCollapse->parent->left = NULL;
				}
				else{
					memberToCollapse->parent->right = NULL;
				}
				if (parent->left == this){
					parent->left = NULL;
				}
				else{
					parent->right = NULL;
				}
				return true;
			}
		}
	}
	if (left != NULL && left->traversForward(memberToCollapse, state, sign)){
		return true;
	}
	if (right != NULL && right->traversForward(memberToCollapse, state, sign)){
		return true;
	}
	return false;
};

void QuExpression::traverseBack(QuExpression* prev, QuExpression* memberToCollapse, std::vector<Qubit*>* state, char sign){
	if (this->sign == '*' || parent == NULL){
		QuExpression* root = this->sign == '*' ? prev : this;
		root->traversForward(memberToCollapse, state, sign);
	}
	else {
		parent->traverseBack(this, memberToCollapse, state, sign);
	}
};

void QuExpression::tryCollapseMembers(){
	if (left != NULL && right != NULL && sign == '*' && parent != NULL){
		std::vector<Qubit*>* state = tryCollapseMultiplication();
		if (state != NULL){
			char sign = '-';
			if ((parent->sign == '-' && parent->right == this) || (parent->parent != NULL && parent->parent->sign == '-')){
				sign = '+';
			}
			parent->traverseBack(this, this, state, sign);
			return;
		}
	}
	if (left != NULL){
		left->tryCollapseMembers();
	}
	if (right != NULL){
		right->tryCollapseMembers();
	} 
	if (left == NULL && right == NULL){
		std::vector<Qubit*>* state = new std::vector<Qubit*>;
		state->push_back(static_cast<Qubit*>(this));
		char sign = parent->sign == '+' ? '-' : '+';
		traverseBack(this, this, state, sign);
	}
};

std::vector<Qubit*>* QuExpression::tryCollapseMultiplication() {
	if (sign == '+' || sign == '-') {
		return NULL;
	}

	std::vector<Qubit*>* state = new std::vector<Qubit*>;
	if (left->getType() == QUBIT) {
		state->push_back(static_cast<Qubit*>(left));
	}
	else {
		std::vector<Qubit*>* leftCollapse = left->tryCollapseMultiplication();
		if (leftCollapse == NULL) {
			return NULL;
		}
		state->insert(state->end(), leftCollapse->begin(), leftCollapse->end());
	}

	if (right->getType() == QUBIT) {
		state->push_back(static_cast<Qubit*>(right));
	}
	else {
		std::vector<Qubit*>* rightCollapse = right->tryCollapseMultiplication();
		if (rightCollapse == NULL) {
			return NULL;
		}
		state->insert(state->end(), rightCollapse->begin(), rightCollapse->end());
	}
	return state;
}

//----------QUBIT-----------//

Qubit::Qubit(int state) : QuExpression(NULL, '+') {
	this->state = new Vector<Complex>(2);
	if (state == 0){
		this->state->set(0, Complex(1, 0));
		this->state->set(1, Complex(0, 0));
 	}
	else if (state == 1){
		this->state->set(0, Complex(0, 0));
		this->state->set(1, Complex(1, 0));
	}
	else{
		throw "There can be no state like that";
	}
};

Qubit::Qubit(Vector<Complex>* state) : QuExpression(NULL, '+'){
	if (state->getLength() > 2){
		throw "Only one qubit allowed";
	}
	
	this->state = state;
};

Qubit::Qubit(char sign, int state) : QuExpression(NULL, sign){
	this->state = new Vector<Complex>(2);
	if (state == 0){
		this->state->set(0, Complex(1, 0));
		this->state->set(1, Complex(0, 0));
	}
	else if (state == 1){
		this->state->set(0, Complex(0, 0));
		this->state->set(1, Complex(1, 0));
	}
	else{
		throw "There can be no state like that";
	}
};

Qubit::Qubit(char sign, Vector<Complex>* state) : QuExpression(NULL, sign){
	if (state->getLength() > 2){
		throw "Only one qubit allowed";
	}

	this->state = state;
};

std::wstring Qubit::output(){
	if (state->get(0).compare(1, 0)) {
		return L"|0>";
	}
	else if (state->get(0).compare(0, 0)) {
		return L"|1>";
	}
	else {
		return L"a|0> + b|1>";
	}
};

void Qubit::applyUnitary(Matrix<Complex> unitary, std::vector<int> target){
	*state = unitary * (*state);
};

QuExpression* Qubit::normalize(){
	if (state->get(0).compare(1, 0) && state->get(1).compare(1, 0)){
		Qubit* left = new Qubit(0);
		Qubit* right = new Qubit(1);
		QuExpression* expression = new QuExpression(left, right, '+');
		return expression;
	}
	else if (state->get(0).compare(1, 0) && state->get(1).compare(-1, 0)){
		Qubit* left = new Qubit(0);
		Qubit* right = new Qubit(1);
		QuExpression* expression = new QuExpression(left, right, '-');
		return expression;
	}
	else{
		return this;
	}
};

QuExpression* Qubit::multiply(Qubit* qubit){
	QuExpression* expression = new QuExpression(this, qubit,'*');
	return expression;
};

int Qubit::evaluate(){
	if (state->get(0).compare(1, 0)) {
		return 1;
	}
	else if (state->get(0).compare(0, 0)) {
		return 0;
	}
	else {
		//TODO:implement measure
		return -1;
	}
};

void Qubit::swap(){
	Complex temp = state->get(0);
	state->set(0, state->get(1));
	state->set(1, temp);
};

ExpressionTypes Qubit::getType(){
	return QUBIT;
};

bool Qubit::compare(Qubit* anotherQubit) {
	Vector<Complex>* anotherState = anotherQubit->getState();
	return anotherState->get(0).compare(state->get(0)) && anotherState->get(1).compare(state->get(1));
}


Vector<Complex>* Qubit::getState() {
	return state;
}