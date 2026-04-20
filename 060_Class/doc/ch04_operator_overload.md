# 第四章：运算符重载 -- 让类像内置类型一样使用

> 本章讲解如何通过运算符重载，让自定义类型支持自然的算术、比较、流输出、下标访问等操作。

**源码位置**: `ch04_operator_overload/src/main.cpp`

---

## 4.1 为什么要重载运算符

内置类型可以直接用 `+`、`==`、`[]` 等运算符，自定义类型默认不行：

```cpp
Vec2 a(1, 2), b(3, 4);
// Vec2 c = a + b;   // 编译错误！

// 重载后：
Vec2 c = a + b;      // ✅ 自然直观
// 等价于 Vec2 c = a.operator+(b);
```

运算符重载让自定义类型的使用体验接近内置类型。

## 4.1 算术运算符与比较运算符：二维向量 Vec2

通过重载 `+`、`-`、`*`、`==`、`!=`、`<` 等运算符，让 `Vec2` 像内置数值类型一样运算。

```cpp
class Vec2 {
public:
  double x, y;
  Vec2(double x = 0, double y = 0) : x(x), y(y) {}

  // 算术运算符（成员函数形式）
  Vec2 operator+(const Vec2 &rhs) { return Vec2(x + rhs.x, y + rhs.y); }
  Vec2 operator-(const Vec2 &rhs) { return Vec2(x - rhs.x, y - rhs.y); }
  Vec2 operator*(double scale)    { return Vec2(x * scale, y * scale); }

  // 比较运算符
  bool operator==(const Vec2 &rhs) const { return x == rhs.x && y == rhs.y; }
  bool operator!=(const Vec2 &rhs) const { return !(*this == rhs); }
  bool operator<(const Vec2 &rhs) const  { return length() < rhs.length(); }

  // 流输出运算符（必须是友元，因为左操作数是 ostream）
  friend ostream &operator<<(ostream &os, const Vec2 &v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
  }
};
```

**使用**:

```cpp
Vec2 v1(3, 4), v2(1, 2);
Vec2 sum = v1 + v2;       // (4, 6)
Vec2 diff = v1 - v2;      // (2, 2)
Vec2 scaled = v1 * 2;     // (6, 8)
bool eq = (v1 == v2);     // false
bool lt = (v1 < v2);      // false (5.0 > 2.24)
cout << v1;               // (3, 4)
```

### 成员函数 vs 非成员函数

| 形式 | 适用场景 | 左操作数 |
|------|---------|---------|
| 成员函数 | `+`、`-`、`*`、`==`、`<` 等 | 必须是类本身 |
| 非成员友元 | `<<`、`>>` | 可以是其他类型（如 `ostream`） |

---

## 4.2 下标运算符 []：动态数组 IntArray

下标运算符通常需要提供 `const` 和非 `const` 两个版本，分别用于只读和可修改访问。

```cpp
class IntArray {
  int *data;
  size_t len;

public:
  explicit IntArray(size_t size) : len(size) {
    data = new int[size]();   // 值初始化（归零）
  }
  ~IntArray() { delete[] data; }

  // 非 const 版本: 返回引用，允许修改
  int &operator[](size_t index) {
    if (index >= len) throw out_of_range("索引越界");
    return data[index];
  }

  // const 版本: 只读访问，返回值
  int operator[](size_t index) const {
    if (index >= len) throw out_of_range("索引越界");
    return data[index];
  }

  size_t size() const { return len; }
};
```

**使用**:

```cpp
IntArray arr(5);
arr[0] = 10;          // 调用非 const 版本，修改元素
arr[1] = 20;

const IntArray &carr = arr;
int val = carr[1];    // 调用 const 版本，只读
// carr[0] = 99;      // 错误! const 版本返回值，不能赋值
```

### explicit 关键字

```cpp
IntArray arr(5);    // OK: 直接初始化
// IntArray arr = 5; // 错误! explicit 阻止隐式转换
```

> `explicit` 防止构造函数的隐式类型转换，避免意外的类型转换bug。

---

## 4.3 自增/自减运算符

前置和后置通过参数列表中的 `int` 占位符区分：

