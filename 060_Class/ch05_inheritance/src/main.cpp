/**
 * 第五章: 继承  -- 代码复用与层次设计
 *
 * 本章要点：
 * 1: 公有继承、私有继承、保护继承
 * 2. 基类与派生类的关系
 * 3. 构造与析构顺序
 * 4. 成员遮蔽 (name hiding)
 * 5. 多重继承与菱形继承
 * 6. 继承中的访问控制
 */

#include <iostream>
#include <string>

using namespace std;

// ============================================================
// 5.1 基础继承: 图形层次
// ============================================================
// 基类
class Shape {
protected:
  string name;
  string color;

public:
  Shape(const string &name, const string &color = "黑色")
      : name(name), color(color) {
    cout << " [Shape 构造]: " << name << endl;
  }
  virtual ~Shape() { // 虚析构函数，确保派生类对象被正确销毁
    cout << " [Shape 析构]: " << name << endl;
  }

  string getName() const { return name; }
  string getColor() const { return color; }

  void describe() const {
    cout << " 这是一个: " << color << "的 " << name << endl;
  }
};
// 派生类
class Rectangle : public Shape { // 公有继承
protected:
  double width, height;

public:
  Rectangle(double w, double h, const string &color = "蓝色")
      : Shape("矩形", color), width(w), height(h) {
    cout << " [Rectangle 构造]: " << name << endl;
  }
  ~Rectangle() { cout << " [Rectangle 析构]: " << name << endl; }

  double area() const { return width * height; }
  double perimeter() const { return 2 * (width + height); }

  void printInfo() const {
    describe(); // 调用基类方法
    cout << " 尺寸: " << width << " x " << height << " 面积: " << area()
         << ", 周长: " << perimeter() << endl;
  }
};
class Circle : public Shape { // 公有继承
protected:
  double radius;

public:
  Circle(double r, const string &color = "红色")
      : Shape("圆形", color), radius(r) {
    cout << " [Circle 构造]: " << name << endl;
  }
  ~Circle() { cout << " [Circle 析构]: " << name << endl; }
  double area() const { return 3.14159 * radius * radius; }
  double perimeter() const { return 2 * 3.14159 * radius; }

  void printInfo() const {
    describe(); // 调用基类方法
    cout << " 半径: " << radius << " 面积: " << area()
         << ", 周长: " << perimeter() << endl;
  }
};

// 二级继承: 正方形是特殊的矩形
class Square : public Rectangle {
public:
  Square(double side, const string &color = "绿色")
      : Rectangle(side, side, color) {
    name = "正方形"; // 修改基类成员
    cout << " [Square 构造]: 正方形" << endl;
  }
  ~Square() { cout << " [Square 析构]: 正方形" << endl; }

  double side() const { return width; } // 访问继承的protected成员
};

// ============================================================
// 5.2 成员遮蔽与 using 声明
// ============================================================
class Base {
public:
  void show(int x) const { cout << "Base::show(int)" << endl; }
  void show(double x) const { cout << "Base::show(double)" << endl; }
};

class Derived : public Base {
public:
  // 屏蔽基类的show,派生类定义了同名函数，遮蔽了基类的所有重载版本
  void show(const string &x) const { cout << "Derived::show(string)" << endl; }

  using Base::show; // 取消遮蔽，允许访问基类的重载版本
  // 现在Derived有三个show函数: show(int), show(double), show(string)
  // 如果不加using Base::show;
  // 则只能访问Derived::show(string)，Base的两个重载版本都被隐藏了
};

// ============================================================
// 5.3 多重继承与菱形继承
// ============================================================
class Printable {
public:
  // virtual函数，允许派生类重写
  virtual void print() const { cout << " [Printable] " << endl; }
  virtual ~Printable() = default;
};

class Serializable {
public:
  virtual string serialize() const { return "[Serializable]"; }
  virtual ~Serializable() = default;
};

// 多重继承: 同时继承 Printable 和 Serializable
class Document : public Printable, public Serializable {
  string content;

public:
  Document(const string &text) : content(text) {}
  // override, 重写基类的虚函数
  void print() const override { cout << " 文档内容: " << content << endl; }
  // override, 重写基类的虚函数
  string serialize() const override {
    return "{\"序列化文档\": \"" + content + "\"}";
  }
};

// 菱形继承: A -> B, A -> C, B -> D, C -> D
class Animal {
public:
  int age;
  void breathe() const { cout << "  呼吸..." << endl; }
  virtual ~Animal() = default;
};

// 虚函数解决菱形问题
class Mammal : virtual public Animal {
public:
  void nurse() { cout << "  哺乳..." << endl; }
};

class WingedAnimal : virtual public Animal {
public:
  void flap() { cout << "  拍动翅膀..." << endl; }
};

// 蝙蝠: 既是哺乳动物又是有翼动物
class Bat : public Mammal, public WingedAnimal {
public:
  void describe() {
    cout << " 蝙蝠, 年龄: " << age << endl; // 虚继承后只有一个 age
    breathe();                              // 调用Animal的成员函数
    nurse();                                // 调用Mammal的成员函数
    flap();                                 // 调用WingedAnimal的成员函数
  }
};

// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 5.1 继承层次与构造析构顺序 =============" << endl;
  cout << "\n --- 创建矩形 --- " << endl;
  Rectangle rect(5, 3, "蓝色");
  rect.printInfo();

  cout << "\n --- 创建正方形 --- " << endl;
  Square sq(4, "黄色");
  sq.printInfo();

  cout << "\n --- 创建圆形 --- " << endl;
  Circle circ(2, "红色");
  circ.printInfo();

  cout << "\n --- 析构顺序 (与构造相反) --- " << endl;
  // 离开作用域，rect, sq, circ 会被销毁，析构函数会按照从派生类到基类的顺序调用

  cout << "\n=========== 5.2 成员遮蔽 =============" << endl;
  Derived d;
  d.show(string("Hello")); // 调用Derived::show(string)
  d.show(42);              // 调用Base::show(int)
  d.show(3.14);            // 调用Base::show(double)
  cout << endl;

  cout << "=========== 5.3 多重继承 =============" << endl;
  Document doc("这是一个文档");
  doc.print();                     // 调用Document::print()
  cout << doc.serialize() << endl; // 调用Document::serialize()

  // 通过不同基类指针访问
  Printable *p = &doc;
  p->print(); // 调用Document::print()

  Serializable *s = &doc;
  cout << s->serialize() << endl; // 调用Document::serialize()

  cout << "\n --- 菱形继承 --- " << endl;
  Bat bat;
  bat.age = 5;    // 只有一个age成员，虚继承解决了菱形继承的问题
  bat.describe(); // 调用Bat的成员函数，展示了从Animal, Mammal,
                  // WingedAnimal继承的功能

  return 0;
}
