# 第一章：类的基础 -- 从 struct 到 class

> 本章介绍 C++ 中类的基本概念，包括 struct 与 class 的区别、成员变量与成员函数、this 指针、const 成员函数等。

**源码位置**: `ch01_basic_class/src/main.cpp`

---

## 1.1 什么是类

类（class）是 C++ 面向对象编程的核心概念。它将**数据**和**操作数据的函数**打包在一起，形成一个自定义类型。

```txt
┌─────────────────────────────┐
│           Class             │
│  ┌───────────────────────┐  │
│  │   成员变量（数据）    │  │
│  │   - name              │  │
│  │   - age               │  │
│  │   - score             │  │
│  ├───────────────────────┤  │
│  │   成员函数（行为）    │  │
│  │   + set_info()        │  │
│  │   + print_info()      │  │
│  │   + is_passed()       │  │
│  └───────────────────────┘  │
└─────────────────────────────┘
```

## 1.2 struct 定义点 -- 最简单的类

在 C++ 中，`struct` 和 `class` 本质上都能定义类。`struct` 默认成员访问权限为 `public`，适合定义简单的数据聚合体。

```cpp
struct Point {
  double x;
  double y;

  // 成员函数: 计算到原点的距离
  double distanceFromOrigin() const {
    return sqrt(x * x + y * y);
  }

  // 成员函数: 打印自身
  void print() const {
    cout << "(" << x << ", " << y << ")" << endl;
  }
};
```

**使用方式**:

```cpp
Point p1 = {3.0, 4.0};     // 聚合初始化
p1.print();                 // 输出: (3, 4)
p1.distanceFromOrigin();    // 输出: 5
```

**要点**:
- `struct` 可以包含成员变量和成员函数
- 默认所有成员为 `public`，外部可直接访问
- `const` 修饰的成员函数承诺不会修改对象的成员变量

---

## 1.3 class 定义学生类

`class` 默认成员访问权限为 `private`，是实现**封装**的关键。通过将数据设为私有、方法设为公有，控制外部对内部状态的访问。

```cpp
class Student {
private:              // 私有成员: 外部不可直接访问
  string name;
  int age;
  double score;

public:               // 公有成员: 外部可以访问
  void setInfo(const string &n, int a, double s) {
    name = n;
    age = a;
    score = s;
  }

  void printInfo() const {
    cout << "姓名: " << this->name
         << ", 年龄: " << this->age
         << ", 成绩: " << this->score << endl;
  }

  bool isPassed() const {
    return score >= 60.0;
  }
};
```

**使用方式**:

```cpp
Student s1;
s1.setInfo("张三", 20, 85.5);   // 通过公有方法设置私有数据
s1.printInfo();                   // 输出: 姓名: 张三, 年龄: 20, 成绩: 85.5
// s1.name = "李四";             // 错误! name 是 private，外部不可访问
```

### struct 与 class 的唯一区别

| 特性 | struct | class |
|------|--------|-------|
| 默认成员访问权限 | `public` | `private` |
| 默认继承方式 | `public` | `private` |
| 其他功能 | 完全相同 | 完全相同 |

> **惯例**: 用 `struct` 定义纯数据聚合体（POD），用 `class` 定义需要封装的类型。

---

## 1.4 this 指针与链式调用

`this` 是 C++ 中的隐式指针，在每个非静态成员函数中自动可用，指向调用该函数的对象本身。

### this 的两大用途

**1. 区分成员变量与同名参数**

当构造函数参数名与成员变量名相同时，用 `this->` 区分：

```cpp
class Counter {
  int value;
public:
  Counter(int value) {
    this->value = value;   // this->value 是成员变量，value 是参数
  }
};
```

**2. 返回 `*this` 实现链式调用**

成员函数返回 `*this` 的引用，使得多个方法调用可以连续进行：

```cpp
class Counter {
  int value;
public:
  Counter(int value) : value(value) {}

  Counter &increment() {
    ++this->value;
    return *this;           // 返回自身引用
  }

  Counter &add(int n) {
    this->value += n;
    return *this;           // 返回自身引用
  }

  int get() const { return value; }
};
```

**链式调用**:

```cpp
Counter c(0);
c.increment().increment().add(10);
// 等价于:
// c.increment();  返回 c 的引用
// .increment();   在 c 上再次调用，返回 c 的引用
// .add(10);       在 c 上再次调用
cout << c.get();   // 输出: 12
```

> **应用场景**: `std::cout` 的连续 `<<` 操作、Builder 模式等都利用了链式调用。

---

## 1.5 const 成员函数

在成员函数声明的末尾加 `const` 关键字，表示该函数**不会修改**对象的任何成员变量。

```cpp
class Circle {
  double radius;
public:
  Circle(double r) : radius(r) {}

  // const 成员函数: 承诺不修改成员变量
  double area() const {
    return 3.14159265 * radius * radius;
  }

  double circumference() const {
    return 2 * 3.14159265 * radius;
  }

  // 非 const 函数: 可以修改成员变量
  void setRadius(double r) {
    radius = r;
  }
};
```

### const 规则

```cpp
const Circle c1(5.0);       // const 对象
c1.area();                   // OK: const 对象可以调用 const 成员函数
// c1.setRadius(10);         // 错误! const 对象不能调用非 const 函数

Circle c2(3.0);             // 非 const 对象
c2.area();                   // OK: 非 const 对象可以调用 const 成员函数
c2.setRadius(10.0);          // OK: 非 const 对象可以调用非 const 函数
```

**核心规则**:

| 对象类型 | 可调用的成员函数 |
|---------|----------------|
| `const` 对象 | 只能调用 `const` 成员函数 |
| 非 `const` 对象 | 可以调用所有成员函数 |

> **最佳实践**: 不修改成员变量的函数都应声明为 `const`，这样 const 对象也能使用。

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| `struct` | 默认 public，适合数据聚合 |
| `class` | 默认 private，适合封装 |
| `this` 指针 | 指向当前对象，用于区分同名变量和链式调用 |
| `const` 成员函数 | 不修改对象状态，const 对象只能调用此类函数 |
| 访问控制 | `public`（对外）、`private`（内部）、`protected`（继承可见） |
