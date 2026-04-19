# 第三章：封装与控制 -- 面向对象的核心原则

> 本章讲解 C++ 封装的核心机制：访问控制、getter/setter 模式、友元、静态成员和不可变类设计。

**源码位置**: `ch03_encapsulation/src/main.cpp`

---

## 3.1 封装实战：银行账户

封装的核心思想：**隐藏内部实现，暴露受控接口**。通过 `private` 保护数据，通过 `public` 方法提供安全的操作。

```cpp
class BankAccount {
private:
  string owner;
  double balance;
  static double interestRate;   // 静态成员: 所有对象共享一份
  static int totalAccounts;     // 静态成员: 类级别的计数器

public:
  BankAccount(const string &owner, double initial)
      : owner(owner), balance(initial) {
    ++totalAccounts;
  }

  ~BankAccount() {
    --totalAccounts;
  }

  // Getter: 只读访问
  double getBalance() const { return balance; }
  string getOwner() const { return owner; }

  // 业务方法（而非简单的 setter）-- 包含校验逻辑
  void deposit(double amount) {
    if (amount <= 0) {
      cout << "存款金额必须为正数" << endl;
      return;
    }
    balance += amount;
  }

  bool withdraw(double amount) {
    if (amount <= 0 || amount > balance) {
      cout << "余额不足或金额无效" << endl;
      return false;
    }
    balance -= amount;
    return true;
  }

  void applyInterest() {
    balance += balance * interestRate;
  }

  // 静态成员函数
  static void setInterestRate(double rate) { interestRate = rate; }
  static int getTotalAccounts() { return totalAccounts; }
};
```

### 访问控制三个级别

| 关键字 | 类内部 | 派生类 | 外部 |
|--------|--------|--------|------|
| `public` | 可访问 | 可访问 | 可访问 |
| `protected` | 可访问 | 可访问 | 不可访问 |
| `private` | 可访问 | 不可访问 | 不可访问 |

### 静态成员

- `static` 成员变量：属于**类级别**，所有对象共享同一份数据
- `static` 成员函数：只能访问 `static` 成员，不依赖对象实例
- 必须在**类外**定义和初始化：

```cpp
double BankAccount::interestRate = 0.01;   // 类外初始化
int BankAccount::totalAccounts = 0;
```

```cpp
BankAccount::setInterestRate(0.02);         // 通过类名调用
cout << BankAccount::getTotalAccounts();    // 不需要对象实例
```

### Getter/Setter vs 业务方法

**不推荐** -- 简单的 getter/setter 暴露了内部实现：

```cpp
void setBalance(double b) { balance = b; }   // 无校验，破坏封装
```

**推荐** -- 业务方法封装了校验逻辑：

```cpp
bool withdraw(double amount) {    // 包含金额校验和余额检查
  if (amount > balance) return false;
  balance -= amount;
  return true;
}
```

---

## 3.2 友元 (friend) -- 打破封装的特殊通道

友元机制允许特定的外部函数或类访问 `private` 成员。友元**不是**成员函数，它只是获得了访问权限。

### 友元函数

```cpp
class Matrix {
  double data[2][2];    // 私有数据

public:
  Matrix(double a, double b, double c, double d) { ... }

  // 声明友元函数: operator+ 可以访问 Matrix 的私有成员
  friend Matrix operator+(const Matrix &lhs, const Matrix &rhs);

  // 友元函数: 重载 << 用于输出
  friend ostream &operator<<(ostream &os, const Matrix &m);
};

// 友元函数实现（不是成员函数，没有 Matrix::）
Matrix operator+(const Matrix &lhs, const Matrix &rhs) {
  return Matrix(
    lhs.data[0][0] + rhs.data[0][0],   // 访问私有成员
    lhs.data[0][1] + rhs.data[0][1],
    lhs.data[1][0] + rhs.data[1][0],
    lhs.data[1][1] + rhs.data[1][1]);
}
```

### 友元类

整个类的所有方法都可以访问另一个类的私有成员：

```cpp
class Matrix {
  double data[2][2];
public:
  friend class MatrixPrinter;   // MatrixPrinter 的所有方法可访问 Matrix 的私有成员
};

class MatrixPrinter {
public:
  static void print(const Matrix &m) {
    cout << "[" << m.data[0][0] << ", " << m.data[0][1] << "]" << endl;   // 直接访问私有成员
    cout << "[" << m.data[1][0] << ", " << m.data[1][1] << "]" << endl;
  }
};
```

### 友元的注意事项

- 友元关系是**单向的**：A 是 B 的友元，不意味着 B 是 A 的友元
- 友元关系**不可继承**：派生类不会自动获得友元权限
- 友元不受 `public`/`private`/`protected` 位置限制，通常放在 `public` 区域
- **谨慎使用**：友元破坏了封装，只在确实必要时使用（如运算符重载 `<<`）

### 友元典型使用场景

1. 重载 `<<` 和 `>>` 运算符（左操作数是流，不能写成成员函数）
2. 两个类紧密耦合需要互相访问内部状态
3. 工厂类需要访问私有构造函数

---

## 3.3 不可变类设计 (Immutable Class)

不可变类的所有操作都**返回新对象**，自身永远不被修改。这在并发编程和函数式风格中非常有用。

```cpp
class Complex {
private:
  double real;
  double imag;

public:
  Complex(double r = 0, double i = 0) : real(r), imag(i) {}

  // 所有运算返回新对象，不修改自身
  Complex operator+(const Complex &other) const {
    return Complex(real + other.real, imag + other.imag);
  }

  Complex operator*(const Complex &other) const {
    return Complex(
      real * other.real - imag * other.imag,
      real * other.imag + imag * other.real);
  }

  double magnitude() const {
    return sqrt(real * real + imag * imag);
  }

  friend ostream &operator<<(ostream &os, const Complex &c) {
    os << c.real << (c.imag >= 0 ? " + " : " - ") << abs(c.imag) << "i";
    return os;
  }
};
```

```cpp
Complex c1(1, 2);       // 1 + 2i
Complex c2(3, 4);       // 3 + 4i
Complex c3 = c1 + c2;   // 返回新对象 4 + 6i，c1 不变
Complex c4 = c1 * c2;   // 返回新对象 -5 + 10i，c1 不变
```

### 设计不可变类的原则

1. 所有成员变量为 `private`
2. 不提供 setter 方法
3. 所有运算方法标记为 `const` 并返回新对象
4. 构造函数初始化所有成员

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 访问控制 | `public` 对外、`private` 内部、`protected` 继承可见 |
| 封装 | 隐藏实现，暴露业务方法（而非裸 setter） |
| 静态成员 | 类级别共享，类外初始化，静态函数只访问静态成员 |
| 友元函数 | 可以访问私有成员的非成员函数 |
| 友元类 | 整个类获得访问权限，单向、不可继承 |
| 不可变类 | 所有操作返回新对象，天然线程安全 |
