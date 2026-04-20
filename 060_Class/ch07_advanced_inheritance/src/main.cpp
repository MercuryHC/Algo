/**
 * 第七章：高级继承模式 -- 设计模式中的类技巧
 *
 * 本章要点：
 * 1: 模板方法模式 (template Method)
 * 2: 策略模式 (Strategy)
 * 3: 观察者模式 (Observer)
 * 4: CPTP (奇异模板参数模式)
 * 5: Mixin 模式
 */

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// ============================================================
// 7.1. 模板方法模式
// ============================================================
// 基类定义算法骨架， 子类实现具体步骤
class DataProcessor {
public:
  // 模板方法: 定义算法流程(不可覆盖)
  void process() {
    loadData();
    valiDate();
    transform();
    saveData();
    cout << " 处理完成！" << endl;
  }
  virtual ~DataProcessor() = default;

protected:
  // TODO： 定义纯虚函数，子类必须实现
  // 纯虚函数，子类必须实现
  virtual void loadData() = 0;
  virtual void transform() = 0;

  // 子类可以选择覆盖
  virtual void valiDate() { cout << " [默认验证] 数据格式检查通过." << endl; }

  virtual void saveData() { cout << " [默认保存] 数据已保存到数据库." << endl; }
};

class CSVProcessor : public DataProcessor {
protected:
  // 纯虚函数，必须实现
  void loadData() override { cout << " [CSV] 从 CSV 文件加载数据." << endl; }
  void transform() override {
    cout << " [CSV] 转换 CSV 数据为内部格式." << endl;
  }

  // 可选覆盖
  void valiDate() override {
    cout << " [CSV] 验证 CSV 数据，检查分隔符和引号." << endl;
  }
};

class JsonProcessor : public DataProcessor {
protected:
  // 纯虚函数，必须实现
  void loadData() override { cout << " [JSON] 从 JSON 文件加载数据." << endl; }
  void transform() override {
    cout << " [JSON] 转换 JSON 数据为内部格式." << endl;
  }

  // 可选覆盖
  void saveData() override {
    cout << " [JSON] 将数据保存为 JSON 文件." << endl;
  }
};

// ============================================================
// 7.2 策略模式: 定义一系列算法，封装起来，并使它们可互换
// ============================================================
// 排序策略接口
class SortStrategy {
public:
  virtual ~SortStrategy() = default;
  // 定义排序接口，子类实现具体算法
  // = 0: 纯虚函数，子类必须实现
  virtual void sort(vector<int> &data) const = 0;
  virtual string name() const = 0;
};
// 冒泡排序
class BubbleSort : public SortStrategy {
public:
  void sort(vector<int> &data) const override {
    cout << " 使用冒泡排序." << endl;
    int n = data.size();
    for (int i = 0; i < n - 1; ++i) {
      for (int j = 0; j < n - i - 1; ++j) {
        if (data[j] > data[j + 1]) {
          swap(data[j], data[j + 1]);
        }
      }
    }
  }
  string name() const override { return "冒泡排序"; }
};
// 快速排序
class QuickSort : public SortStrategy {
public:
  void sort(vector<int> &data) const override {
    cout << " 使用快速排序." << endl;
    quickSort(data, 0, data.size() - 1);
  }
  string name() const override { return "快速排序"; }

private:
  void quickSort(vector<int> &data, int low, int high) const {
    if (low < high) {
      int pi = partition(data, low, high);
      quickSort(data, low, pi - 1);
      quickSort(data, pi + 1, high);
    }
  }
  int partition(vector<int> &data, int low, int high) const {
    int pivot = data[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
      if (data[j] < pivot) {
        ++i;
        swap(data[i], data[j]);
      }
    }
    swap(data[i + 1], data[high]);
    return i + 1;
  }
};

// 上下文类: 使用策略
class Sorter {
  // 持有策略对象的指针，允许在运行时切换算法
  const SortStrategy *strategy;

public:
  Sorter(const SortStrategy *strategy) : strategy(strategy) {}

  void setStrategy(const SortStrategy *newStrategy) {
    strategy = newStrategy;
    cout << "切换排序策略: " << strategy->name() << endl;
  }

  void sort(vector<int> &data) const {
    cout << "使用排序策略: " << strategy->name() << endl;
    strategy->sort(data);
  }
};

// ============================================================================
// 7.3 CRTP (Curiously Recurring Template Pattern)
// ============================================================================
// 编译期多态：零运行开销
// 解释：CRTP 是一种 C++ 模板技巧，基类是一个模板，参数是子类自身类型
// 通过 CRTP，基类可以在编译期调用子类的方法，实现静态多态（编译期多态），
// 避免虚函数的运行时开销
// 适用场景：计数器、接口检查、静态多态等
// 注意：CRTP 不是传统意义上的继承，而是一种模板模式，基类和子类之间的关系在
// 编译期被解析
// ============================================================================
// 基类模板，Derived 是子类类型
// 在基类中使用 Derived 来访问子类的方法或成员，实现编译期多态
template <typename Derived> class CounterCPTR {
  static int count;

public:
  CounterCPTR() { ++count; }
  CounterCPTR(const CounterCPTR &) { ++count; }
  ~CounterCPTR() { --count; }

  static int getCount() { return count; }
};
template <typename Derived> int CounterCPTR<Derived>::count = 0;
// 具体类，继承自 CounterCPTR，传入自身类型作为模板参数
class Widget : public CounterCPTR<Widget> {
  string name;

public:
  Widget(const string &name) : name(name) {
    cout << "创建 Widget: " << name << endl;
  }
};
// 另一个具体类，继承自 CounterCPTR，传入自身类型作为模板参数
class Gadget : public CounterCPTR<Gadget> {
public:
  Gadget() { cout << "创建 Gadget" << endl; }
};

