#include <bits/stdc++.h>

using namespace std;

char TOKEN;
string::iterator ITER;

using Term = vector<pair<string, char>>;

void Match(char token) {
  if (TOKEN == token) TOKEN = *++ITER;
  else throw "No";
}

Term AddTerm();
Term MulTerm();

Term Expression() {
  Term leftTerm { AddTerm() };
  while (TOKEN == '+' or TOKEN == '-') {
    char token { TOKEN };
    Match(TOKEN);
    Term rightTerm { AddTerm() };
    if (token == '+') copy(begin(rightTerm), end(rightTerm), back_inserter(leftTerm));
    else {
      for (const auto &[factor, op] : rightTerm) {
        if (op == '-') leftTerm.emplace_back(factor, '+');
        else leftTerm.emplace_back(factor, '-');
      }
    }
  }
  return leftTerm;
}

Term AddTerm() {
  Term leftTerm { MulTerm() };
  while (TOKEN == '*' or TOKEN == '/') {
    char token { TOKEN };
    Match(TOKEN);
    Term rightTerm { MulTerm() };
    Term newTerm;
    for (const auto &[factor1, op1] : leftTerm) {
      for (const auto &[factor2, op2] : rightTerm) {
        if (op1 == '-') {
          if (op2 == '+') newTerm.emplace_back(factor1, '-');
          else newTerm.emplace_back(factor1, '+');
        } else newTerm.emplace_back(factor1, op2);
        newTerm.back().first += token;
        newTerm.back().first += factor2;
      }
    }
    leftTerm = newTerm;
  }
  return leftTerm;
}

Term MulTerm() {
  Term term;
  if (TOKEN == '(') {
    Match('(');
    term = Expression();
  } else if (TOKEN == '-') {
    Match('-');
    term.assign({ {string { TOKEN }, '-'} });
  } else if (isalpha(TOKEN)) term.assign({ {string { TOKEN }, '+'} });
  else throw "No";
  Match(TOKEN);
  return term;
}

int main() {
  string exp { "(-a+b*a)*(a-b*c)#" };
  ITER = begin(exp);
  TOKEN = *ITER;
  Term result;
  try {
    result = Expression();
    if (TOKEN not_eq '#') throw "No";
  } catch (...) {
    cout << "No";
  }
  exp = "";
  for (const auto &[factor, op] : result) {
    exp += op;
    exp += factor;
  }
  if (exp.begin()[0] == '-') cout << exp;
  else cout << exp.substr(1);
  return 0;
}
