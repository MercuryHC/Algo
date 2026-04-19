# 第六章：多态性 -- 同一接口，不同行为

> 本章深入讲解运行时多态的核心机制：虚函数、动态绑定、override/final、纯虚函数、抽象类、接口设计和 RTTI。

**源码位置**: `ch06_polymorphism/src/main.cpp`

---

## 6.1 虚函数与动态绑定

多态是面向对象的核心特征之一：通过基类指针/引用调用方法时，**实际执行的是派生类的版本**。

### 基类与派生类

```cpp
class Animal {
protected:
  string name;
public:
  Animal(const string &name) : name(name) {}
  virtual ~Animal() = default;              // 虚析构: 至关重要！

  virtual void speak() const {              // 虚函数: 派生类可覆盖
    cout << name << "发出声音." << endl;
  }
  virtual string getType() const {          // 虚函数
    return "动物";
  }

  void info() const {                       // 非虚函数: 不会动态绑定
    cout << "这是一只 " << getType() << "，名字叫 " << name << endl;
  }
};

class Dog : public Animal {
public:
  Dog(const string &name) : Animal(name) {}
  void speak() const override {             // override: 显式标注覆盖
    cout << name << "汪汪叫." << endl;
  }
  string getType() const override { return "狗"; }
  void fetch() const { cout << name << "正在捡球." << endl; }  // Dog 特有方法
};

class Cat : public Animal { ... };
class Duck : public Animal { ... };
```

### 动态绑定的运作方式

```cpp
vector<unique_ptr<Animal>> zoo;
zoo.push_back(make_unique<Dog>("旺财"));
zoo.push_back(make_unique<Cat>("咪咪"));
zoo.push_back(make_unique<Duck>("嘎嘎"));

for (const auto &animal : zoo) {
  animal->speak();    // 动态绑定: 运行时根据实际对象类型调用对应版本
}
// 输出:
// 旺财汪汪叫.
// 咪咪喵喵叫.
// 嘎嘎嘎嘎叫.
```

### 虚函数表 (vtable) 原理

编译器为每个含虚函数的类创建一个**虚函数表**（vtable），每个对象存储一个指向 vtable 的指针（vptr）：

```
Animal 对象:          Dog 对象:              Cat 对象:
[vptr] ──→ vtable    [vptr] ──→ vtable      [vptr] ──→ vtable
           │ speak  → Animal::speak    │ speak  → Dog::speak      │ speak  → Cat::speak
           │ getType→ Animal::getType  │ getType→ Dog::getType    │ getType→ Cat::getType
```

- `animal->speak()` 通过 vptr 查表，在运行时决定调用哪个函数
- 非虚函数不经过 vtable，直接根据指针类型静态绑定

### override 与 final

```cpp
class Dog : public Animal {
public:
  void speak() const override { ... }     // override: 编译器检查是否正确覆盖基类虚函数
  // void spek() const override;          // 错误! 拼写错误，编译器报错（无此虚函数）
};

class RobotDog final : public Dog {        // final: 此类不可被继承
public:
  void speak() const override final { ... } // final: 此函数不可被覆盖
};

// class FuturisticDog : public RobotDog {}  // 错误! RobotDog 是 final 类
```

| 关键字 | 作用 | 位置 |
|--------|------|------|
| `override` | 告诉编译器检查是否正确覆盖了基类虚函数 | 派生类函数声明末尾 |
| `final` (函数) | 禁止进一步覆盖该虚函数 | 派生类函数声明末尾 |
| `final` (类) | 禁止该类被继承 | 类声明中类名后 |

> **最佳实践**: 所有覆盖的虚函数都应标注 `override`，利用编译器捕获拼写错误和签名不匹配。

---

## 6.2 纯虚函数与抽象类

### 纯虚函数

在虚函数声明后加 `= 0`，表示该函数**没有默认实现**，派生类**必须**实现：

```cpp
class Shape {
public:
  virtual ~Shape() = default;

  virtual double area() const = 0;          // 纯虚函数
  virtual double perimeter() const = 0;     // 纯虚函数
  virtual string name() const = 0;          // 纯虚函数

  virtual void print() const {              // 虚函数（有默认实现）
    cout << "这是一个 " << name() << "，面积: " << area()
         << "，周长: " << perimeter() << endl;
  }
};
```

### 抽象类

包含至少一个纯虚函数的类称为**抽象类**，不能被实例化：

```cpp
// Shape s;                                  // 错误! 抽象类不可实例化
// Shape s = Shape();                        // 错误!

vector<unique_ptr<Shape>> shapes;           // OK: 可以声明指向抽象类的指针/引用
shapes.push_back(make_unique<CircleShape>(5.0));
```

