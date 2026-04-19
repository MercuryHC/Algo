# 第八章：模板类 -- 泛型编程的基础

> 本章讲解 C++ 模板编程的核心内容，包括类模板、非类型参数、特化与偏特化、变参模板和模板模板参数。

**源码位置**: `ch08_template_class/src/main.cpp`

---

## 8.1 模板类：智能指针 SmartPtr

通过类模板实现一个简化版的独占式智能指针，演示模板类的完整设计：

```cpp
template <typename T>
class SmartPtr {
  T *ptr;                     // 管理任意类型的指针

public:
  explicit SmartPtr(T *p = nullptr) : ptr(p) {}
  ~SmartPtr() { delete ptr; }

  // 禁止拷贝（独占所有权）
  SmartPtr(const SmartPtr &) = delete;
  SmartPtr &operator=(const SmartPtr &) = delete;

  // 移动语义: 转移所有权
  SmartPtr(SmartPtr &&other) noexcept : ptr(other.ptr) {
    other.ptr = nullptr;
  }
  SmartPtr &operator=(SmartPtr &&other) noexcept {
    if (this != &other) {
      delete ptr;
      ptr = other.ptr;
      other.ptr = nullptr;
    }
    return *this;
  }

  // 解引用操作符
  T &operator*() { return *ptr; }
  T *operator->() { return ptr; }

  // 工具方法
  T *get() const { return ptr; }
  T *release() { T *tmp = ptr; ptr = nullptr; return tmp; }
  explicit operator bool() const { return ptr != nullptr; }
};
```

**使用**:

```cpp
{
  SmartPtr<int> sp1(new int(42));
  cout << *sp1;                    // 42

  SmartPtr<string> sp2(new string("Hello"));
  cout << sp2->length();           // 5

  SmartPtr<int> sp3 = std::move(sp1);   // 所有权转移
  cout << (sp1 ? "非空" : "空");         // 空
  cout << *sp3;                          // 42
}   // 离开作用域，sp2、sp3 自动析构释放资源
```

### 模板类的实例化

```cpp
SmartPtr<int>       → 编译器生成 SmartPtr<int> 版本
SmartPtr<string>    → 编译器生成 SmartPtr<string> 版本
```

编译器在**编译期**根据使用到的具体类型，从模板生成对应的类代码。不同类型参数产生完全独立的类。

---

## 8.2 非类型模板参数与默认值

模板参数不仅限于类型，还可以是编译期常量值（整数、枚举、指针等）：

```cpp
template <typename T, size_t N = 10>    // T 是类型参数，N 是非类型参数（有默认值 10）
class FixedArray {
  T data[N];                            // 编译期确定大小的数组
  size_t count;

public:
  FixedArray() : count(0) {}

  void push_back(const T &value) {
    if (count < N) data[count++] = value;
    else throw runtime_error("数组已满");
  }

  void push_back(T &&value) {           // 右值引用重载
    if (count < N) data[count++] = std::move(value);
    else throw runtime_error("数组已满");
  }

  T &operator[](size_t index) {
    if (index >= count) throw out_of_range("索引越界");
    return data[index];
  }

  size_t size() const { return count; }
  size_t capacity() const { return N; }

  // 迭代器支持
  T *begin() { return data; }
  T *end() { return data + count; }
};
```

**使用**:

```cpp
FixedArray<int, 5> arr;              // 容量 5 的 int 数组
arr.push_back(10);
arr.push_back(20);

FixedArray<string> defaultArr;       // 容量 10（使用默认值）的 string 数组
cout << defaultArr.capacity();       // 10

// 范围 for 循环（借助 begin()/end()）
for (const auto &elem : arr) {
  cout << elem << " ";
}
```

### 模板参数分类

| 参数类型 | 示例 | 说明 |
|---------|------|------|
| 类型参数 | `typename T` | 代表一种类型 |
| 非类型参数 | `size_t N` | 编译期常量值 |
| 模板模板参数 | `template<typename> class C` | 模板本身作为参数 |
| 默认参数 | `size_t N = 10` | 可省略，使用默认值 |

---

## 8.3 模板类的特化与偏特化

当通用模板对某些特定类型不适用或可以优化时，使用特化提供专门实现。

### 通用版本

```cpp
template <typename T, typename U>
class Pair {
public:
  T first;
  U second;
  Pair(const T &a, const U &b) : first(a), second(b) {}
  void print() const {
    cout << "Pair: (" << first << ", " << second << ")" << endl;
  }
};
```

### 全特化 (Full Specialization)

固定**所有**模板参数，为特定类型组合提供专门实现：

```cpp
template <>
class Pair<int, int> {               // 当 T 和 U 都是 int 时使用此版本
public:
  int first, second;
  Pair(int a, int b) : first(a), second(b) {}
  void print() const {
    cout << "相同类型对: (" << first << ", " << second << ")" << endl;
  }
  int sum() const { return first + second; }    // 全特化版本独有的方法
  int max() const { return (first > second) ? first : second; }
};
```

