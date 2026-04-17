/**
 * 第六章：多态性 -- 同一接口，不同行为
 *
 * 本章要点：
 * 1: 虚函数与动态绑定
 * 2: override与final关键字
 * 3: 纯虚函数与抽象类
 * 4: 接口设计
 * 5: 运行时类型识别 (RTTI) 与 dynamic_cast
 * 6: 虚函数表(vtable) 原理
 */

#include <cstring>
#include <iostream>
#include <memory>
#include <ratio>
#include <string>

using namespace std;

// ============================================================
// 6.1. 虚函数与动态绑定
// ============================================================
class Animal {
protected:
  string name;

public:
  Animal(const string &name) : name(name) {}
  virtual ~Animal() = default; // 虚析构函数, 至关重要！！！

  // 虚函数: 派生类可覆盖
  virtual void speak() const { cout << "  " << name << "  发次声音." << endl; }
  virtual string getType() const { return "动物"; }
  // 非虚函数: 不会动态绑定
  void info() const {
    cout << "  这是一只 " << getType() << "，名字叫 " << name << "." << endl;
  }
};

class Dog : public Animal {
public:
  Dog(const string &name) : Animal(name) {}
  // override: 可选，但推荐，编译器会检查是否正确覆盖了基类的虚函数
  void speak() const override { cout << "  " << name << "  汪汪叫." << endl; }
  string getType() const override { return "狗"; }

  void fetch() const { cout << "  " << name << "  正在捡球." << endl; }
};

class Cat : public Animal {
public:
  Cat(const string &name) : Animal(name) {}
  void speak() const override { cout << "  " << name << "  喵喵叫." << endl; }
  string getType() const override { return "猫"; }

  void climb() const { cout << "  " << name << "  正在爬树." << endl; }
};

class Duck : public Animal {
public:
  Duck(const string &name) : Animal(name) {}
  void speak() const override { cout << "  " << name << "  嘎嘎叫." << endl; }
  string getType() const override { return "鸭子"; }
};

// final 类: 不能被继承
class RobotDog final : public Dog {
public:
  RobotDog(const string &name) : Dog(name) {}
  void speak() const override {
    cout << "  " << name << "  [电子音]汪汪." << endl;
  }
  string getType() const override { return "机器狗"; }
};

// class FuturisticDog : public RobotDog { // 错误: RobotDog 是 final
// 类，不能被继承

// ============================================================
// 6.2 纯虚函数与抽象类 (接口)
// ============================================================
// 抽象类: 包含纯虚函数，不能实例化
class Shape {
public:
  virtual ~Shape() = default;

  // 纯虚函数: 必须在派生类中实现
  virtual double area() const = 0;
  virtual double perimeter() const = 0;
  virtual string name() const = 0;

  // 非纯虚函数： 有默认实现，派生类可以覆盖
  virtual void print() const {
    cout << "  这是一个 " << name() << "，面积: " << area()
         << ", 周长: " << perimeter() << "." << endl;
  }
};

class CircleShape : public Shape {
  double radius;

public:
  CircleShape(double r) : radius(r) {}

  double area() const override { return 3.14159 * radius * radius; }

  double perimeter() const override { return 2 * 3.14159 * radius; }

  string name() const override { return "圆形(r=" + to_string(radius) + ")"; }
};

class RectangleShape : public Shape {
  double width, height;

public:
  RectangleShape(double w, double h) : width(w), height(h) {}

  double area() const override { return width * height; }

  double perimeter() const override { return 2 * (width + height); }

  string name() const override {
    return "矩形(w=" + to_string(width) + ", h=" + to_string(height) + ")";
  }
};

class TriangleShape : public Shape {
  double a, b, c;

public:
  TriangleShape(double sideA, double sideB, double sideC)
      : a(sideA), b(sideB), c(sideC) {}

  double area() const override {
    double s = (a + b + c) / 2;                   // 半周长
    return sqrt(s * (s - a) * (s - b) * (s - c)); // 海伦公式
  }

  double perimeter() const override { return a + b + c; }

  string name() const override {
    return "三角形(a=" + to_string(a) + ", b=" + to_string(b) +
           ", c=" + to_string(c) + ")";
  }
};