### 具体派生类

派生类必须实现**所有**纯虚函数才能成为具体类（可实例化）：

```cpp
class CircleShape : public Shape {
  double radius;
public:
  CircleShape(double r) : radius(r) {}

  double area() const override { return 3.14159 * radius * radius; }
  double perimeter() const override { return 2 * 3.14159 * radius; }
  string name() const override { return "圆形(r=" + to_string(radius) + ")"; }
  // print() 不需要覆盖，使用 Shape 的默认实现
};

class RectangleShape : public Shape { ... };
class TriangleShape : public Shape { ... };
```

### 多态的实际应用

```cpp
void printAllShapes(const vector<unique_ptr<Shape>> &shapes) {
  double totalArea = 0;
  for (const auto &shape : shapes) {
    shape->print();              // 多态: 每种图形输出自身信息
    totalArea += shape->area();  // 多态: 各自计算面积
  }
  cout << "总面积: " << totalArea << endl;
}
```

---

## 6.3 接口设计

在 C++ 中，接口通过**全是纯虚函数**（且有一个虚析构函数）的类来实现，类似 Java/C# 的 interface：

```cpp
// 绘制接口
class IDrawable {
public:
  virtual ~IDrawable() = default;
  virtual void draw() const = 0;
};

// 序列化接口
class ISerializable {
public:
  virtual ~ISerializable() = default;
  virtual string toJson() const = 0;
  virtual string fromJson(const string &json) = 0;
};

// 同时实现多个接口
class Button : public IDrawable, public ISerializable {
  string label;
  int x, y;
public:
  Button(const string &label, int x = 0, int y = 0)
      : label(label), x(x), y(y) {}

  void draw() const override {
    cout << "[按钮] " << label << " (位置: " << x << ", " << y << ")" << endl;
  }

  string toJson() const override {
    return "{\"type\": \"Button\", \"label\": \"" + label + "\"}";
  }

  string fromJson(const string &json) override { ... }
};
```

**通过接口指针操作**:

```cpp
Button myButton("提交", 100, 200);

IDrawable *drawable = &myButton;
drawable->draw();                        // 多态调用

ISerializable *serializable = &myButton;
cout << serializable->toJson();          // 多态调用
```

---

## 6.4 RTTI 与 dynamic_cast

运行时类型识别（RTTI）允许在运行时安全地进行向下类型转换：

```cpp
Animal *animal = zoo[0].get();          // 实际指向 Dog 对象

// dynamic_cast: 安全的向下转换
if (Dog *d = dynamic_cast<Dog*>(animal)) {
  cout << "这是一只狗！" << endl;
  d->fetch();                           // 安全调用 Dog 特有方法
}

Animal *catPtr = zoo[1].get();          // 实际指向 Cat 对象
if (Dog *d = dynamic_cast<Dog*>(catPtr)) {
  d->fetch();                           // 不会执行，转换失败返回 nullptr
} else {
  cout << "这不是一只狗。" << endl;
}
```

### RTTI 的两种形式

| 操作 | 说明 |
|------|------|
| `dynamic_cast<T*>(ptr)` | 安全向下转换，失败返回 `nullptr` |
| `dynamic_cast<T&>(ref)` | 安全向下转换，失败抛出 `bad_cast` |
| `typeid(obj).name()` | 返回类型的实现相关名称 |

### dynamic_cast 的前提条件

- 类必须有**虚函数**（通常有虚析构函数即可）
- 转换是在继承层次内的指针或引用之间进行
- 有性能开销（需要查询 vtable 中的类型信息）

---

## 虚函数 vs 非虚函数调用对比

```cpp
class Animal {
public:
  virtual void speak() const { cout << "动物" << endl; }   // 虚函数
  void info() const { cout << getType() << endl; }          // 非虚函数
};

Animal *a = new Dog("旺财");
a->speak();    // 虚函数: 调用 Dog::speak() → "旺财汪汪叫"（动态绑定）
a->info();     // 非虚函数: 调用 Animal::info()（静态绑定，但内部调用虚函数 getType 则动态绑定）
```

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 虚函数 | `virtual` 声明，通过基类指针/引用实现动态绑定 |
| vtable | 编译器为含虚函数的类生成的函数指针表 |
| override | 建议所有覆盖标注 `override`，让编译器检查 |
| final | 禁止类被继承或函数被覆盖 |
| 纯虚函数 | `= 0`，无默认实现，派生类必须实现 |
| 抽象类 | 含纯虚函数，不可实例化 |
| 接口 | 全是纯虚函数的类，C++ 通过多重继承实现多接口 |
| dynamic_cast | 安全的运行时向下转换，需要虚函数支持 |
