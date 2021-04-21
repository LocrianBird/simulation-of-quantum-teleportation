#ifndef MATH_LIB_H
#define MATH_LIB_H

#include <iostream>
#include <cmath>

class Complex{
private:
	double Re;
	double Im;
public:
	Complex() {

	}

	Complex(double Re, double Im){
		this->Re = Re;
		this->Im = Im;
	}
	
	double getRe(){
		return Re;
	}

	double getIm(){
		return Im;
	}

	Complex& operator!(){
		return Complex(Re, -Im);
	}
	//TODO: overload operator ==
	bool compare(double re, double im) {
		return Re == re && Im == im;
	}

	bool compare(Complex another) {
		return Re == another.getRe() && Im == another.getIm();
	}

	friend std::ostream &operator<<(std::ostream &out, Complex c) {
		double im = c.getIm() < 0 ? -c.getIm() : c.getIm();
		char sign = c.getIm() < 0 ? '-' : '+';
		out << c.getRe() << ' ' << sign << ' ' << 'i' << im << '\n';
		return out;
	}

	double& operator~(){
		double result = std::sqrt(Re*Re + Im*Im);
		return result;
	}

	Complex& operator=(Complex& complex){
		Re = complex.getRe();
		Im = complex.getIm();
		return *this;
	}

	bool operator!=(Complex& complex) {
		return (complex.getRe() != Re) || (complex.getIm() == Im);
	}

	void operator+=(Complex& complex){
		Re += complex.getRe();
		Im += complex.getIm();
	}
};

inline int& operator^(Complex c1, Complex c2){
	double mod1 = ~c1;
	double mod2 = ~c2;
	int result = mod1 * mod1 + mod2 * mod2; 
	return result;
}

inline Complex operator+(Complex c1, Complex c2){
	return Complex(c1.getRe() + c2.getRe(), c1.getIm() + c2.getIm());
}

inline Complex operator-(Complex c1, Complex c2){
	return Complex(c1.getRe() - c2.getRe(), c1.getIm() - c2.getIm());
}

inline Complex operator*(Complex c1, Complex c2){
	double a = c1.getRe(), b = c1.getIm();
	double c = c2.getRe(), d = c1.getIm();
	double real = a * c - b * d;
	double im = a * d + c * b;
	return Complex(real, im);
}

template<typename T>
class Vector{
private:
	T* vector = nullptr;
	unsigned int length;

public:
	Vector(unsigned int length, T* vector){
		this->length = length;
		this->vector = vector;
	}
	
	Vector(unsigned int length){
		this->length = length;
		vector = new T[length];
		std::memset(vector, 0, length * sizeof(T));
	}

	void set(int pos, T val) {
		vector[pos] = val;
	}

	T get(int pos) {
		return vector[pos];
	}

	T& operator[](std::size_t idx){
		return vector[idx];
	}

	int getLength(){
		return length;
	}

	friend std::ostream &operator<<(std::ostream &out, Vector vec) {
		out << '[';
		int i = 0;
		for (; i < vec.getLength() - 1; i++) {
			out << vec[i] << ' ';
		}
		out << vec[i] << ']' << '\n';
		return out;
	}
};

template<typename T>
class Matrix {
private:
	T** matrix = nullptr;
	unsigned int rows;
	unsigned int columns;

public:
	Matrix(unsigned int rows, unsigned int columns, T** matrix){
		this->rows = rows;
		this->columns = columns;
		this->matrix = matrix;
	}

	Matrix(unsigned int rows, unsigned int columns){
		this->rows = rows;
		this->columns = columns;
		this->matrix = new T*[rows];
		for (int i = 0; i < rows; i++) {
			matrix[i] = new T[columns];
			std::memset(matrix[i], 0, columns * sizeof(T));
		}
	}

	T*& operator[](std::size_t idx) { 
		return matrix[idx]; 
	}

	int getRows() {
		return rows;
	}

	int getColumns() {
		return columns;
	}

	friend std::ostream &operator<<(std::ostream &out, Matrix mat) {	
		for (int i = 0; i < mat.getRows(); i++) {
			out << '|';
			int j = 0;
			for (; j < mat.getColumns() - 1; j++){
				out << mat[i][j] << ' ';
			}
			out << mat[i][j] << '|' << '\n';
		}
		return out;
	}
};

