/**
 * 第八章：模板类 -- 泛型编程的基础
 *
 * 本章要点：
 * 1: 函数模板与类模板
 * 2: 模板参数 (类型参数、非类型参数、默认参数)
 * 3: 模板成员函数
 * 4: 模板与友元
 * 5: 变参模板 (variadic templates)
 * 6: 完美转发
 */

#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <tuple>

using namespace std;

// ============================================================
// 8.1 模板类： 智能指针
// ============================================================
template <typename T> class SmartPtr {
  T *ptr;

public:
  // 构造
  explicit SmartPtr(T *p = nullptr) : ptr(p) {
    cout << "SmartPtr 构造，管理资源: " << endl;
  }
  // 析构
  ~SmartPtr() {
    cout << "SmartPtr 析构，释放资源: " << endl;
    delete ptr;
  }

  // 禁止拷贝 (独占所有权)
  SmartPtr(const SmartPtr &) = delete;
  SmartPtr &operator=(const SmartPtr &) = delete;

  // 移动构造
  SmartPtr(SmartPtr &&other) noexcept : ptr(other.ptr) {
    other.ptr = nullptr; // 转移所有权
    cout << "SmartPtr 移动构造，转移资源: "
         << (ptr ? to_string(*ptr) : "nullptr") << endl;
  }
  // 移动赋值
  SmartPtr &operator=(SmartPtr &&other) noexcept {
    if (this != &other) {
      delete ptr;      // 释放当前资源
      ptr = other.ptr; // 转移所有权
      other.ptr = nullptr;
      cout << "SmartPtr 移动赋值，转移资源: "
           << (ptr ? to_string(*ptr) : "nullptr") << endl;
    }
    return *this;
  }

  // 解引用操作符
  T &operator*() {
    if (!ptr) {
      throw runtime_error("解引用空指针");
    }
    return *ptr;
  }
  T *operator->() {
    if (!ptr) {
      throw runtime_error("访问空指针");
    }
    return ptr;
  }

  // 获取原始指针
  T *get() const { return ptr; }

  // 释放所有权
  T *release() {
    T *temp = ptr;
    ptr = nullptr; // 放弃所有权
    cout << "SmartPtr 释放所有权，资源: "
         << (temp ? to_string(*temp) : "nullptr") << endl;
    return temp;
  }

  explicit operator bool() const { return ptr != nullptr; }
};

// ============================================================
// 8.2 非类型模板参数与默认值
// ============================================================
template <typename T, size_t N = 10> class FixedArray {
  T data[N];
  size_t count;

public:
  FixedArray() : count(0) {}
  void push_back(const T &value) {
    if (count < N) {
      data[count++] = value;
    } else {
      throw runtime_error("数组已满");
    }
  }
  void push_back(T &&value) {
    if (count < N) {
      data[count++] = std::move(value);
    } else {
      throw runtime_error("数组已满");
    }
  }

  T &operator[](size_t index) {
    if (index >= count) {
      throw out_of_range("索引越界");
    }
    return data[index];
  }
  const T &operator[](size_t index) const {
    if (index >= count) {
      throw out_of_range("索引越界");
    }
    return data[index];
  }

  size_t size() const { return count; }
  size_t capacity() const { return N; }

  // 迭代器支持
  T *begin() { return data; }
  T *end() { return data + count; }
  const T *begin() const { return data; }
  const T *end() const { return data + count; }
};

// ============================================================
// 8.3 模板类的特化与偏特化
// ============================================================
// 通用版本
template <typename T, typename U> class Pair {
public:
  T first;
  U second;

  Pair(const T &a, const U &b) : first(a), second(b) {}

  void print() const {
    cout << "Pair: (" << first << ", " << second << ")" << endl;
  }
};

// 全特化: 两个类型相同时的特殊处理
template <typename T> class Pair<T, T> {
public:
  T first;
  T second;

  Pair(const T &a, const T &b) : first(a), second(b) {}

  void print() const {
    cout << " 相同类型对: (" << first << ", " << second << ")" << endl;
  }

  T sum() const { return first + second; }
  T max() const { return (first > second) ? first : second; }
};

// 偏特化: 第二个类型为指针的特殊处理
template <typename T> class Pair<T, T *> {
public:
  T first;
  T *second;
  bool owned; // 是否拥有 second 的所有权

  Pair(const T &a, T *b, bool take_ownership = false)
      : first(a), second(b), owned(take_ownership) {}
  ~Pair() {
    if (owned) {
      delete first;
      delete second;
    }
  }
  void print() const {
    cout << " 偏特化类型对: (" << first << ", "
         << (second ? to_string(*second) : "nullptr") << ")" << endl;
  }
};

// ============================================================
// 8.4 变参模板与完美转发
// ============================================================
// 递归终止条件
void printArgs() { cout << "参数列表结束" << endl; }

