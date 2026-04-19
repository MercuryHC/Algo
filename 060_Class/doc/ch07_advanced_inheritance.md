# 第七章：高级继承模式 -- 设计模式中的类技巧

> 本章介绍如何利用继承和模板实现经典设计模式，包括模板方法、策略模式、CRTP 和 Mixin 模式。

**源码位置**: `ch07_advanced_inheritance/src/main.cpp`

---

## 7.1 模板方法模式 (Template Method)

**核心思想**: 基类定义算法的固定骨架（模板方法），子类实现其中可变的步骤。

### 类结构

```
DataProcessor (基类)
├── process()           ← 模板方法: 定义固定流程，不可覆盖
├── loadData()          ← 纯虚函数: 子类必须实现
├── validate()          ← 虚函数: 有默认实现，子类可选覆盖
├── transform()         ← 纯虚函数: 子类必须实现
└── saveData()          ← 虚函数: 有默认实现，子类可选覆盖

CSVProcessor (派生类)
├── loadData()          ← 实现: 从 CSV 加载
├── validate()          ← 覆盖: 检查分隔符和引号
└── transform()         ← 实现: 转换 CSV 格式

JsonProcessor (派生类)
├── loadData()          ← 实现: 从 JSON 加载
├── transform()         ← 实现: 转换 JSON 格式
└── saveData()          ← 覆盖: 保存为 JSON
```

### 代码实现

```cpp
class DataProcessor {
public:
  // 模板方法: 定义算法流程 (非虚函数，不可覆盖)
  void process() {
    loadData();       // 步骤1: 加载数据
    validate();       // 步骤2: 验证数据
    transform();      // 步骤3: 转换数据
    saveData();       // 步骤4: 保存数据
    cout << "处理完成！" << endl;
  }

  virtual ~DataProcessor() = default;

protected:
  // 纯虚函数: 子类必须实现
  virtual void loadData() = 0;
  virtual void transform() = 0;

  // 虚函数: 有默认实现，子类可选覆盖
  virtual void validate() {
    cout << "[默认验证] 数据格式检查通过." << endl;
  }
  virtual void saveData() {
    cout << "[默认保存] 数据已保存到数据库." << endl;
  }
};

class CSVProcessor : public DataProcessor {
protected:
  void loadData() override { cout << "[CSV] 从 CSV 文件加载数据." << endl; }
  void transform() override { cout << "[CSV] 转换 CSV 数据为内部格式." << endl; }
  void validate() override { cout << "[CSV] 验证 CSV 数据，检查分隔符和引号." << endl; }
};

class JsonProcessor : public DataProcessor {
protected:
  void loadData() override { cout << "[JSON] 从 JSON 文件加载数据." << endl; }
  void transform() override { cout << "[JSON] 转换 JSON 数据为内部格式." << endl; }
  void saveData() override { cout << "[JSON] 将数据保存为 JSON 文件." << endl; }
};
```

**使用**:

```cpp
CSVProcessor csvProcessor;
csvProcessor.process();
// 输出:
// [CSV] 从 CSV 文件加载数据.
// [CSV] 验证 CSV 数据，检查分隔符和引号.
// [CSV] 转换 CSV 数据为内部格式.
// [默认保存] 数据已保存到数据库.
// 处理完成！
```

### 设计要点

- 模板方法 `process()` 是 `public` 非虚函数 -- 外部调用，内部不可变
- 算法步骤是 `protected` 虚函数 -- 子类实现/覆盖，外部不可见
- 纯虚函数（`= 0`）强制子类必须实现
- 有默认实现的虚函数提供可选的扩展点

---

## 7.2 策略模式 (Strategy)

**核心思想**: 定义一系列算法，将每个算法封装起来，并使它们可以互换。策略模式让算法的变化独立于使用它的客户端。

### 类结构

```
SortStrategy (策略接口)
├── BubbleSort     (具体策略: 冒泡排序)
├── QuickSort      (具体策略: 快速排序)

Sorter (上下文)
├── strategy       (持有策略指针)
├── setStrategy()  (运行时切换策略)
└── sort()         (委托给策略执行)
```

### 代码实现

```cpp
// 策略接口
class SortStrategy {
public:
  virtual ~SortStrategy() = default;
  virtual void sort(vector<int> &data) const = 0;
  virtual string name() const = 0;
};

// 具体策略: 冒泡排序
class BubbleSort : public SortStrategy {
public:
  void sort(vector<int> &data) const override {
    int n = data.size();
    for (int i = 0; i < n - 1; ++i)
      for (int j = 0; j < n - i - 1; ++j)
        if (data[j] > data[j + 1])
          swap(data[j], data[j + 1]);
  }
  string name() const override { return "冒泡排序"; }
};

// 具体策略: 快速排序
class QuickSort : public SortStrategy { ... };

// 上下文: 使用策略
class Sorter {
  const SortStrategy *strategy;
public:
  Sorter(const SortStrategy *s) : strategy(s) {}

  void setStrategy(const SortStrategy *newStrategy) {
    strategy = newStrategy;              // 运行时切换算法
  }

  void sort(vector<int> &data) const {
    strategy->sort(data);                // 委托给策略执行
  }
};
```

**使用**:

```cpp
vector<int> data = {5, 2, 9, 1, 7, 3};
BubbleSort bubble;
QuickSort quick;

Sorter sorter(&bubble);
sorter.sort(data);                     // 使用冒泡排序
sorter.setStrategy(&quick);
sorter.sort(data);                     // 切换为快速排序
```

