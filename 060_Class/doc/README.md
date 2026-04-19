# C++ 类与面向对象编程 -- 教程文档

> 本教程循序渐进地讲解 C++ 类、继承、多态、模板等核心概念，从基础到高级，共 10 章。

## 目录

| 章节 | 主题 | 文档 |
|------|------|------|
| 第 1 章 | 类的基础 -- 从 struct 到 class | [ch01_basic_class.md](ch01_basic_class.md) |
| 第 2 章 | 构造函数与析构函数 -- 对象的生与死 | [ch02_constructor.md](ch02_constructor.md) |
| 第 3 章 | 封装与控制 -- 面向对象的核心原则 | [ch03_encapsulation.md](ch03_encapsulation.md) |
| 第 4 章 | 运算符重载 -- 让类像内置类型一样使用 | [ch04_operator_overload.md](ch04_operator_overload.md) |
| 第 5 章 | 继承 -- 代码复用与层次设计 | [ch05_inheritance.md](ch05_inheritance.md) |
| 第 6 章 | 多态性 -- 同一接口，不同行为 | [ch06_polymorphism.md](ch06_polymorphism.md) |
| 第 7 章 | 高级继承模式 -- 设计模式中的类技巧 | [ch07_advanced_inheritance.md](ch07_advanced_inheritance.md) |
| 第 8 章 | 模板类 -- 泛型编程的基础 | [ch08_template_class.md](ch08_template_class.md) |
| 第 9 章 | 模板元编程 -- 编译期计算 | [ch09_advanced_template.md](ch09_advanced_template.md) |
| 第 10 章 | 实战项目 | [ch10_export_patterns.md](ch10_export_patterns.md) |

## 知识体系路线图

```
基础
  Ch1 类定义(struct/class/this/const)
   └─→ Ch2 对象生命周期(构造/析构/拷贝/移动)
        └─→ Ch3 封装(访问控制/友元/静态成员)

进阶
  Ch4 运算符重载(算术/比较/流/下标/仿函数)
   └─→ Ch5 继承(单继承/多重继承/虚继承)
        └─→ Ch6 多态(虚函数/抽象类/接口/RTTI)

高级
  Ch7 设计模式(模板方法/策略/CRTP/Mixin)
   └─→ Ch8 模板类(类模板/特化/变参模板)
        └─→ Ch9 模板元编程(编译期计算/SFINAE/Concepts)

实战
  Ch10 综合项目(ECS框架/状态机/最佳实践)
```

## 构建与运行

每个章节为独立的 CMake 项目，构建方式一致：

```bash
cd chXX_xxxx
mkdir -p build && cd build
cmake ..
cmake --build .
./bin/chXX_xxxx_main
```