// 递归展开: 打印任意数量的参数
template <typename T, typename... Args>
void printArgs(const T &first, const Args &...reset) {
  cout << "参数: " << first << endl;
  if (sizeof...(reset) > 0)
    cout << "剩余参数数量: " << sizeof...(reset) << endl;
  printArgs(reset...); // 完美转发剩余参数
}

// 类型安全的变参模板容器
template <typename... Types> class Tuple {
public:
  // 内部存储: 使用 std::tuple 来存储任意类型的参数
  tuple<Types...> data;

  Tuple(Types... args) : data(std::make_tuple(args...)) {}

  // 获取指定位置的值
  template <size_t I> auto get() -> decltype(std::get<I>(data)) {
    return std::get<I>(data);
  }

  static constexpr size_t size() { return sizeof...(Types); }
};

// ============================================================
// 8.5 模板模板参数
// ============================================================
template <template <typename> class Container, typename T> class Stack {
  Container<T> container;

public:
  void push(const T &val) { container.push_back(val); }

  T pop() {
    if (container.empty()) {
      throw runtime_error("栈为空");
    }
    T val = container.back();
    container.pop_back();
    return val;
  }

  bool empty() const { return container.empty(); }
  size_t size() const { return container.size(); }
};

// 适配 vector 为简单容器
template <typename T> class SimpleVector {
  T *data;
  size_t len, cap;

public:
  SimpleVector() : data(new T[4]), len(0), cap(4) {}
  ~SimpleVector() { delete[] data; }

  void push_back(const T &val) {
    // 扩容逻辑
    if (len == cap) {
      cap *= 2;
      T *newData = new T[cap];
      memcpy(newData, data, len * sizeof(T));
      delete[] data;
      data = newData;
    }
    data[len++] = val;
  }

  void pop_back() {
    if (len > 0) {
      --len;
    }
  }
  T &back() {
    if (len == 0) {
      throw runtime_error("容器为空");
    }
    return data[len - 1];
  }
  bool empty() const { return len == 0; }
  size_t size() const { return len; }
};
// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 8.1 模板类：智能指针 =============" << endl;
  {
    SmartPtr<int> sp1(new int(42));
    cout << " sp1 管理的值: " << *sp1 << endl;

    SmartPtr<string> sp2(new string("Hello, 模板类!"));
    cout << " sp2字符串: " << *sp2 << endl;
    cout << " sp2长度: " << sp2->length() << endl;

    // 移动语义示例
    SmartPtr<int> sp3 = std::move(sp1);
    cout << " 移动后的sp1: " << (sp1.get() ? "非空" : "空") << endl;
    cout << " 移动后的sp3: " << *sp3 << endl;
  } // sp1, sp2, sp3 在此作用域结束，资源自动释放
  // cout << *sp3 << endl; // 访问已释放资源，可能导致未定义行为
  cout << endl;

  cout << "=========== 8.2 非类型模板参数与默认值 =============" << endl;
  FixedArray<int, 5> arr;
  for (int i = 0; i < 5; ++i) {
    arr.push_back(i * i);
  }

  cout << " 数组容量: " << arr.capacity() << endl;
  cout << " 数组元素: ";
  for (size_t i = 0; i < arr.size(); ++i) {
    cout << arr[i] << " ";
  }
  cout << endl;

  // 使用默认大小
  FixedArray<string> defaultArr;
  cout << " 默认数组容量: " << defaultArr.capacity() << endl;
  cout << endl;

  cout << "=========== 8.3 模板类的特化与偏特化 =============" << endl;
  Pair<int, double> p1(1, 3.14);
  p1.print();

  Pair<int, int> p2(42, 99); // 触发全特化版本
  p2.print();
  cout << " 和: " << p2.sum() << ", 最大: " << p2.max() << endl;

  int a = 100, b = 200;
  Pair<int *, int> p3(&a, b); // 触发偏特化版本，拥有指针所有权
  p3.print();
  cout << endl;

  cout << "=========== 8.4 变参模板与完美转发 =============" << endl;
  cout << " ";
  printArgs(42, "Hello", 3.14, string("World"));

  Tuple<int, string, double> tTuple(42, "Alice", 3.14);
  cout << " Tuple 大小: " << tTuple.size() << endl;
  cout << " Tuple 第0个元素: " << tTuple.get<0>() << endl;
  cout << " Tuple 第1个元素: " << tTuple.get<1>() << endl;
  cout << " Tuple 第2个元素: " << tTuple.get<2>() << endl;
  cout << endl;

  cout << "=========== 8.5 模板模板参数 =============" << endl;
  Stack<SimpleVector, int> myStack;
  myStack.push(10);
  myStack.push(20);
  myStack.push(30);

  cout << " 栈顶元素: " << myStack.pop() << endl;
  cout << " 栈顶元素: " << myStack.pop() << endl;
  cout << " 栈是否为空: " << (myStack.empty() ? "是" : "否") << endl;

  return 0;
}