```cpp
class Iterator {
  int *ptr;
public:
  Iterator(int *p = nullptr) : ptr(p) {}

  // 前置 ++: 先自增，返回引用（修改后的自身）
  Iterator &operator++() {
    ++ptr;
    return *this;
  }

  // 后置 ++: 返回旧值，int 为占位参数（仅用于区分，无实际意义）
  Iterator operator++(int) {
    Iterator tmp = *this;   // 保存旧值
    ++ptr;                  // 自增
    return tmp;             // 返回旧值
  }

  // 解引用
  int &operator*() const { return *ptr; }

  // 比较运算符
  bool operator==(const Iterator &rhs) const { return ptr == rhs.ptr; }
  bool operator!=(const Iterator &rhs) const { return ptr != rhs.ptr; }
};
```

**使用**:

```cpp
int nums[] = {10, 20, 30, 40, 50};
Iterator it(nums);

*it;          // 10
*++it;        // 20 (前置: 先加再用)
*it++;        // 20 (后置: 先用再加)，之后 *it = 30
```

### 前置 vs 后置对比

| 特性 | 前置 `++i` | 后置 `i++` |
|------|-----------|-----------|
| 返回值 | 引用（`T&`） | 临时对象（`T`） |
| 效率 | 更高（无拷贝） | 略低（需要拷贝旧值） |
| 签名 | `T& operator++()` | `T operator++(int)` |
| 语义 | 先加再用 | 先用再加 |

> **最佳实践**: 优先使用前置 `++`，除非需要后置语义。

---

## 4.4 函数调用运算符 () 与类型转换

### 函数调用运算符（仿函数）

重载 `()` 让对象像函数一样被调用：

```cpp
class Matrix2x2 {
  double m[2][2];
public:
  // 像函数一样访问矩阵元素: mat(row, col)
  double &operator()(int row, int col) {
    if (row < 0 || row > 1 || col < 0 || col > 1)
      throw out_of_range("矩阵索引越界");
    return m[row][col];
  }

  // const 版本
  double operator()(int row, int col) const {
    if (row < 0 || row > 1 || col < 0 || col > 1)
      throw out_of_range("矩阵索引越界");
    return m[row][col];
  }

  // 矩阵乘法
  Matrix2x2 operator*(const Matrix2x2 &rhs) const {
    Matrix2x2 result;
    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 2; ++j) {
        result(i, j) = 0;
        for (int k = 0; k < 2; ++k)
          result(i, j) += m[i][k] * rhs(k, j);
      }
    return result;
  }
};
```

**使用**:

```cpp
Matrix2x2 mat(1, 2, 3, 4);
mat(0, 1);            // 访问第0行第1列元素 → 2
mat(0, 0) = 99;       // 修改元素
Matrix2x2 product = mat1 * mat2;
```

### 显式类型转换运算符

```cpp
class Matrix2x2 {
  // ...
public:
  explicit operator double() const {
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];   // 返回行列式
  }
};
```

```cpp
Matrix2x2 mat(1, 2, 3, 4);
double det = static_cast<double>(mat);   // 显式转换 → -2
// double d = mat;  // 错误! explicit 阻止隐式转换
```

> **推荐使用 `explicit`**: 防止意外的隐式类型转换。

---

## 运算符重载速查表

| 运算符 | 推荐形式 | 可否成员函数 | 备注 |
|--------|---------|-------------|------|
| `+`, `-`, `*`, `/` | 成员或非成员 | 可以 | 返回新对象 |
| `==`, `!=`, `<`, `>` | 成员函数 | 可以 | 应标记 `const` |
| `<<`, `>>` | 非成员友元 | 不可以 | 左操作数是流 |
| `[]` | 成员函数 | 必须 | 提供 const/非 const 两版 |
| `++`, `--` | 成员函数 | 必须 | 前置返回引用，后置返回值 |
| `()` | 成员函数 | 必须 | 仿函数，可接受任意参数 |
| `->`, `*` | 成员函数 | 必须 | 智能指针常用 |
| 类型转换 | 成员函数 | 必须 | 建议 `explicit` |
| `=` | 成员函数 | 必须 | 拷贝/移动赋值 |

### 不可重载的运算符

- `.` (成员访问)
- `.*` (成员指针访问)
- `::` (作用域解析)
- `?:` (三目运算符)
- `sizeof`、`typeid`

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 算术运算符 | 成员函数形式，返回新对象 |
| 比较运算符 | 返回 `bool`，标记 `const` |
| 流运算符 | 必须是非成员友元，返回流引用 |
| 下标运算符 | 提供 const/非 const 两版，含越界检查 |
| 自增/自减 | 前置返回引用，后置用 `int` 占位返回旧值 |
| 函数调用 `()` | 仿函数，使对象像函数一样使用 |
| 类型转换 | 用 `explicit` 防止隐式转换 |
| `explicit` | 防止单参构造和类型转换的隐式调用 |
