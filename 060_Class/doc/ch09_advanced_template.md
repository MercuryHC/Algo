# 第九章：模板元编程 -- 编译期计算

> 本章探讨 C++ 模板元编程技术，利用模板在编译期进行计算、类型检查和代码生成。

**源码位置**: `ch09_advanced_template/src/main.cpp`

> **注意**: 本章框架已搭建，以下内容为规划要点和知识讲解，源码待完善。

---

## 9.1 编译期计算

模板元编程的核心能力之一是让计算在**编译期**完成，运行时直接使用结果。

### 经典示例：编译期阶乘

```cpp
// 递归模板: 一般情况
template <int N>
struct Factorial {
  static constexpr int value = N * Factorial<N - 1>::value;
};

// 特化: 递归终止条件
template <>
struct Factorial<0> {
  static constexpr int value = 1;
};

// 使用
cout << Factorial<5>::value;    // 120，编译期已计算完成
```

### 编译期斐波那契数列

```cpp
template <int N>
struct Fibonacci {
  static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template <>
struct Fibonacci<0> { static constexpr int value = 0; };

template <>
struct Fibonacci<1> { static constexpr int value = 1; };

cout << Fibonacci<10>::value;   // 55
```

> **原理**: 模板实例化是编译期过程，编译器会递归实例化模板直到匹配特化终止条件，最终结果是一个编译期常量。

---

## 9.2 类型萃取 (Type Traits)

C++11 `<type_traits>` 提供了丰富的编译期类型查询和转换工具。

### 常用类型查询

```cpp
#include <type_traits>

is_integral<int>::value           // true
is_pointer<int*>::value           // true
is_const<const int>::value        // true
is_same<int, double>::value       // false
is_base_of<Base, Derived>::value  // true
```

### 条件类型选择

```cpp
// 根据条件选择类型
conditional<true, int, double>::type    // int
conditional<false, int, double>::type   // double

// 移除/添加修饰
remove_const<const int>::type           // int
remove_pointer<int*>::type              // int
add_const<int>::type                    // const int
```

### 自定义 type trait

```cpp
template <typename T>
struct is_string : false_type {};

template <>
struct is_string<string> : true_type {};

template <>
struct is_string<const string> : true_type {};

static_assert(is_string<string>::value, "应该是 string");
static_assert(!is_string<int>::value, "不应该是 string");
```

---

## 9.3 SFINAE (Substitution Failure Is Not An Error)

当模板参数替换失败时，编译器不会报错，而是从重载候选集中移除该模板。

### enable_if 基本用法

```cpp
#include <type_traits>

// 只对整数类型启用
template <typename T>
typename enable_if<is_integral<T>::value, T>::type
process(T value) {
  cout << "处理整数: " << value << endl;
  return value;
}

// 只对浮点类型启用
template <typename T>
typename enable_if<is_floating_point<T>::value, T>::type
process(T value) {
  cout << "处理浮点: " << value << endl;
  return value;
}

process(42);      // 调用整数版本
process(3.14);    // 调用浮点版本
// process("hi");  // 编译错误: 没有匹配的重载
```

### SFINAE 的典型应用

- 根据类型特性选择不同的函数实现
- 为特定类型提供优化的重载版本
- 模板类的条件性成员函数

---

## 9.4 if constexpr (C++17)

C++17 引入 `if constexpr`，在编译期进行条件分支，比 SFINAE 更直观：

```cpp
template <typename T>
auto process(T value) {
  if constexpr (is_integral_v<T>) {
    cout << "整数: " << value << endl;
    return value + 1;
  } else if constexpr (is_floating_point_v<T>) {
    cout << "浮点: " << value << endl;
    return value + 0.5;
  } else {
    cout << "其他类型" << endl;
    return value;
  }
}
```

**与 SFINAE 的区别**:
- `if constexpr`: 编译期条件，未选中的分支**不会被编译**
- SFINAE: 通过替换失败移除候选，更底层但更难理解
- 优先使用 `if constexpr`，复杂场景才用 SFINAE

---

## 9.5 概念与约束 (Concepts, C++20)

C++20 Concepts 提供了对模板参数的**命名约束**，比 SFINAE 更清晰：

```cpp
#include <concepts>

// 定义概念
template <typename T>
concept Numeric = integral<T> || floating_point<T>;

// 使用概念约束模板参数
template <Numeric T>
T add(T a, T b) {
  return a + b;
}

// requires 子句
template <typename T>
requires requires(T a, T b) { a + b; }
T add(T a, T b) {
  return a + b;
}
```

### Concepts 的优势

- 更清晰的错误信息（直接指出哪个约束不满足）
- 更直观的语法
- 可以组合和复用

---

## 9.6 编译期类型列表

利用变参模板在类型级别操作：

```cpp
// 类型列表
template <typename... Ts>
struct TypeList {};

// 获取长度
template <typename List>
struct Length;

template <typename... Ts>
struct Length<TypeList<Ts...>> {
  static constexpr size_t value = sizeof...(Ts);
};

// 获取第 N 个类型
template <size_t N, typename List>
struct TypeAt;

template <typename Head, typename... Tail>
struct TypeAt<0, TypeList<Head, Tail...>> {
  using type = Head;
};

template <size_t N, typename Head, typename... Tail>
struct TypeAt<N, TypeList<Head, Tail...>> {
  using type = typename TypeAt<N - 1, TypeList<Tail...>>::type;
};

// 使用
using MyList = TypeList<int, double, string>;
cout << Length<MyList>::value;                       // 3
static_assert(is_same<TypeAt<1, MyList>::type, double>::value, "");
```

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 编译期计算 | 模板递归 + 特化终止，结果为编译期常量 |
| 类型萃取 | `<type_traits>` 编译期类型查询与转换 |
| SFINAE | 替换失败不是错误，实现条件性重载 |
| `if constexpr` | C++17 编译期条件分支，比 SFINAE 简洁 |
| Concepts | C++20 命名约束，更清晰的模板参数限制 |
| 类型列表 | 变参模板实现类型级运算 |
