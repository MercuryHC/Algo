# 第二章：构造函数与析构函数 -- 对象的生与死

> 本章深入讲解对象的生命周期管理，涵盖默认构造、参数化构造、拷贝/移动语义、析构函数、初始化列表和委托构造。

**源码位置**: `ch02_constructor/src/main.cpp`

---

## 2.1 构造函数的各种形态 -- 以 Mystring 为例

通过自定义字符串类 `Mystring`，完整演示对象的构造、拷贝、移动和析构全过程。

### 默认构造函数

无参数（或所有参数都有默认值）的构造函数：

```cpp
Mystring() : data(nullptr), length(0) {
  cout << " [默认构造] 空字符串" << endl;
}
```

```cpp
Mystring s1;   // 触发默认构造
```

### 参数化构造函数

接收参数完成初始化：

```cpp
Mystring(const char *str) {
  if (str) {
    length = strlen(str);
    data = new char[length + 1];
    strcpy(data, str);
  } else {
    data = nullptr;
    length = 0;
  }
  cout << " [参数构造] \"" << (data ? data : "") << "\"" << endl;
}
```

```cpp
Mystring s2("Hello");   // 触发参数化构造
```

### 拷贝构造函数（深拷贝）

用一个已有对象初始化新对象时调用，实现**深拷贝**（分配新内存并复制数据）：

```cpp
Mystring(const Mystring &other) {
  length = other.length;
  if (other.data) {
    data = new char[length + 1];
    strcpy(data, other.data);   // 深拷贝: 分配新内存
  } else {
    data = nullptr;
  }
  cout << " [拷贝构造] \"" << data << "\"" << endl;
}
```

**触发场景**:

```cpp
Mystring s3 = s2;    // 拷贝构造（初始化，不是赋值！）
Mystring s3(s2);     // 同上，另一种写法
```

### 拷贝赋值运算符

将一个已有对象的值赋给另一个已有对象：

```cpp
Mystring &operator=(const Mystring &other) {
  if (this != &other) {       // 1. 自赋值检测
    delete[] data;             // 2. 释放旧内存
    length = other.length;
    if (other.data) {
      data = new char[length + 1];
      strcpy(data, other.data);   // 3. 深拷贝
    } else {
      data = nullptr;
    }
  }
  return *this;               // 4. 返回自身引用
}
```

```cpp
Mystring s4;          // 默认构造
s4 = s2;              // 拷贝赋值（s4 已存在）
```

> **自赋值检测至关重要**: 如果没有 `if (this != &other)`，当 `s = s;` 时会先释放自身内存，导致读取已释放的数据。

### 移动构造函数（C++11）

接收**右值引用**，直接"窃取"资源指针，避免不必要的内存分配和复制：

```cpp
Mystring(Mystring &&other) noexcept
    : data(other.data), length(other.length) {
  other.data = nullptr;     // 源对象置空，防止重复释放
  other.length = 0;
  cout << " [移动构造] 资源转移完成" << endl;
}
```

```cpp
Mystring s5 = std::move(s2);   // s2 的资源转移到 s5，s2 变为空
```

### 移动赋值运算符

```cpp
Mystring &operator=(Mystring &&other) noexcept {
  if (this != &other) {
    delete[] data;           // 释放自己的旧资源
    data = other.data;       // 窃取资源
    length = other.length;
    other.data = nullptr;    // 源对象置空
    other.length = 0;
  }
  return *this;
}
```

```cpp
Mystring s6("World");
s6 = std::move(s3);    // s3 的资源转移到 s6
```

### 析构函数

对象销毁时自动调用，负责释放资源：

```cpp
~Mystring() {
  cout << " [析构] \"" << (data ? data : "空") << "\"" << endl;
  delete[] data;   // 释放动态分配的内存
}
```

**析构顺序**: 栈上的对象按声明的**逆序**析构（先构造的后析构，类似栈的 LIFO）。

### 构造/赋值函数速查表

| 函数 | 触发示例 | 核心操作 |
|------|---------|---------|
| 默认构造 | `Mystring s;` | 初始化为空状态 |
| 参数化构造 | `Mystring s("Hi");` | 分配内存并复制 |
| 拷贝构造 | `Mystring s2 = s1;` | 深拷贝（新内存） |
| 拷贝赋值 | `s2 = s1;` | 释放旧内存 + 深拷贝 |
| 移动构造 | `Mystring s2 = std::move(s1);` | 窃取指针，源置空 |
| 移动赋值 | `s2 = std::move(s1);` | 释放旧 + 窃取 + 源置空 |
| 析构函数 | 离开作用域 | 释放资源 |