/*
		VECTOR
*/

//return scalar (-|)
template<typename T>
inline T operator&(Vector<T> first, Vector<T> second) {
	if (first.getLength() != second.getLength()){
		throw "Illegal dimensions";
	}
	T result = 0;
	for (int i = 0; i < first.getLength(); i++)
	{
		result += first[i] * second[i];
	}
	return result;
}


//return matrix (|-)
template <typename T>
inline Matrix<T> operator*(Vector<T> first, Vector<T> second) {
	Matrix<T> mat(first.getLength(), second.getLength());
	for (int i = 0; i < first.getLength(); i++)
	{
		for (int j = 0; j < second.getLength(); j++)
		{
			mat[i][j] = first[i] * second[j];
		}
	}
	return mat;
}

/*
		MATRIX		
*/


template<typename T>
inline Vector<T> operator*(Matrix<T>& mat, Vector<T> vec)
{
	int len_vec = vec.getLength();
	int rows = mat.getRows();
	int columns = mat.getColumns();
	if (columns != len_vec){
		throw "Illegal dimensions";
	}
	Vector<T> fin(rows);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < len_vec; j++)
		{
			fin[i] += mat[i][j] * vec[j];
		}
	}
	return fin;
}

template<typename T>
inline Vector<T> operator*(Vector<T> vec, Matrix<T>& mat)
{
	int len_vec = vec.getLength();
	int rows = mat.getRows();
	int columns = mat.getColumns();
	if (rows != len_vec){
		throw "Illegal dimensions";
	}
	Vector<T> fin(columns);
	for (int i = 0; i < columns; i++)
	{
		for (int j = 0; j < len_vec; j++)
		{
			fin[i] += vec[j] * mat[j][i];
		}
	}

	return fin;
}

template<typename T>
inline Matrix<T> operator*(Matrix<T> first, Matrix<T>& second) {
	if (first.getColumns() != second.getRows()) {
		throw "Illegal dimensions";
	}

	Matrix<T> result(first.getRows(), second.getColumns());
	for (int i = 0; i < first.getRows(); i++) {
		for (int j = 0; j < second.getColumns(); j++) {
			for (int z = 0; z < first.getColumns(); z++) {
				result[i][j] += first[i][z] * second[z][j];
			}
		}
	}

	return result;
}

template<typename T>
class Bracket{
private:
	Complex bra_complex;
	Complex ket_complex;
	Vector<T> bra;
	Vector<T> ket;

public:
	Bracket(Vector<T> bra, Vector<T> ket){
		this->bra = bra;
		this->ket = ket;
	}

	Bracket(Vector<T> bra, Vector<T> ket, Complex bra_complex, Complex ket_complex){
		this->bra = bra;
		this->ket = ket;
		this->bra_complex = bra_complex;
		this->ket_complex = ket_complex;
	}

	Vector<T> getBra(){
		return bra;
	}

	Vector<T> getKet(){
		return ket;
	}

	T& operator&(){
		return bra&ket;
	}

	Complex getKetComplex(){
		return ket_complex;
	}

	Complex getBraComplex(){
		return bra_complex;
	}

	friend std::ostream &operator<<(std::ostream &out, Bracket bracket) {
		out << "<a|b>\n";
		return out;
	}
};


template<typename T>
inline Bracket<T>& operator+(Bracket<T> first, Bracket<T> second){
	Vector<T> bra(first.getBra().getLength() + second.getBra().getLength());
	Vector<T> ket(first.getKet().getLength() + second.getKet().getLength());
	for (int i = 0; i < first.getBra().getLength(); i++)
	{
		bra[i] = first.getBra()[i];
	}
	for (int i = 0; i < second.getBra().getLength(); i++)
	{
		bra[i] = second.getBra()[i];
	}

	for (int i = 0; i < first.getKet().getLength(); i++)
	{
		ket[i] = first.getKet()[i];
	}
	for (int i = 0; i < second.getKet().getLength(); i++)
	{
		ket[i] = second.getKet()[i];
	}

	return Bracket<T>(bra, ket);
}



#endif