### 模板方法 vs 策略模式

| 特性 | 模板方法 | 策略模式 |
|------|---------|---------|
| 实现方式 | 继承 | 组合 |
| 算法切换 | 需要更换子类 | 运行时切换策略对象 |
| 扩展性 | 新增子类 | 新增策略类 |
| 耦合度 | 较高（继承） | 较低（组合） |

---

## 7.3 CRTP (Curiously Recurring Template Pattern)

**核心思想**: 利用模板在**编译期**实现多态，避免虚函数的运行时开销。

### 用途一：编译期计数器

```cpp
template <typename Derived>
class CounterCRTP {
  static int count;                    // 每个派生类有独立的 static 变量
public:
  CounterCRTP() { ++count; }
  CounterCRTP(const CounterCRTP &) { ++count; }
  ~CounterCRTP() { --count; }
  static int getCount() { return count; }
};

template <typename Derived>
int CounterCRTP<Derived>::count = 0;

// 使用: 将自身作为模板参数传给基类
class Widget : public CounterCRTP<Widget> { ... };   // Widget 有独立的 count
class Gadget : public CounterCRTP<Gadget> { ... };   // Gadget 有独立的 count
```

**原理**: `CounterCRTP<Widget>` 和 `CounterCRTP<Gadget>` 是两个完全不同的类模板实例化，各自有独立的 `static count`。

```cpp
{
  Widget w1("按钮"), w2("文本框");
  Gadget g1;
  CounterCRTP<Widget>::getCount();    // 2
  CounterCRTP<Gadget>::getCount();    // 1
}
CounterCRTP<Widget>::getCount();      // 0 (已析构)
```

### 用途二：编译期多态接口

```cpp
template <typename Derived>
class PrintableCRTP {
public:
  void print() const {
    // 编译期绑定: static_cast 将 this 转为 Derived*
    cout << static_cast<const Derived*>(this)->toString() << endl;
  }
};

class Person : public PrintableCRTP<Person> {
  string name; int age;
public:
  Person(const string &name, int age) : name(name), age(age) {}
  string toString() const {
    return "Person(name: " + name + ", age: " + to_string(age) + ")";
  }
};

Person p("张三", 30);
p.print();    // 编译期确定调用 Person::toString()
```

### CRTP vs 虚函数多态

| 特性 | 虚函数 | CRTP |
|------|--------|------|
| 绑定时机 | 运行时 | 编译期 |
| 运行开销 | vtable 查询 | 零开销 |
| 代码膨胀 | 无 | 有（每个类型生成一份代码） |
| 灵活性 | 高（运行时多态） | 低（编译期确定） |

---

## 7.4 Mixin 模式

**核心思想**: 通过模板继承，像"混入"一样组合多个功能层。

```cpp
// 基础类
class Message {
  string content;
public:
  Message(const string &content) : content(content) {}
  void print() const { cout << "消息内容: " << content << endl; }
};

// Mixin 层1: 添加时间戳功能
template <typename Base>
class Timestamped : public Base {
  string timestamp;
public:
  template <typename... Args>
  Timestamped(const string &ts, Args &&...args)
      : Base(std::forward<Args>(args)...), timestamp(ts) {}

  string getTimestamp() const { return timestamp; }
  void printWithTimestamp() const {
    Base::print();
    cout << "时间戳: " << timestamp << endl;
  }
};

// Mixin 层2: 添加标签功能
template <typename Base>
class Tagged : public Base {
  vector<string> tags;
public:
  template <typename... Args>
  Tagged(const vector<string> &tg, Args &&...args)
      : Base(std::forward<Args>(args)...), tags(tg) {}

  bool hasTag(const string &tag) const {
    return find(tags.begin(), tags.end(), tag) != tags.end();
  }
  void printTags() const {
    Base::print();
    cout << "标签: ";
    for (const auto &t : tags) cout << t << " ";
    cout << endl;
  }
};

// 组合: 像洋葱一样逐层包裹
using TaggedMessage = Tagged<Timestamped<Message>>;
```

**使用**:

```cpp
// 构造顺序: 从外到内传参
// Tagged(tags, timestamp, content)
TaggedMessage msg({"重要", "待办"}, "2026-04-19 10:00", "这是一个重要的待办消息。");
msg.printTags();              // 消息内容 + 标签
msg.printWithTimestamp();     // 消息内容 + 时间戳
msg.hasTag("重要");            // true
```

### Mixin 的优势

- **自由组合**: 可以任意搭配不同的功能层
- **避免钻石继承**: 每个功能层是独立的模板，不存在多基类问题
- **编译期确定**: 不依赖虚函数，零运行开销

---

## 本章小结

| 模式 | 核心机制 | 使用场景 |
|------|---------|---------|
| 模板方法 | 继承 + 虚函数 | 固定算法骨架，变化步骤由子类实现 |
| 策略模式 | 组合 + 多态 | 运行时切换算法/行为 |
| CRTP | 模板 + 继承 | 编译期多态，零开销，类型安全 |
| Mixin | 模板 + 多重继承 | 自由组合功能层 |

### 设计原则

- **继承 vs 组合**: 优先使用组合（策略模式），继承用于 "is-a" 关系
- **编译期 vs 运行时**: 性能敏感场景使用 CRTP，灵活场景使用虚函数
- **开放封闭原则**: 对扩展开放（新增子类/策略），对修改封闭（不改已有代码）
