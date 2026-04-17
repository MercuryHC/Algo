/**
 * 第一章：类的基础 - 从struct到class
 *
 * 本章要点：
 * 1: 类的定义与实例化
 * 2: 成员变量与成员函数
 * 3: this 指针
 * 4: struct 与 class 的区别
 */

#include <cmath>
#include <iostream>
#include <string>

using namespace std;

// ============================================================
// 1.1. 最简单的类，用 struct 定义一个点
// ============================================================
// struct 默认成员是 public
struct Point {
  double x;
  double y;

  // 成员函数: 计算到原点的距离
  double distanceFromOrigin() const { return sqrt(x * x + y * y); }

  // 成员函数：打印自身
  void print() const { cout << "(" << x << ", " << y << ")" << endl; }
};

// ============================================================
// 1.2. 用 class 定义一个学生类
// ============================================================
// struct 默认成员是 private（这是与 struct 的唯一区别）
class Student {
private: // 私有成员: 外部不可直接访问
  string name;
  int age;
  double score;

public: // 公有成员: 外部可以访问
  // 成员函数: 设置信息
  void setInfo(const string &n, int a, double s) {
    name = n;
    age = a;
    score = s;
  }

  // 成员函数: 打印信息
  void printInfo() const {
    // this 是指向当前对象的指针
    cout << "姓名: " << this->name << ", 年龄" << this->age << ", 成绩"
         << this->score << endl;
  }

  //  成员函数: 判断是否及格
  bool isPassefd() const { return score >= 60.0; }
};

// ============================================================
// 1.3. this 指针的显示使用
// ============================================================
class Counter {
  int value;

public:
  // 在构造函数中用 this 指针区分同名参数
  Counter(int value) {
    this->value = value; // this->value 是成员, value 是参数
  }

  //  返回 *this 支持链式调用
  Counter &increment() {
    ++this->value;
    return *this; // 返回自身引用
  }

  Counter &add(int n) {
    this->value += n;
    return *this;
  }

  int get() const { return value; }
};

// ============================================================
// 1.4. 常量成员函数 （const 成员函数）
// ============================================================
class Circle {
  double radius;

public:
  Circle(double r) : radius(r) {}

  // const 放在函数末尾声明，表示该函数不会被修改成员变量
  // const的对象只能调用const成员函数
  double area() const { return 3.14159265 * radius * radius; }

  double cicumference() const { return 2 * 3.14159265 * radius; }

  // 非const函数: 可以修改成员变量
  void setRadius(double r) { radius = r; }
};

// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 1.1 struct 定义的点 =============" << endl;
  Point p1 = {3.0, 4.0};
  p1.print();
  cout << "到原点的距离: " << p1.distanceFromOrigin() << endl;
  cout << endl;

  cout << "=========== 1.2 class 定义的学生 ===========" << endl;
  Student s1, s2;
  s1.setInfo("张三", 20, 85.8);
  s2.setInfo("李四", 21, 55.5);
  s1.printInfo();
  s2.printInfo();
  cout << "张三" << (s1.isPassefd() ? "及格" : "不及格") << endl;
  cout << "李四" << (s2.isPassefd() ? "及格" : "不及格") << endl;
  cout << endl;

  cout << "========= 1.3 this 指针与链式调用 =========" << endl;
  Counter c(0);
  // 链式调用: increment() 返回 *this, 所以可以连续输出
  c.increment().increment().add(10);
  cout << "计数器值: " << c.get() << endl;
  cout << endl;

  cout << "========= 1.4 const 成员函数 =============" << endl;
  const Circle c1(5.0); // const 对象
  cout << "半径 = 5 的圆面积: " << c1.area() << endl;
  cout << "半径 = 5 的圆周长: " << c1.cicumference() << endl;
  // c1.setRadius(10)      // 错误！const 对象不能调用非const函数

  Circle c2(3.0); // 非const对象
  cout << "半径 = 3 的圆面积: " << c2.area() << endl;
  c2.setRadius(10.0); // 可以调用, c2非const
  cout << "修改后半径 = 10 的圆面积: " << c2.area() << endl;

  return 0;
}
