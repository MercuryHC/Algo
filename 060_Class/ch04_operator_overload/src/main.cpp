/**
 * 第四章：运算符重载 -- 让你的类像内置类型一样使用
 *
 * 本章要点：
 * 1: 算数运算符 (+, -, *, /)
 * 2: 比较运算符 (==, !=, <, >, <=, >=)
 * 3: 流运算符 (<<, >>)
 * 4: 下标运算符 ([])
 * 5: 自增/自减运算符 (++, --)
 * 6: 函数调用运算符 (())
 * 7: 类型转换运算符
 */

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdexcept>

using namespace std;

// ============================================================
// 4.1. 算数运算符: 二维向量
// ============================================================
class Vec2 {
public:
  double x, y;
  Vec2(double x = 0, double y = 0) : x(x), y(y) {}

  // 成员函数形式的运算符
  Vec2 operator+(const Vec2 &rhs) { return Vec2(x + rhs.x, y + rhs.y); }
  Vec2 operator-(const Vec2 &rhs) { return Vec2(x - rhs.x, y - rhs.y); }
  // 标量乘法 (向量 * 标量)
  Vec2 operator*(double scale) { return Vec2(x * scale, y * scale); }
  // 点积
  double dot(const Vec2 &rhs) { return x * rhs.x + y * rhs.y; }
  double lenght() const { return sqrt(x * x + y * y); }

  // 比较运算符
  bool operator==(const Vec2 &rhs) const { return x == rhs.x && y == rhs.y; }
  bool operator!=(const Vec2 &rhs) const { return x != rhs.x || y != rhs.y; }
  bool operator<(const Vec2 &rhs) const { return lenght() < rhs.lenght(); }

  // 流输出运算符: 必须是友元 (左操作数是 ostream)
  friend ostream &operator<<(ostream &os, const Vec2 &v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
  }
};

// ============================================================
// 4.2. 下标运算符: 动态数组
// ============================================================
class IntArray {
  int *data;
  size_t len;

public:
  explicit IntArray(size_t size) : len(size) {
    data = new int[size](); //  值初始化 (归零)
  }
  ~IntArray() { delete[] data; }

  // 非 const 版本: 运行修改
  int &operator[](size_t index) {
    if (index >= len)
      throw out_of_range("索引越界");
    return data[index];
  }

  // const 版本: 只读访问
  int operator[](size_t index) const {
    if (index >= len)
      throw out_of_range("索引越限");
    return data[index];
  }

  size_t size() const { return len; }

  friend ostream &operator<<(ostream &os, const IntArray &arr) {
    os << "[";
    for (size_t i = 0; i < arr.len; ++i) {
      if (i > 0)
        os << ", ";
      os << arr.data[i];
    }
    os << "]";
    return os;
  }
};

// ============================================================
// 4.3 自增自减运算符
// ============================================================
class Iterator {
  int *ptr;

public:
  Iterator(int *p = nullptr) : ptr(p) {}

  // 前置++: 返回引用
  Iterator &operator++() {
    ++ptr;
    return *this;
  }
  // 后置++: 返回旧值 (int 沧桑仅用于分区)
  Iterator operator++(int) {
    Iterator tmp = *this;
    ++ptr;
    return tmp;
  }

  // 前置--
  Iterator operator--() {
    --ptr;
    return *this;
  }
  // 后置 --
  Iterator operator--(int) {
    Iterator tmp = *this;
    --ptr;
    return tmp;
  }

  // 解引用
  int &operator*() const { return *ptr; }

  bool operator==(const Iterator &rhs) const { return ptr == rhs.ptr; }
  bool operator!=(const Iterator &rhs) const { return ptr != rhs.ptr; }
};

// ============================================================
// 4.4 函数调用运算符 (仿函数) 与类型转换
// ============================================================
class Matrix2x2 {
  double m[2][2];

public:
  Matrix2x2() { memset(m, 0, sizeof(m)); }
  Matrix2x2(double a, double b, double c, double d) {
    m[0][0] = a;
    m[0][1] = b;
    m[1][0] = c;
    m[1][1] = d;
  }

  // 函数调用运算符: 像函数一样使用对象
  // m(row, col) 访问元素
  double &operator()(int row, int col) {
    if (row < 0 || row > 1 || col < 0 || col > 1) {
      throw out_of_range("矩阵索引越界");
    }
    return m[row][col];
  }
  double operator()(int row, int col) const {
    if (row < 0 || row > 1 || col < 0 || col > 1) {
      throw out_of_range("矩阵索引越界");
    }
    return m[row][col];
  }

  // 矩阵乘法
  Matrix2x2 operator*(const Matrix2x2 &rhs) const {
    Matrix2x2 result;
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        result(i, j) = 0;
        for (int k = 0; k < 2; ++k) {
          result(i, j) += m[i][k] * rhs(k, j);
        }
      }
    }
    return result;
  }

  // 显式类型转换运算符
  explicit operator double() const {
    // 返回行列式
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
  }

  friend ostream &operator<<(ostream &os, const Matrix2x2 &mat) {
    os << "[[" << mat.m[0][0] << ", " << mat.m[0][1] << "], "
       << "[" << mat.m[1][0] << ", " << mat.m[1][1] << "]]";
    return os;
  }
};

// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 4.1 向量算术运算符 =============" << endl;
  Vec2 v1(3, 4), v2(1, 2);
  cout << " v1 = " << v1 << endl;
  cout << " v2 = " << v2;
  cout << " v1 + v2 = " << (v1 + v2) << endl;
  cout << " v1 - v2 = " << (v1 - v2) << endl;
  cout << " v1 * 2 = " << (v1 * 2) << endl;
  // cout << " 2 * v1 = " << (2 * v1) << endl;
  cout << " v1 · v1 = " << v1.dot(v2) << endl;
  cout << " 2 < v1 ? " << (v1 < v2 ? "是" : "否") << endl;
  cout << endl;

  cout << "=========== 4.2 下标运算符 =============" << endl;
  IntArray arr(5);
  for (size_t i = 0; i < arr.size(); ++i) {
    arr[i] = static_cast<int>(i * i);
  }
  cout << " arr = " << arr << endl;
  const IntArray &carr = arr;
  cout << " carr[2] = " << carr[2] << " (const 版本)" << endl;
  cout << endl;

  cout << "=========== 4.3 自增自减运算符 =============" << endl;
  int num[] = {10, 20, 30, 40, 50};
  Iterator it(num);
  cout << " *it = " << *it << endl;
  cout << " *++it = " << *++it << endl; // 前置: 先加在用
  cout << " *it++ = " << *it++ << ", 现在 *it = " << *it
       << endl; // 后置： 先用再加
  cout << endl;

  cout << "=========== 4.4 函数调用运算符与类型转换 =============" << endl;
  Matrix2x2 mat1(1, 2, 3, 4);
  Matrix2x2 mat2(5, 6, 7, 8);
  cout << " mat1 = " << mat1 << endl;
  cout << " mat2 = " << mat2 << endl;
  cout << " mat1 * mat2 = " << (mat1 * mat2) << endl;

  // 用 () 访问元素
  cout << " mat1(0, 1) = " << mat1(0, 1) << endl;
  mat1(0, 0) = 99; // 修改元素
  cout << " 修改 mat1(0,0) = 99 后 mat1 = " << mat1 << endl;

  // 显式类型转换
  double det = static_cast<double>(mat1);
  cout << " mat1 的行列式 = " << det << endl;

  return 0;
}
