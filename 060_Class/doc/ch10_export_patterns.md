# 第十章：实战项目

> 本章综合运用前面所学知识，通过实战项目展示现代 C++ 类设计的最佳实践。

**源码位置**: `ch10_export_patterns/src/main.cpp`

> **注意**: 本章框架已搭建，以下内容为规划的项目和知识点，源码待完善。

---

## 10.1 实战项目：简易 ECS (Entity-Component-System) 框架

### 什么是 ECS？

ECS 是一种常用于游戏开发的架构模式：
- **Entity（实体）**: 只是一个 ID，本身没有数据和逻辑
- **Component（组件）**: 纯数据容器，如 `Position`、`Velocity`、`Health`
- **System（系统）**: 处理拥有特定组件的实体的逻辑

### 设计思路

```cpp
// 组件: 纯数据
struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Health { int hp; int maxHp; };

// 实体: 只是一个 ID
using EntityId = size_t;

// 组件管理器: 为每种组件类型存储数据
class ComponentManager {
  // 使用 std::any 或 type_index + unordered_map 实现类型擦除的存储
  // 每种组件类型对应一个 dense array
};

// 系统: 处理逻辑
class MovementSystem {
public:
  void update(ComponentManager &cm, float dt) {
    // 遍历所有有 Position + Velocity 的实体
    // position.x += velocity.dx * dt;
    // position.y += velocity.dy * dt;
  }
};

class RenderSystem { ... };
class HealthSystem { ... };

// ECS 世界
class World {
  ComponentManager components;
  vector<unique_ptr<System>> systems;
public:
  EntityId createEntity();
  void destroyEntity(EntityId id);

  template <typename T>
  void addComponent(EntityId id, T component);

  template <typename T>
  T &getComponent(EntityId id);

  void update(float dt);
};
```

### 涉及的核心技术

| 技术 | 应用 |
|------|------|
| 模板 | 泛型组件存储和访问 |
| 类型擦除 | 统一管理不同类型的组件 |
| CRTP | 高效的系统基类 |
| 移动语义 | 组件数据的零拷贝转移 |
| 变参模板 | 批量添加组件 |
| `std::any` / `type_index` | 运行时类型信息 |

---

## 10.2 类型安全的状态机

使用模板和枚举实现编译期安全的状态转换：

```cpp
template <typename Derived, typename State>
class StateMachine {
  State currentState;

public:
  StateMachine(State initial) : currentState(initial) {}

  void transitionTo(State newState) {
    static_cast<Derived*>(this)->onExit(currentState);
    currentState = newState;
    static_cast<Derived*>(this)->onEnter(newState);
  }

  State state() const { return currentState; }
};

// 使用
enum class TrafficLight { Red, Green, Yellow };

class TrafficController : public StateMachine<TrafficController, TrafficLight> {
public:
  TrafficController() : StateMachine(TrafficLight::Red) {}
  void onEnter(TrafficLight state) { ... }
  void onExit(TrafficLight state) { ... }
};
```

---

## 10.3 表达式模板

利用模板延迟计算，实现数学表达式的高效求值：

```cpp
// 向量运算: 避免临时对象
template <typename L, typename R>
struct VecAdd {
  const L &left;
  const R &right;
  double operator[](size_t i) const { return left[i] + right[i]; }
};

// 延迟求值: a + b + c 不产生中间临时对象
auto result = VecAdd<VecAdd<Vec, Vec>, Vec>{VecAdd<Vec, Vec>{a, b}, c};
```

---

## 10.4 编译期多态 vs 运行时多态对比

| 特性 | 虚函数（运行时） | CRTP/Concepts（编译期） |
|------|----------------|----------------------|
| 灵活性 | 运行时选择类型 | 编译期确定类型 |
| 性能 | vtable 间接调用 | 零开销，可内联 |
| 代码膨胀 | 无 | 每种类型一份代码 |
| 二进制兼容 | 好 | 差 |
| 适用场景 | 插件、动态加载 | 高性能库、嵌入式 |

---

## 10.5 现代 C++ 类设计最佳实践

### Rule of Zero

优先让编译器自动生成特殊成员函数：

```cpp
class GoodClass {
  std::string name;           // 由 std::string 管理内存
  std::vector<int> data;      // 由 std::vector 管理内存
public:
  GoodClass(const std::string &n) : name(n) {}
  // 不需要自定义析构、拷贝、移动 -- 编译器全部搞定
};
```

### RAII (Resource Acquisition Is Initialization)

资源获取即初始化，利用构造/析构管理资源生命周期：

```cpp
class FileHandle {
  FILE *file;
public:
  FileHandle(const char *path, const char *mode) : file(fopen(path, mode)) {
    if (!file) throw runtime_error("打开文件失败");
  }
  ~FileHandle() { if (file) fclose(file); }     // 析构自动释放

  FileHandle(const FileHandle &) = delete;       // 禁止拷贝
  FileHandle(FileHandle &&other) noexcept : file(other.file) {
    other.file = nullptr;
  }
};
```

### 使用 smart pointer

```cpp
// 独占所有权
auto ptr = make_unique<Widget>("按钮");

// 共享所有权
auto shared = make_shared<Widget>("按钮");

// 弱引用（打破循环引用）
weak_ptr<Widget> weak = shared;
```

### 面向接口编程

```cpp
// 通过接口（抽象类）解耦
class ILogger {
public:
  virtual ~ILogger() = default;
  virtual void log(const string &msg) = 0;
};

class FileLogger : public ILogger { ... };
class ConsoleLogger : public ILogger { ... };

// 依赖注入
class Service {
  unique_ptr<ILogger> logger;
public:
  Service(unique_ptr<ILogger> log) : logger(std::move(log)) {}
};
```

---

## 本章小结

| 实战主题 | 核心技术 |
|---------|---------|
| ECS 框架 | 模板、类型擦除、CRTP、变参模板 |
| 状态机 | CRTP、枚举、编译期安全 |
| 表达式模板 | 延迟计算、模板嵌套 |
| 性能对比 | 虚函数 vs CRTP trade-off |
| 最佳实践 | Rule of Zero、RAII、智能指针、接口隔离 |
