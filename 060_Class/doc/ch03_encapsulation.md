# 第三章：封装与访问控制 —— 面向对象的核心原则

> 对应代码：`ch03_encapsulation.cpp`

---

## 3.1 访问控制

C++ 提供三种访问修饰符和一种特殊机制：

```
┌─────────────────────────────────────────────┐
│                  类内部                       │
│  ┌───────┐  ┌───────────┐  ┌─────────────┐ │
│  │public │  │ protected │  │  private    │ │
│  │       │  │           │  │             │ │
│  │外部+   │  │ 自己+      │  │ 仅自己      │ │
│  │派生类  │  │ 派生类     │  │             │ │
│  └───────┘  └───────────┘  └─────────────┘ │
│                                              │
│  friend（特许通道）→ 可以访问 private         │
└─────────────────────────────────────────────┘
```

### 访问级别对比

| 修饰符 | 类内部 | 派生类 | 外部 |
|--------|--------|--------|------|
| `public` | ✅ | ✅ | ✅ |
| `protected` | ✅ | ✅ | ❌ |
| `private` | ✅ | ❌ | ❌ |
| `friend` | ✅ | ❌ | ✅（特许） |

## 3.2 封装实战：银行账户

### 为什么不用裸的 getter/setter

```cpp
// 不好的设计：直接暴露数据
class BadAccount {
public:
    string owner;
    double balance;  // 外部可以随意修改！
};

BadAccount acc;
acc.balance = -1000000;  // 灾难！
```

```cpp
// 好的设计：通过业务方法操作
class BankAccount {
private:
    double balance;

public:
    void deposit(double amount) {      // 有校验逻辑
        if (amount <= 0) return;
        balance += amount;
    }

    bool withdraw(double amount) {     // 有业务规则
        if (amount > balance) return false;
        balance -= amount;
        return true;
    }

    double get_balance() const { return balance; }  // 只读访问
};
```

### 设计原则

- **不要暴露裸数据**：通过方法操作，可以加入校验、日志、通知等
- **最小权限**：能 private 就 private，能 protected 就 protected
- **业务方法优于 setter**：`withdraw()` 比 `set_balance()` 更能体现意图

## 3.3 静态成员

### 静态成员变量

所有对象**共享同一份**，不属于任何单个对象：

```cpp
class BankAccount {
    static double interest_rate;   // 声明
    static int total_accounts;
};

// 类外初始化（必须！）
double BankAccount::interest_rate = 0.02;
int BankAccount::total_accounts = 0;
```

```
acc1 ──→ ┌──────────────┐
acc2 ──→ │ interest_rate │ ← 所有账户共享
acc3 ──→ │ total_accounts│
         └──────────────┘
```

### 静态成员函数

没有 `this` 指针，只能访问静态成员：

```cpp
static void set_interest_rate(double rate) {
    interest_rate = rate;   // ✅ 可以访问静态成员
    // owner = "xxx";      // ❌ 不能访问非静态成员
}

// 调用方式
BankAccount::set_interest_rate(0.03);  // 通过类名调用
```

### 静态成员的用途

| 用途 | 示例 |
|------|------|
| 计数器 | `static int total_accounts;` |
| 共享配置 | `static double interest_rate;` |
| 工厂方法 | `static BankAccount create(...)` |
| 工具函数 | `static double calc_interest(...)` |

## 3.4 友元（friend）

友元打破了封装的壁垒，让特定的函数或类可以访问 private 成员。

### 友元函数

```cpp
class Matrix {
    double data[2][2];

public:
    // 声明友元
    friend Matrix operator+(const Matrix& lhs, const Matrix& rhs);
    friend ostream& operator<<(ostream& os, const Matrix& m);
};

// 实现：可以直接访问 data
Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
    return Matrix(
        lhs.data[0][0] + rhs.data[0][0],
        // ... 直接访问 private 成员
    );
}
```

### 友元类

```cpp
class Matrix {
    friend class MatrixPrinter;  // 整个 MatrixPrinter 类都是友元
};

class MatrixPrinter {
public:
    static void print_detailed(const Matrix& m) {
        // 可以直接访问 m.data
        cout << m.data[0][0];  // ✅
    }
};
```

### 何时使用友元

| 场景 | 解决方案 |
|------|---------|
| `operator<<` 需要访问私有数据 | 友元函数 |
| 两个紧密关联的类需要互相访问 | 友元类 |
| 需要给特定函数开后门 | 友元函数 |

**注意**：友元关系不能被继承，不能被传递。

## 3.5 不可变类设计

不可变对象一旦创建就不能修改，所有操作都返回新对象：

```cpp
class Complex {
    double real, imag;

public:
    Complex(double r, double i) : real(r), imag(i) {}

    // 操作返回新对象，不修改自身
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }

    Complex operator*(const Complex& other) const {
        return Complex(
            real * other.real - imag * other.imag,
            real * other.imag + imag * other.real
        );
    }
};
```

**不可变类的优势**：
1. 线程安全（无共享可变状态）
2. 可以安全地缓存和复用
3. 易于推理和测试
4. 适合作为 map/set 的键

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 访问控制 | `public` 对外、`private` 内部、`protected` 继承可见 |
| 封装 | 隐藏实现，暴露业务方法（而非裸 setter） |
| 静态成员 | 类级别共享，类外初始化，静态函数只访问静态成员 |
| 友元函数 | 可以访问私有成员的非成员函数 |
| 友元类 | 整个类获得访问权限，单向、不可继承 |
| 不可变类 | 所有操作返回新对象，天然线程安全 |