### 深拷贝 vs 移动语义对比

```
深拷贝:  [新分配内存] ← 复制数据 ← [原内存]    (两份独立数据)
移动:    [窃取指针] ← 转移所有权              (零拷贝，源对象置空)
```

---

## 2.2 初始化列表

初始化列表在构造函数体**执行之前**完成成员初始化，比在函数体内赋值更高效（少一次默认构造 + 赋值）。

```cpp
class Rectangle {
  const double width;    // const 成员：必须用初始化列表
  const double height;
  double areaCache;

public:
  Rectangle(double w, double h) : width(w), height(h), areaCache(w * h) {
    cout << "矩形: " << width << " x " << height << ", 面积: " << areaCache << endl;
  }
};
```

### 必须使用初始化列表的场景

1. **`const` 成员变量** -- const 变量不能在函数体内赋值
2. **引用类型成员** -- 引用必须在初始化时绑定
3. **没有默认构造函数的类类型成员** -- 无法先默认构造再赋值

### 初始化顺序

**按成员声明顺序初始化**，而非列表中的书写顺序：

```cpp
class Example {
  int a;    // 先初始化 a
  int b;    // 再初始化 b
public:
  Example(int x) : b(x), a(b) {}   // 危险! a 先于 b 初始化，此时 b 未定义
};
```

> **最佳实践**: 初始化列表的书写顺序与成员声明顺序保持一致。

---

## 2.3 委托构造函数（C++11）

一个构造函数调用同类中的另一个构造函数完成初始化：

```cpp
class Logger {
  string prefix;
  int level;

public:
  // 主构造函数
  Logger(const string &p, int l) : prefix(p), level(l) {
    cout << "Logger(\"" << prefix << "\", " << level << ")" << endl;
  }

  // 委托到主构造函数
  Logger() : Logger("Default", 0) {
    cout << " (委托到默认构造完成)" << endl;
  }

  // 委托到主构造函数
  Logger(const string &p) : Logger(p, 1) {
    cout << " (委托到单参数构造完成)" << endl;
  }
};
```

```cpp
Logger log1;              // 输出: Logger("Default", 0) → 委托到默认构造完成
Logger log2("NETWORK");   // 输出: Logger("NETWORK", 1) → 委托到单参数构造完成
Logger log3("DISABLE", 3); // 直接调用主构造函数
```

> **注意**: 委托构造函数的函数体在目标构造函数执行完毕后执行。

---

## 2.4 Rule of Three / Five / Zero

### Rule of Three

如果类需要自定义以下**任何一个**，通常需要自定义**全部三个**：
- 析构函数
- 拷贝构造函数
- 拷贝赋值运算符

原因：如果需要自定义析构函数（通常因为管理了资源），那么默认的拷贝语义（浅拷贝）通常是错误的。

### Rule of Five（C++11）

扩展 Rule of Three，加入：
- 移动构造函数
- 移动赋值运算符

### Rule of Zero（推荐）

尽量让类**不需要**自定义这五个函数：
- 使用智能指针（`unique_ptr`、`shared_ptr`）管理资源
- 使用 STL 容器代替手动内存管理
- 让编译器自动生成正确的默认实现

```cpp
// Rule of Zero 的典范
class GoodString {
  std::string data;    // 由 std::string 管理内存
public:
  GoodString(const std::string &s) : data(s) {}
  // 不需要自定义析构、拷贝、移动 -- 编译器自动生成正确版本
};
```

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 默认构造 | 无参构造，初始化为合理默认状态 |
| 参数化构造 | 接收参数完成初始化 |
| 拷贝语义 | 深拷贝，分配新内存复制数据 |
| 移动语义 | 窃取资源指针，源对象置空，零拷贝 |
| 析构函数 | 释放资源，按构造逆序调用 |
| 初始化列表 | 高效初始化，const/引用成员必须使用 |
| 委托构造 | 构造函数间复用初始化逻辑 |
| Rule of Five | 管理资源时需自定义全部五个特殊函数 |
| Rule of Zero | 优先使用 RAII 类型，让编译器自动生成 |
