// RUN: %check_clang_tidy %s modernize-loop-convert %t -- -- -std=c++11

struct Str {
  Str() = default;
  Str(const Str &) = default;
  void constMember(int) const;
  void nonConstMember(int);
  bool operator<(const Str &str) const;     // const operator.
  Str &operator=(const Str &str) = default; // non const operator.
};

// This class is non-trivially copyable because the copy-constructor and copy
// assignment take non-const references.
struct ModifiesRightSide {
  ModifiesRightSide() = default;
  ModifiesRightSide(ModifiesRightSide &) = default;
  bool operator<(ModifiesRightSide &) const;
  ModifiesRightSide &operator=(ModifiesRightSide &) = default;
};

template <typename T>
void copyArg(T);

template <typename T>
void nonConstRefArg(T &);

// If we define this as a template, the type is deduced to "T&",
// and "const (T&) &" is the same as "T& &", and this collapses to "T&".
void constRefArg(const Str &);
void constRefArg(const ModifiesRightSide &);
void constRefArg(const int &);

void foo();

const int N = 10;
Str Array[N], OtherStr;
ModifiesRightSide Right[N], OtherRight;
int Ints[N], OtherInt;

void memberFunctionsAndOperators() {
  // Calling const member functions or operator is a const usage.
  for (int I = 0; I < N; ++I) {
    Array[I].constMember(0);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead [modernize-loop-convert]
  // CHECK-FIXES: for (auto Elem : Array)
  // CHECK-FIXES-NEXT: Elem.constMember(0);

  for (int I = 0; I < N; ++I) {
    if (Array[I] < OtherStr)
      foo();
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto Elem : Array)
  // CHECK-FIXES-NEXT: if (Elem < OtherStr)
  for (int I = 0; I < N; ++I) {
    if (Right[I] < OtherRight)
      foo();
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (const auto & Elem : Right)
  // CHECK-FIXES-NEXT: if (Elem < OtherRight)

  // Calling non-const member functions is not.
  for (int I = 0; I < N; ++I) {
    Array[I].nonConstMember(0);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Array)
  // CHECK-FIXES-NEXT: Elem.nonConstMember(0);

  for (int I = 0; I < N; ++I) {
    Array[I] = OtherStr;
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Array)
  // CHECK-FIXES-NEXT: Elem = OtherStr;

  for (int I = 0; I < N; ++I) {
    Right[I] = OtherRight;
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Right)
  // CHECK-FIXES-NEXT: Elem = OtherRight;
}

void usedAsParameterToFunctionOrOperator() {
  // Copying is OK, as long as the copy constructor takes a const-reference.
  for (int I = 0; I < N; ++I) {
    copyArg(Array[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto Elem : Array)
  // CHECK-FIXES-NEXT: copyArg(Elem);

  for (int I = 0; I < N; ++I) {
    copyArg(Right[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Right)
  // CHECK-FIXES-NEXT: copyArg(Elem);

  // Using as a const reference argument is allowed.
  for (int I = 0; I < N; ++I) {
    constRefArg(Array[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto Elem : Array)
  // CHECK-FIXES-NEXT: constRefArg(Elem);

  for (int I = 0; I < N; ++I) {
    if (OtherStr < Array[I])
      foo();
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto Elem : Array)
  // CHECK-FIXES-NEXT: if (OtherStr < Elem)

  for (int I = 0; I < N; ++I) {
    constRefArg(Right[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (const auto & Elem : Right)
  // CHECK-FIXES-NEXT: constRefArg(Elem);

  // Using as a non-const reference is not.
  for (int I = 0; I < N; ++I) {
    nonConstRefArg(Array[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Array)
  // CHECK-FIXES-NEXT: nonConstRefArg(Elem);
  for (int I = 0; I < N; ++I) {
    nonConstRefArg(Right[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Right)
  // CHECK-FIXES-NEXT: nonConstRefArg(Elem);
  for (int I = 0; I < N; ++I) {
    if (OtherRight < Right[I])
      foo();
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Right)
  // CHECK-FIXES-NEXT: if (OtherRight < Elem)
}

void primitiveTypes() {
  // As argument to a function.
  for (int I = 0; I < N; ++I) {
    copyArg(Ints[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: copyArg(Int);
  for (int I = 0; I < N; ++I) {
    constRefArg(Ints[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: constRefArg(Int);
  for (int I = 0; I < N; ++I) {
    nonConstRefArg(Ints[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int & Int : Ints)
  // CHECK-FIXES-NEXT: nonConstRefArg(Int);

  // Builtin operators.
  // Comparisons.
  for (int I = 0; I < N; ++I) {
    if (Ints[I] < N)
      foo();
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: if (Int < N)

  for (int I = 0; I < N; ++I) {
    if (N == Ints[I])
      foo();
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: if (N == Int)

  // Assignment.
  for (int I = 0; I < N; ++I) {
    Ints[I] = OtherInt;
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int & Int : Ints)
  // CHECK-FIXES-NEXT: Int = OtherInt;

  for (int I = 0; I < N; ++I) {
    OtherInt = Ints[I];
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: OtherInt = Int;

  for (int I = 0; I < N; ++I) {
    OtherInt = Ints[I] = OtherInt;
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int & Int : Ints)
  // CHECK-FIXES-NEXT: OtherInt = Int = OtherInt;

  // Arithmetic operations.
  for (int I = 0; I < N; ++I) {
    OtherInt += Ints[I];
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: OtherInt += Int;

  for (int I = 0; I < N; ++I) {
    Ints[I] += Ints[I];
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int & Int : Ints)
  // CHECK-FIXES-NEXT: Int += Int;

  for (int I = 0; I < N; ++I) {
    int Res = 5 * (Ints[I] + 1) - Ints[I];
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: int Res = 5 * (Int + 1) - Int;
}

void takingReferences() {
  // We do it twice to prevent the check from thinking that they are aliases.

  // Class type.
  for (int I = 0; I < N; ++I) {
    Str &J = Array[I];
    Str &K = Array[I];
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & Elem : Array)
  // CHECK-FIXES-NEXT: Str &J = Elem;
  // CHECK-FIXES-NEXT: Str &K = Elem;
  for (int I = 0; I < N; ++I) {
    const Str &J = Array[I];
    const Str &K = Array[I];
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto Elem : Array)
  // CHECK-FIXES-NEXT: const Str &J = Elem;
  // CHECK-FIXES-NEXT: const Str &K = Elem;

  // Primitive type.
  for (int I = 0; I < N; ++I) {
    int &J = Ints[I];
    int &K = Ints[I];
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int & Int : Ints)
  // CHECK-FIXES-NEXT: int &J = Int;
  // CHECK-FIXES-NEXT: int &K = Int;
  for (int I = 0; I < N; ++I) {
    const int &J = Ints[I];
    const int &K = Ints[I];
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int Int : Ints)
  // CHECK-FIXES-NEXT: const int &J = Int;
  // CHECK-FIXES-NEXT: const int &K = Int;

  // Aliases.
  for (int I = 0; I < N; ++I) {
    const Str &J = Array[I];
    (void) J;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto J : Array)
  for (int I = 0; I < N; ++I) {
    Str &J = Array[I];
    (void) J;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (auto & J : Array)

  for (int I = 0; I < N; ++I) {
    const int &J = Ints[I];
    (void) J;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int J : Ints)

  for (int I = 0; I < N; ++I) {
    int &J = Ints[I];
    (void) J;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop
  // CHECK-FIXES: for (int & J : Ints)
}