// CRTP 实现编译期接口检查
template <typename Derived> class PrintTableCRTP {
public:
  void print() const {
    // NOTE: 编译期绑定: 调用 Derived 类的 toString() 方法
    cout << static_cast<const Derived *>(this)->toString() << endl;
  }
};

class Person : public PrintTableCRTP<Person> {
  string name;
  int age;

public:
  Person(const string &name, int age) : name(name), age(age) {}
  string toString() const {
    return "Person(name: " + name + ", age: " + to_string(age) + ")";
  }
};

class Product : public PrintTableCRTP<Product> {
  string name;
  double price;

public:
  Product(const string &name, double price) : name(name), price(price) {}
  string toString() const {
    return "Product(name: " + name + ", price: " + to_string(price) + ")";
  }
};

// ============================================================
// 7.4 Mixin 模式: 通过多重继承组合功能
// ============================================================
// 功能类1: 提供时间戳功能
template <typename Base> class Timestamped : public Base {
  string timestamp;

public:
  template <typename... Args>
  Timestamped(const string &ts, Args &&...args)
      : Base(std::forward<Args>(args)...), timestamp(ts) {}

  string getTimestamp() const { return timestamp; }
  void printWithTimestamp() const {
    Base::print(); // 调用 Base 类的 print() 方法
    cout << "时间戳: " << timestamp << endl;
  }
};
// 功能类2: 提供标签功能
template <typename Base> class Tagged : public Base {
  vector<string> tags;

public:
  template <typename... Args>
  Tagged(const vector<string> &tg, Args &&...args)
      : Base(std::forward<Args>(args)...), tags(tg) {}

  void addTag(const string &tag) { tags.push_back(tag); }
  bool hasTag(const string &tag) const {
    return find(tags.begin(), tags.end(), tag) != tags.end();
  }
  void printTags() const {
    Base::print(); // 调用 Base 类的 print() 方法
    cout << "标签: ";
    for (const auto &tag : tags) {
      cout << tag << " ";
    }
    cout << endl;
  }
};

// 基础消息类
class Message {
  string content;

public:
  Message(const string &content) : content(content) {}
  void print() const { cout << "消息内容: " << content << endl; }
  string getContent() const { return content; }
};

// 组合 Timestamped 和 Tagged 功能
// 注意: TaggedMessage 需要按照顺序构造内层到外层
// Tagged<Timestamped<Message>> 先构造 Timestamped<Message>，再构造
// 构造顺序: Timestamped<Message> -> Tagged<Timestamped<Message>>
//
// 继承关系：
// TaggedMessage 继承自 Tagged<Timestamped<Message>>，同时具有时间戳和标签功能
// Timestamped<Message> 继承自 Message，具有时间戳功能
// Message 是基础类，提供消息内容和打印功能
// 功能组合
// Message 提供基本功能（内容和打印）
// Timestamped<Message> 在 Message 基础上添加时间戳功能
// Tagged<Timestamped<Message>> 在 Timestamped<Message> 基础上添加标签功能
using TaggedMessage = Tagged<Timestamped<Message>>;

// ============================================================
// 主函数
// ============================================================
int main() {
  cout << "=========== 7.1 模板方法模式 =============" << endl;
  cout << "--- csv 处理 ---" << endl;
  CSVProcessor csvProcessor;
  csvProcessor.process();

  cout << "\n--- json 处理 ---" << endl;
  JsonProcessor jsonProcessor;
  jsonProcessor.process();
  cout << endl;

  cout << "=========== 7.2 策略模式 =============" << endl;
  vector<int> data = {5, 2, 9, 1, 25, 10, 16, 9, 100, 3};
  BubbleSort bubbleSort;
  QuickSort quickSort;

  Sorter sorter(&bubbleSort);
  sorter.sort(data);
  cout << "排序结果: ";
  for (int num : data) {
    cout << num << " ";
  }
  cout << endl;
  // 切换策略
  sorter.setStrategy(&quickSort);
  // 重新排序
  sorter.sort(data);
  cout << "排序结果: ";
  for (int num : data) {
    cout << num << " ";
  }
  cout << endl << endl;

  cout << "=========== 7.3 CRTP =============" << endl;
  {
    Widget w1("按钮");
    Widget w2("文本框");
    Gadget g1;
    cout << "当前 Widget 数量: " << CounterCPTR<Widget>::getCount() << endl;
    cout << "当前 Gadget 数量: " << CounterCPTR<Gadget>::getCount() << endl;
  }
  cout << "销毁对象后 Widget 数量: " << CounterCPTR<Widget>::getCount() << endl;
  cout << "销毁对象后 Gadget 数量: " << CounterCPTR<Gadget>::getCount() << endl;

  cout << "\n--- CRTP 编译期多态 ---" << endl;
  Person person("张三", 30);
  person.print();
  Product product("手机", 2999.99);
  product.print();
  cout << endl;

  cout << "=========== 7.4 Mixin 模式 =============" << endl;
  TaggedMessage msg({"重要", "待办"}, "2026-04-19 10:00",
                    "这是一个重要的待办消息。");
  msg.printTags();
  msg.printWithTimestamp();
  cout << " 有‘日记’标签？" << (msg.hasTag("日记") ? "有" : "没有") << endl;
  cout << " 有‘重要’标签？" << (msg.hasTag("重要") ? "有" : "没有") << endl;

  return 0;
}
