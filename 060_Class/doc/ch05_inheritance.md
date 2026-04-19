# 第五章：继承 -- 代码复用与层次设计

> 本章讲解 C++ 继承机制，包括公有/私有/保护继承、构造析构顺序、成员遮蔽、多重继承和虚继承解决菱形问题。

**源码位置**: `ch05_inheritance/src/main.cpp`

---

## 5.1 基础继承：图形层次

继承允许在已有类（基类）的基础上创建新类（派生类），实现代码复用和层次化设计。

### 基类 Shape

```cpp
class Shape {
protected:                          // protected: 派生类可访问，外部不可访问
  string name;
  string color;

public:
  Shape(const string &name, const string &color = "黑色")
      : name(name), color(color) {
    cout << " [Shape 构造]: " << name << endl;
  }

  virtual ~Shape() {                // 虚析构函数: 确保通过基类指针删除时正确调用派生类析构
    cout << " [Shape 析构]: " << name << endl;
  }

  void describe() const {
    cout << "这是一个 " << color << "的 " << name << endl;
  }
};
```

### 派生类 Rectangle

```cpp
class Rectangle : public Shape {    // 公有继承
protected:
  double width, height;

public:
  Rectangle(double w, double h, const string &color = "蓝色")
      : Shape("矩形", color), width(w), height(h) {   // 必须在初始化列表中调用基类构造
    cout << " [Rectangle 构造]" << endl;
  }

  ~Rectangle() {
    cout << " [Rectangle 析构]" << endl;
  }

  double area() const { return width * height; }

  void printInfo() const {
    describe();    // 调用基类方法
    cout << " 尺寸: " << width << " x " << height << " 面积: " << area() << endl;
  }
};
```

### 二级继承 Square

```cpp
class Square : public Rectangle {   // 继承自 Rectangle
public:
  Square(double side, const string &color = "绿色")
      : Rectangle(side, side, color) {    // 调用 Rectangle 的构造函数
    name = "正方形";                       // 修改 protected 基类成员
  }

  double side() const { return width; }   // 访问继承的 protected 成员
};
```

### 构造与析构顺序

创建 `Square sq(4, "黄色")` 时的调用链：

```
构造顺序（基类 → 派生类）:
  Shape 构造 → Rectangle 构造 → Square 构造

析构顺序（派生类 → 基类）:
  Square 析构 → Rectangle 析构 → Shape 析构
```

> **规则**: 构造从最底层基类开始，析构从最外层派生类开始，两者顺序严格相反。

### 三种继承方式

| 继承方式 | 基类 `public` | 基类 `protected` | 基类 `private` |
|---------|-------------|-----------------|---------------|
| `public` 继承 | 仍为 `public` | 仍为 `protected` | 不可访问 |
| `protected` 继承 | 变为 `protected` | 仍为 `protected` | 不可访问 |
| `private` 继承 | 变为 `private` | 变为 `private` | 不可访问 |

> **实际开发中 95% 以上使用 `public` 继承**，它表达的是 "is-a"（是一个）关系。

### 虚析构函数的重要性

```cpp
Shape *p = new Rectangle(5, 3);
delete p;    // 如果 Shape 的析构函数不是 virtual，只调用 Shape::~Shape()
             // Rectangle 的析构函数不会被调用 → 资源泄漏！
```

> **规则**: 只要类可能被继承，就应该声明虚析构函数（即使函数体为空 `= default`）。

---

## 5.2 成员遮蔽与 using 声明

当派生类定义了与基类**同名**的函数时，会遮蔽基类的**所有**同名重载版本：

### 问题：名称遮蔽

```cpp
class Base {
public:
  void show(int x) const    { cout << "Base::show(int)" << endl; }
  void show(double x) const { cout << "Base::show(double)" << endl; }
};

class Derived : public Base {
public:
  void show(const string &x) const {
    cout << "Derived::show(string)" << endl;
  }
  // 此时 Base::show(int) 和 Base::show(double) 都被遮蔽！
};

Derived d;
d.show(string("Hello"));  // OK: Derived::show(string)
d.show(42);               // 错误! Base::show(int) 被遮蔽
d.show(3.14);             // 错误! Base::show(double) 被遮蔽
```

### 解决方案：using 声明

```cpp
class Derived : public Base {
public:
  void show(const string &x) const { ... }
  using Base::show;    // 将 Base 的所有 show 重载版本引入 Derived 作用域
};

Derived d;
d.show(string("Hello"));  // Derived::show(string)
d.show(42);               // Base::show(int)  ← now accessible
d.show(3.14);             // Base::show(double) ← now accessible
```

> **遮蔽 vs 覆盖(override)**: 遮蔽发生在同名不同签名的函数之间；覆盖是虚函数的重写机制。

---

## 5.3 多重继承与菱形继承

### 多重继承

一个类同时继承多个基类：

```cpp
class Printable {
public:
  virtual void print() const { cout << "[Printable]" << endl; }
  virtual ~Printable() = default;
};

class Serializable {
public:
  virtual string serialize() const { return "[Serializable]"; }
  virtual ~Serializable() = default;
};

class Document : public Printable, public Serializable {
  string content;
public:
  Document(const string &text) : content(text) {}
  void print() const override { cout << "文档内容: " << content << endl; }
  string serialize() const override {
    return "{\"文档\": \"" + content + "\"}";
  }
};
```

**通过不同基类指针访问**:

```cpp
Document doc("这是一个文档");

Printable *p = &doc;
p->print();                  // 多态: 调用 Document::print()

Serializable *s = &doc;
cout << s->serialize();      // 多态: 调用 Document::serialize()
```

### 菱形继承问题

当继承图形成菱形时，最底层派生类会有多份顶层基类的成员：

```
     Animal (有 age 成员)
      /    \
  Mammal   WingedAnimal   (各继承一份 age)
      \    /
       Bat               (有两份 age! 访问 age 有歧义)
```

**不使用虚继承的问题**:

```cpp
class Bat : public Mammal, public WingedAnimal {
  void describe() {
    // age = 5;           // 错误! 有歧义: Mammal::age 还是 WingedAnimal::age?
    Mammal::age = 5;      // 必须显式指定
    WingedAnimal::age = 5;
  }
};
```

### 解决方案：虚继承 (virtual)

```cpp
class Mammal : virtual public Animal { ... };      // 虚继承
class WingedAnimal : virtual public Animal { ... }; // 虚继承
class Bat : public Mammal, public WingedAnimal {
  void describe() {
    age = 5;     // OK! 只有唯一一份 age
    breathe();   // OK! 只有唯一一份 Animal 成员
  }
};
```

虚继承确保无论经过多少条继承路径，基类子对象只存在**一份**。

---

## 本章小结

| 概念 | 关键点 |
|------|--------|
| 公有继承 | "is-a" 关系，最常用 |
| 构造顺序 | 基类 → 派生类 |
| 析构顺序 | 派生类 → 基类（与构造相反） |
| 虚析构函数 | 通过基类指针删除时确保正确析构 |
| 成员遮蔽 | 派生类同名函数遮蔽基类所有重载 |
| using 声明 | 恢复被遮蔽的基类函数 |
| 多重继承 | 同时继承多个基类，需注意二义性 |
| 虚继承 | 解决菱形继承，保证基类子对象唯一 |
| `protected` | 派生类可访问，外部不可访问 |
