#include <iostream>

using namespace std;

char TOKEN;
string::iterator ITER;

struct Matrix {
  uint64_t rank;
  uint64_t file;
  uint64_t cost;
};

void Match(char token) {
  if (TOKEN not_eq '#' and TOKEN == token) TOKEN = *++ITER;
  else throw "No";
}

Matrix T();

Matrix E() {
  Matrix leftMatrix { T() };
  while (TOKEN == 'A' || TOKEN == 'B' || TOKEN == 'C' || TOKEN == '(') {
    Matrix rightMatrix { T() };
    leftMatrix.file = rightMatrix.file;
    leftMatrix.cost += leftMatrix.rank * rightMatrix.file * rightMatrix.rank;
  }
  return leftMatrix;
}

Matrix T() {
  Matrix matrix;
  switch(TOKEN) {
  case 'A': matrix = {50, 20, 0}; break;
  case 'B': matrix = {20, 5, 0}; break;
  case 'C': matrix = {5, 30, 0}; break;
  case '(':
    Match('(');
    matrix = E();
    break;
  default: throw "No";
  }
  Match(TOKEN);
  return matrix;
}

int main()
{
  string exp { "AB)#" };
  ITER = begin(exp);
  TOKEN = *ITER;
  Matrix result;
  try {
    result = E();
    if (TOKEN not_eq '#') throw "No";
    cout << result.cost;
  } catch (...) {
    cout << "No";
  }
  return 0;
}