// ============================================================
// 6.3 接口设计：可绘制的可序列化接口
// ============================================================
class IODrawable {
public:
  virtual ~IODrawable() = default;
  virtual void draw() const = 0;
};

class ISerializable {
public:
  virtual ~ISerializable() = default;
  virtual string toJson() const = 0;
  virtual string fromJson(const string &json) = 0;
};

// 实现多个接口
class Button : public IODrawable, public ISerializable {
  string label;
  int x, y;

public:
  Button(const string &label, int x = 0, int y = 0)
      : label(label), x(x), y(y) {}

  void draw() const override {
    cout << "  [按钮] " << label << " (位置: " << x << ", " << y << ")" << endl;
  }

  string toJson() const override {
    return "{\"type\": \"Button\", \"label\": \"" + label +
           "\", \"x\": " + to_string(x) + ", \"y\": " + to_string(y) + "}";
  }

  string fromJson(const string &json) override {
    // 简单解析示例，实际应用中应使用 JSON 库
    size_t start = json.find("\"label\": \"") + 10;
    size_t end = json.find("\"", start);
    if (start != string::npos && end != string::npos) {
      label = json.substr(start, end - start);
      return "成功解析 JSON.";
    }
    return "解析 JSON 失败.";
  }
};

// ============================================================
// 6.4 多态的实际应用: 图形计算器
// ============================================================
void printAllShapes(const vector<unique_ptr<Shape>> &shapes) {
  cout << " --- 所有图形 ---" << endl;
  double totalArea = 0;
  for (const auto &shape : shapes) {
    shape->print();
    totalArea += shape->area();
  }
  cout << "总面积: " << totalArea << endl;
}

// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 6.1 虚函数与动态绑定 =============" << endl;
  // 基类指针指向派生类对象，调用虚函数实现动态绑定
  vector<unique_ptr<Animal>> zoo;
  zoo.push_back(make_unique<Dog>("旺财"));
  zoo.push_back(make_unique<Cat>("咪咪"));
  zoo.push_back(make_unique<Duck>("嘎嘎"));
  zoo.push_back(make_unique<RobotDog>("铁蛋"));

  // 多态调用：运行时决定调用哪个版本的 speak() 和 getType()
  for (const auto &animal : zoo) {
    animal->info();  // 非虚函数：根据指针类型调用
    animal->speak(); // 虚函数：根据实际对象类型调用
  }

  // dynamic_cast: 安全地向下转换
  cout << "\n --- dynamic_cast 示例 ---" << endl;
  Animal *animal = zoo[0].get(); // 指向 Dog 对象
  if (Dog *d = dynamic_cast<Dog *>(animal)) {
    cout << "这是一只狗！可以玩接球." << endl;
    d->fetch(); // 成功转换，调用 Dog 特有的方法
  }

  Animal *catPtr = zoo[1].get(); // 指向 Cat 对象
  if (Dog *d = dynamic_cast<Dog *>(catPtr)) {
    cout << "这是一只狗！可以玩接球."
         << endl; // 不会执行，因为 catPtr 实际上指向的是 Cat 对象
    d->fetch();
  } else {
    cout << "这不是一只狗，无法玩接球." << endl;
  }
  cout << endl;

  cout << "=========== 6.2 纯虚函数与图形系统 =============" << endl;
  vector<unique_ptr<Shape>> shapes;
  shapes.push_back(make_unique<CircleShape>(5.0));
  shapes.push_back(make_unique<RectangleShape>(4.0, 6.0));
  shapes.push_back(make_unique<TriangleShape>(3.0, 4.0, 5.0));
  printAllShapes(shapes);
  cout << endl;

  cout << "=========== 6.3 接口设计 =============" << endl;
  Button myButton("提交", 100, 200);
  myButton.draw();
  string json = myButton.toJson();
  cout << "按钮的 JSON 表示: " << json << endl;
  string parseResult = myButton.fromJson(
      "{\"type\": \"Button\", \"label\": \"取消\", \"x\": 150, \"y\": 250}");
  cout << parseResult << endl;
  myButton.draw();

  // 通过接口指针操作
  IODrawable *drawable = &myButton;
  drawable->draw();

  ISerializable *serializable = &myButton;
  cout << "序列化为 JSON: " << serializable->toJson() << endl;

  return 0;
}
