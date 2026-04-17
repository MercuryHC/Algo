/**
 * 第三章：封装与控制 -- 面向对象的核心原则
 *
 * 本章要点：
 * 1: public / private / protected 控制访问
 * 2: getter/setter 模式
 * 3: 友元 (friend)
 * 4: 静态成员 (static)
 * 5: 不可变类设计
 */

#include <cmath>
#include <iostream>
#include <string>

using namespace std;

// ============================================================
// 3.1. 封装实战: 银行账户
// ============================================================
class BankAccout {
private:
  string owner;
  double balance;
  static double interestRate; // 静态成员: 所有对象共享
  static int totalAccounts;   // 静态成员: 账户总数
public:
  BankAccout(const string &owner, double initial)
      : owner(owner), balance(initial) {
    ++totalAccounts;
    cout << " 开户: " << owner << " 余额: " << balance << endl;
  }

  ~BankAccout() {
    --totalAccounts;
    cout << " 销户: " << owner << endl;
  }

  // Getter: 只读访问 const
  double getBalance() const { return balance; }
  string getOwner() const { return owner; }

  // 业务方法 (而非简单的setter)
  void deposit(double amount) {
    if (amount <= 0) {
      cout << " 存款金额必须为正数" << endl;
    }
    balance += amount;
    cout << " " << owner << " 存入 " << amount << ", 金额: " << balance << endl;
  }

  bool withDraw(double amount) {
    if (amount <= 0) {
      cout << " 存款金额必须为正数" << endl;
      return false;
    }
    if (amount > balance) {
      cout << " " << owner << " 余额不足!" << endl;
    }
    balance -= amount;
    cout << " " << owner << " 取出 " << amount << ", 余额: " << balance << endl;
    return true;
  }

  void applyInterest() {
    double interest = balance * interestRate;
    balance += interest;
    cout << " " << owner << " 利息: " << interest << ", 余额: " << balance
         << endl;
  }

  // 静态成员函数: 只能访问静态成员
  static void setInterestRate(double rate) {
    interestRate = rate;
    cout << " 利率设置为: " << interestRate << endl;
  }
  static int getTotalAccounts() { return totalAccounts; }
};

// 静态成员必须在类外定义和初始化
double BankAccout::interestRate = 0.01; // 默认利率 1%
int BankAccout::totalAccounts = 0;

// ============================================================
// 3.2. 友元: 打破封装的特殊通道
// ============================================================
class Matrix {
  double data[2][2];

public:
  Matrix(double a, double b, double c, double d) {
    data[0][0] = a;
    data[0][1] = b;
    data[1][0] = c;
    data[1][1] = d;
  }

  // 友元函数: 可以访问 Matrix 的私有成员, 但不是成员函数
  friend Matrix operator+(const Matrix &lhs, const Matrix &rhs);

  // 友元类: 可以访问 Matrix 的私有成员
  friend class MatrixPrinter;

  // 友元函数: 用于输出
  friend ostream &operator<<(ostream &os, const Matrix &m);

  // 友元函数实现: 矩阵加法
  friend Matrix operator+(const Matrix &lhs, const Matrix &rhs) {
    return Matrix(
        lhs.data[0][0] + rhs.data[0][0], lhs.data[0][1] + rhs.data[0][1],
        lhs.data[1][0] + rhs.data[1][0], lhs.data[1][1] + rhs.data[1][1]);
  }
};

// 友元类
class MatrixPrinter {
public:
  static void print(const Matrix &m) {
    cout << " 矩阵详情: " << endl;
    cout << " [" << m.data[0][0] << ", " << m.data[0][1] << "]" << endl;
    cout << " [" << m.data[1][0] << ", " << m.data[1][1] << "]" << endl;
    double det = m.data[0][0] * m.data[1][1] - m.data[0][1] * m.data[1][0];
    cout << " 行列式: " << det << endl;
  }

  friend ostream &operator<<(ostream &os, const Matrix &m) {
    os << " [" << m.data[0][0] << ", " << m.data[0][1] << "]" << endl;
    os << " [" << m.data[1][0] << ", " << m.data[1][1] << "]";
    return os;
  }
};

// ============================================================
// 3.3. 不可变类设计 (Immutable Class)
// ============================================================
class Complex {
private:
  double real;
  double imag;

public:
  Complex(double r = 0, double i = 0) : real(r), imag(i) {}

  // 所有操作返回新对象，不修改自身
  // 加法 complex + complex
  Complex operator+(const Complex &other) const {
    return Complex(real + other.real, imag + other.imag);
  }

  // 乘法 complex * complex
  Complex operator*(const Complex &other) const {
    return Complex(real * other.real - imag * other.imag,
                   real * other.imag + imag * other.real);
  }

  // 计算模长
  double magnitude() const { return sqrt(real * real + imag * imag); }

  friend ostream &operator<<(ostream &os, const Complex &c) {
    os << c.real << (c.imag >= 0 ? " + " : " - ") << abs(c.imag) << "i";
    return os;
  }
};

// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 3.1 封装: 银行账户 =============" << endl;
  BankAccout::setInterestRate(0.02); // 设置利率

  BankAccout a1("张三", 1000);
  BankAccout a2("李四", 2000);
  cout << " 当前账户总数: " << BankAccout::getTotalAccounts() << endl;

  a1.deposit(500);
  a1.withDraw(300);
  a1.withDraw(1500); // 余额不足
  a1.applyInterest();
  cout << endl;

  cout << "=========== 3.2 友元: 矩阵运算 =============" << endl;
  Matrix m1(1, 2, 3, 4);
  Matrix m2(5, 6, 7, 8);
  cout << " m1: " << m1 << endl;
  cout << " m2: " << m2 << endl;
  Matrix m3 = m1 + m2;
  cout << " m1 + m2: " << m3 << endl;
  MatrixPrinter::print(m3);
  cout << endl;

  cout << "=========== 3.3 不可变类: 复数 =============" << endl;
  Complex c1(1, 2);
  Complex c2(3, 4);
  cout << " c1: " << c1 << endl;
  cout << " c2: " << c2 << endl;
  Complex c3 = c1 + c2;
  Complex c4 = c1 * c2;
  cout << " c1 + c2: " << c3 << endl;
  cout << " c1 * c2: " << c4 << endl;

  return 0;
}
