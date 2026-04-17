/**
 * 第二章：构造函数与析构函数 -- 对象的生与死
 *
 * 本章要点：
 * 1: 默认构造函数、参数化构造函数
 * 2: 拷贝构造函数
 * 3: 移动构造函数（C++11）
 * 4: 析构函数
 * 5: 初始化列表
 * 6: Rule of Three / Five / Zero
 */

#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>

using namespace std;

// ============================================================
// 2.1. 构造函数的各种形态 -- 以字符串为例
// ============================================================
class Mystring {
private:
  char *data;
  size_t length;

public:
  // ------ 默认构造函数 ------
  Mystring() : data(nullptr), length(0) {
    cout << " [默认构造] 空字符串" << endl;
  }

  // ------ 参数化构造函数 (用初始化列表) ------
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

  // ------ 拷贝构造函数 (深拷贝) ------
  // 当用一个对象初始化另外一个对象时调用
  // 例如: Mystring s2 = s1; 或 Mystring s2(s1);
  Mystring(const Mystring &other) {
    length = other.length;
    if (other.data) {
      data = new char[length + 1];
      strcpy(data, other.data);
    } else {
      data = nullptr;
    }
    cout << " [拷贝构造] \"" << data << "\"" << endl;
  }

  // ------ 拷贝赋值运算符 ------
  // 当把一个已经存在的对象复制给另一个已经存在的对象时调用
  // 例如: s2 = s1
  Mystring &operator=(const Mystring &other) {
    cout << " [拷贝赋值] \"" << other.data << "\"" << endl;
    if (this != &other) { // 自赋值检测
      delete[] data;      // 释放旧内存
      length = other.length;
      if (other.data) {
        data = new char[length + 1];
        strcpy(data, other.data);
      } else {
        data = nullptr;
      }
    }
    return *this;
  }

  // ------ 移动构造函数 (C++11) ------
  // 接收右值引用, “偷取”资源而非复制
  Mystring(Mystring &&other) noexcept : data(other.data), length(other.length) {
    // 将资源对象置为安全的空状态
    other.data = nullptr;
    other.length = 0;
    cout << " [移动构造] 资源转移完成" << endl;
  }

  // ------ 移动赋值运算符 ------
  Mystring &operator=(Mystring &&other) noexcept {
    cout << " [移动赋值] \"" << other.data << "\"" << endl;
    if (this != &other) {
      delete[] data;     // 释放自己的旧资源
      data = other.data; // 窃取资源
      length = other.length;
      other.data = nullptr; // 源对象置空
      other.length = 0;
    }
    return *this;
  }

  // ------ 析构函数 ------
  // 对象销毁时自动调用，释放资源
  ~Mystring() {
    cout << " [析构] \"" << (data ? data : "空") << "\"" << endl;
    delete[] data;
  }

  // 辅助函数
  const char *c_str() const { return data ? data : ""; }
  size_t size() const { return length; }
};

// ============================================================
// 2.2. 初始化函数列表的使用
// ============================================================
class Rectangle {
  const double width; // const成员必须用初始化列表
  const double height;
  double areaCache; // 普通成员也可以用初始化列表

public:
  // 初始化列表: 在构造函数体执行之前完成成员初始化
  // 顺序按照成员声明顺序， 而不是列表中的顺序
  Rectangle(double w, double h) : width(w), height(h), areaCache(w * h) {
    cout << " 矩形: " << width << " x " << height << ", 面积" << areaCache
         << endl;
  }

  void print() const {
    cout << " 宽 = " << width << " 高 = " << height << " 面积 = " << areaCache
         << endl;
  }
};

// ============================================================
// 2.3. 委托构造函数 (C++11)
// ============================================================
class Logger {
  string prefix;
  int level;

public:
  // 主构造函数
  Logger(const string &p, int l) : prefix(p), level(l) {
    cout << " Logger(\"" << prefix << "\", " << level << endl;
  }

  // 委托构造: 调用另一个构造函数完成初始化
  Logger() : Logger("Default", 0) { cout << " (委托到默认构造完成)" << endl; }

  Logger(const string &p) : Logger(p, 1) {
    cout << " (委托到单参数构造完成)" << endl;
  }
  void print() const {
    cout << " [" << prefix << "] level = " << level << endl;
  }
};

// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 2.1 各种构造与析构 =============" << endl;

  cout << "\n--- 默认构造 ---" << endl;
  Mystring s1;

  cout << "\n--- 参数化构造---" << endl;
  Mystring s2("Hello");

  cout << "\n--- 拷贝构造 ---" << endl;
  Mystring s3 = s2; // 拷贝构造，非赋值！

  cout << "\n--- 拷贝赋值 ---" << endl;
  Mystring s4;
  s4 = s2; // 拷贝赋值

  cout << "\n--- 移动构造 ---" << endl;
  Mystring s5 = std::move(s2); // s2 的资源被转移到 s5
  cout << " s5: \"" << s5.c_str() << "\", s2 的大小" << s2.size() << endl;

  cout << "\n--- 移动赋值 ---" << endl;
  Mystring s6("World");
  s6 = std::move(s3);
  cout << " s6: \"" << s6.c_str() << "\"" << endl;

  cout << "\n--- 析构顺序 （与构造顺序相反） ---" << endl;
  // 栈上对象按照声明的逆袭析构

  cout << "=========== 2.2 初始化列表 =============" << endl;
  Rectangle rect(5.0, 3.0);
  rect.print();

  cout << "=========== 2.3 委托构造 =============" << endl;
  Logger log1;
  log1.print();
  Logger log2("NEWORK");
  log2.print();
  Logger log3("DISABLE", 3);
  log3.print();

  cout << "\n ---程序结束，观察析构 ---" << endl;

  return 0;
}