> **注意**: 本章源码中使用的是 `template <typename T> class Pair<T, T>` 形式（只固定两者为相同类型，但不限定具体类型），这在严格意义上属于**偏特化**。全特化是所有参数都固定为具体类型，如 `Pair<int, int>`。

### 偏特化 (Partial Specialization)

固定**部分**模板参数或对参数施加约束：

```cpp
// 当第二个参数为指针类型时
template <typename T>
class Pair<T, T*> {
public:
  T first;
  T *second;
  bool owned;                         // 是否拥有指针的所有权

  Pair(const T &a, T *b, bool take_ownership = false)
      : first(a), second(b), owned(take_ownership) {}

  ~Pair() {
    if (owned) { delete second; }     // 管理指针生命周期
  }

  void print() const {
    cout << "偏特化: (" << first << ", "
         << (second ? to_string(*second) : "nullptr") << ")" << endl;
  }
};
```

**使用**:

```cpp
Pair<int, double> p1(1, 3.14);        // 使用通用版本
Pair<int, int> p2(42, 99);            // 使用偏特化（同类型版本）
p2.sum();                              // 141
p2.max();                              // 99

int val = 200;
Pair<int, int*> p3(100, &val);        // 使用偏特化（指针版本）
```

### 特化层次

```
通用模板    <typename T, typename U>
  ├── 偏特化  <typename T>         (T, T)  -- 同类型
  │     └── 全特化  <>              (int, int)
  └── 偏特化  <typename T>         (T, T*) -- 第二参数为指针
```

编译器选择**最特化**（最具体）的匹配版本。

---

## 8.4 变参模板与完美转发

### 变参模板 (Variadic Templates)

使用 `...`（省略号）接受任意数量、任意类型的参数：

```cpp
// 递归终止条件
void printArgs() {
  cout << "参数列表结束" << endl;
}

// 递归展开: 处理第一个参数，剩余参数递归
template <typename T, typename... Args>
void printArgs(const T &first, const Args &...rest) {
  cout << "参数: " << first << endl;
  cout << "剩余参数数量: " << sizeof...(rest) << endl;
  printArgs(rest...);                 // 递归调用
}
```

**调用过程**:

```
printArgs(42, "Hello", 3.14)
  → 参数: 42, 剩余: 2
  printArgs("Hello", 3.14)
    → 参数: Hello, 剩余: 1
    printArgs(3.14)
      → 参数: 3.14, 剩余: 0
      printArgs()           → 参数列表结束
```

### 变参模板类

```cpp
template <typename... Types>
class Tuple {
public:
  tuple<Types...> data;               // 使用 std::tuple 存储

  Tuple(Types... args) : data(std::make_tuple(args...)) {}

  template <size_t I>
  auto get() -> decltype(std::get<I>(data)) {
    return std::get<I>(data);
  }

  static constexpr size_t size() { return sizeof...(Types); }
};
```

**使用**:

```cpp
Tuple<int, string, double> t(42, "Alice", 3.14);
cout << t.size();          // 3
cout << t.get<0>();         // 42
cout << t.get<1>();         // Alice
cout << t.get<2>();         // 3.14
```

### 完美转发

使用 `std::forward` 保持参数的值类别（左值/右值）：

```cpp
template <typename... Args>
void forwardArgs(Args &&...args) {
  target(std::forward<Args>(args)...);   // 完美转发
}
```

---

## 8.5 模板模板参数

将**模板本身**作为模板参数，实现对容器的抽象：

```cpp
// Container 是一个接受一个类型参数的模板
template <template <typename> class Container, typename T>
class Stack {
  Container<T> container;                // 使用任意容器类型

public:
  void push(const T &val) { container.push_back(val); }
  T pop() {
    T val = container.back();
    container.pop_back();
    return val;
  }
  bool empty() const { return container.empty(); }
  size_t size() const { return container.size(); }
};
```

**自定义容器适配**:

```cpp
template <typename T>
class SimpleVector {
  T *data;
  size_t len, cap;
public:
  SimpleVector() : data(new T[4]), len(0), cap(4) {}
  ~SimpleVector() { delete[] data; }
  void push_back(const T &val) { ... }
  void pop_back() { if (len > 0) --len; }
  T &back() { return data[len - 1]; }
  bool empty() const { return len == 0; }
  size_t size() const { return len; }
};
```

**使用**:

```cpp
Stack<SimpleVector, int> myStack;     // 使用 SimpleVector 作为底层容器
myStack.push(10);
myStack.push(20);
myStack.push(30);
cout << myStack.pop();                // 30
cout << myStack.pop();                // 20
```

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 类模板 | `template <typename T>` 定义泛型类 |
| 非类型参数 | `size_t N` 编译期常量 |
| 默认参数 | `size_t N = 10` 可省略 |
| 全特化 | 固定所有参数，如 `Pair<int, int>` |
| 偏特化 | 固定部分参数或加约束，如 `Pair<T, T*>` |
| 变参模板 | `typename... Args` 接受任意数量参数 |
| `sizeof...` | 获取参数包中的参数数量 |
| 完美转发 | `std::forward` 保持值类别 |
| 模板模板参数 | `template <typename> class C` 模板作为参数 |
