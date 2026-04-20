/**
 * 第十章：专家级实战 —— 综合项目与设计模式
 *
 * 本章要点：
 * 1. 实战项目：简易 ECS（实体组件系统）
 * 2. 类型安全的状态机
 * 3. 表达式模板
 * 4. 编译期多态 vs 运行时多态对比
 * 5. 现代 C++ 类设计最佳实践
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>
#include <variant>
#include <optional>
#include <array>
#include <numeric>
using namespace std;

// ============================================================
// 10.1 ECS（Entity-Component-System）框架
// ============================================================

// 组件基类标记
struct Component {
    virtual ~Component() = default;
};

// 具体组件
struct Position : Component {
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Velocity : Component {
    float vx, vy;
    Velocity(float vx = 0, float vy = 0) : vx(vx), vy(vy) {}
};

struct Health : Component {
    int hp, max_hp;
    Health(int hp = 100) : hp(hp), max_hp(hp) {}
};

struct Name : Component {
    string name;
    Name(const string& n) : name(n) {}
};

// 实体：持有组件的容器
class Entity {
    size_t id;
    unordered_map<type_index, unique_ptr<Component>> components;

public:
    Entity(size_t id) : id(id) {}

    size_t get_id() const { return id; }

    template <typename T, typename... Args>
    T& add(Args&&... args) {
        auto comp = make_unique<T>(std::forward<Args>(args)...);
        T* ptr = comp.get();
        components[type_index(typeid(T))] = std::move(comp);
        return *ptr;
    }

    template <typename T>
    T* get() {
        auto it = components.find(type_index(typeid(T)));
        if (it != components.end())
            return static_cast<T*>(it->second.get());
        return nullptr;
    }

    template <typename T>
    bool has() const {
        return components.count(type_index(typeid(T))) > 0;
    }

    template <typename T>
    void remove() {
        components.erase(type_index(typeid(T)));
    }
};

// 世界：管理实体和系统
class World {
    vector<unique_ptr<Entity>> entities;
    size_t next_id = 0;

public:
    Entity& create_entity() {
        entities.push_back(make_unique<Entity>(next_id++));
        return *entities.back();
    }

    // 遍历所有具有指定组件的实体
    template <typename... Components, typename Func>
    void each(Func&& func) {
        for (auto& e : entities) {
            if ((e->has<Components>() && ...)) {
                func(*e);
            }
        }
    }

    size_t entity_count() const { return entities.size(); }
};

// 系统：对实体进行批量操作
void movement_system(World& world) {
    world.each<Position, Velocity>([](Entity& e) {
        auto* pos = e.get<Position>();
        auto* vel = e.get<Velocity>();
        pos->x += vel->vx;
        pos->y += vel->vy;
    });
}

void display_system(World& world) {
    world.each<Name, Position>([](Entity& e) {
        auto* name = e.get<Name>();
        auto* pos = e.get<Position>();
        cout << "  " << name->name << " @ (" << pos->x << ", " << pos->y << ")";
        if (auto* hp = e.get<Health>()) {
            cout << " HP: " << hp->hp << "/" << hp->max_hp;
        }
        cout << endl;
    });
}

// ============================================================
// 10.2 类型安全的状态机
// ============================================================
namespace StateMachine {

struct Idle {};
struct Running { int speed; };
struct Paused { int elapsed; };
struct Stopped {};

using State = variant<Idle, Running, Paused, Stopped>;

struct StartEvent {};
struct PauseEvent {};
struct ResumeEvent {};
struct StopEvent {};

// 状态转移表：编译期检查所有转移
State transition(const State& current, const StartEvent&) {
    if (holds_alternative<Idle>(current)) {
        return Running{10};
    }
    return current;  // 无效转移保持当前状态
}

State transition(const State& current, const PauseEvent&) {
    if (auto* r = get_if<Running>(&current)) {
        return Paused{r->speed};
    }
    return current;
}

State transition(const State& current, const ResumeEvent&) {
    if (auto* p = get_if<Paused>(&current)) {
        return Running{p->elapsed};
    }
    return current;
}

State transition(const State& current, const StopEvent&) {
    if (holds_alternative<Running>(current) || holds_alternative<Paused>(current)) {
        return Stopped{};
    }
    return current;
}

string state_name(const State& s) {
    if (holds_alternative<Idle>(s))    return "空闲";
    if (holds_alternative<Running>(s)) return "运行中";
    if (holds_alternative<Paused>(s))  return "已暂停";
    if (holds_alternative<Stopped>(s)) return "已停止";
    return "未知";
}

}  // namespace StateMachine

// ============================================================
// 10.3 表达式模板（惰性求值）
// ============================================================
template <typename E>
class VecExpression {
public:
    double operator[](size_t i) const {
        return static_cast<const E&>(*this)[i];
    }
    size_t size() const {
        return static_cast<const E&>(*this).size();
    }
};

class Vec : public VecExpression<Vec> {
    vector<double> data;

public:
    Vec(size_t n, double val = 0) : data(n, val) {}

    template <typename E>
    Vec(const VecExpression<E>& expr) : data(expr.size()) {
        for (size_t i = 0; i < data.size(); ++i)
            data[i] = expr[i];
    }

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }
    size_t size() const { return data.size(); }

    // 直接赋值：惰性求值，避免临时对象
    template <typename E>
    Vec& operator=(const VecExpression<E>& expr) {
        for (size_t i = 0; i < data.size(); ++i)
            data[i] = expr[i];
        return *this;
    }

    friend ostream& operator<<(ostream& os, const Vec& v) {
        os << "[";
        for (size_t i = 0; i < v.size(); ++i) {
            if (i > 0) os << ", ";
            os << v[i];
        }
        os << "]";
        return os;
    }
};

// 加法表达式（不产生临时对象）
template <typename E1, typename E2>
class VecAdd : public VecExpression<VecAdd<E1, E2>> {
    const E1& u;
    const E2& v;

public:
    VecAdd(const E1& u, const E2& v) : u(u), v(v) {}

    double operator[](size_t i) const { return u[i] + v[i]; }
    size_t size() const { return u.size(); }
};

template <typename E1, typename E2>
VecAdd<E1, E2> operator+(const VecExpression<E1>& u, const VecExpression<E2>& v) {
    return VecAdd<E1, E2>(static_cast<const E1&>(u), static_cast<const E2&>(v));
}

// 标量乘法表达式
template <typename E>
class VecScale : public VecExpression<VecScale<E>> {
    const E& v;
    double s;

public:
    VecScale(const E& v, double s) : v(v), s(s) {}

    double operator[](size_t i) const { return v[i] * s; }
    size_t size() const { return v.size(); }
};

template <typename E>
VecScale<E> operator*(const VecExpression<E>& v, double s) {
    return VecScale<E>(static_cast<const E&>(v), s);
}

template <typename E>
VecScale<E> operator*(double s, const VecExpression<E>& v) {
    return VecScale<E>(static_cast<const E&>(v), s);
}

// ============================================================
// 10.4 现代 C++ 类设计最佳实践
// ============================================================

// 使用 std::optional 表示可能不存在的值
class Config {
    optional<string> database_host;
    optional<int> database_port;
    string app_name;

public:
    Config(const string& name) : app_name(name) {}

    Config& set_db_host(const string& host) {
        database_host = host;
        return *this;
    }

    Config& set_db_port(int port) {
        database_port = port;
        return *this;
    }

    void print() const {
        cout << "  应用: " << app_name << endl;
        cout << "  数据库: "
             << database_host.value_or("localhost") << ":"
             << database_port.value_or(3306) << endl;
    }
};

// 使用 std::variant 实现类型安全的联合
class JSONValue {
    using Object = unordered_map<string, JSONValue>;
    using Array = vector<JSONValue>;

    variant<nullptr_t, bool, double, string, Array, Object> value;

public:
    JSONValue() : value(nullptr) {}
    JSONValue(bool b) : value(b) {}
    JSONValue(double d) : value(d) {}
    JSONValue(const string& s) : value(s) {}
    JSONValue(const char* s) : value(string(s)) {}

    void print(ostream& os = cout, int indent = 0) const {
        string pad(indent * 2, ' ');
        visit([&](const auto& v) {
            using T = decay_t<decltype(v)>;
            if constexpr (is_same_v<T, nullptr_t>) {
                os << "null";
            } else if constexpr (is_same_v<T, bool>) {
                os << (v ? "true" : "false");
            } else if constexpr (is_same_v<T, double>) {
                os << v;
            } else if constexpr (is_same_v<T, string>) {
                os << "\"" << v << "\"";
            } else if constexpr (is_same_v<T, Array>) {
                os << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) os << ", ";
                    v[i].print(os, indent);
                }
                os << "]";
            } else if constexpr (is_same_v<T, Object>) {
                os << "{";
                bool first = true;
                for (const auto& [k, val] : v) {
                    if (!first) os << ", ";
                    os << "\"" << k << "\": ";
                    val.print(os, indent);
                    first = false;
                }
                os << "}";
            }
        }, value);
    }
};

// ============================================================
// 主函数
// ============================================================
int main() {
    cout << "========== 10.1 ECS 实体组件系统 ==========" << endl;
    World world;

    auto& player = world.create_entity();
    player.add<Name>("勇者");
    player.add<Position>(0, 0);
    player.add<Velocity>(1.5f, 0.5f);
    player.add<Health>(150);

    auto& enemy = world.create_entity();
    enemy.add<Name>("史莱姆");
    enemy.add<Position>(10, 5);
    enemy.add<Velocity>(-0.3f, 0.1f);
    enemy.add<Health>(30);

    auto& item = world.create_entity();
    item.add<Name>("金币");
    item.add<Position>(5, 3);

    cout << "  实体数量: " << world.entity_count() << endl;
    cout << "\n--- 初始状态 ---" << endl;
    display_system(world);

    cout << "\n--- 移动后 ---" << endl;
    movement_system(world);
    movement_system(world);
    display_system(world);
    cout << endl;

    cout << "========== 10.2 类型安全状态机 ==========" << endl;
    using namespace StateMachine;
    State state = Idle{};

    cout << "  当前: " << state_name(state) << endl;
    state = transition(state, StartEvent{});
    cout << "  Start -> " << state_name(state) << endl;
    state = transition(state, PauseEvent{});
    cout << "  Pause -> " << state_name(state) << endl;
    state = transition(state, ResumeEvent{});
    cout << "  Resume -> " << state_name(state) << endl;
    state = transition(state, StopEvent{});
    cout << "  Stop -> " << state_name(state) << endl;
    state = transition(state, StartEvent{});
    cout << "  Start(无效) -> " << state_name(state) << " (Stopped 不接受 Start)" << endl;
    cout << endl;

    cout << "========== 10.3 表达式模板 ==========" << endl;
    Vec a(5), b(5), c(5);
    for (size_t i = 0; i < 5; ++i) {
        a[i] = i * 1.0;
        b[i] = i * 2.0;
        c[i] = i * 3.0;
    }
    cout << "  a = " << a << endl;
    cout << "  b = " << b << endl;
    cout << "  c = " << c << endl;

    // 表达式模板：a + b*2 + c 一次性求值，无临时对象
    Vec result(5);
    result = a + b * 2.0 + c;
    cout << "  a + b*2 + c = " << result << endl;
    cout << endl;

    cout << "========== 10.4 现代 C++ 类设计 ==========" << endl;
    Config cfg("MyApp");
    cfg.set_db_host("192.168.1.100").set_db_port(5432);
    cfg.print();

    Config cfg2("TestApp");  // 使用默认值
    cfg2.print();
    cout << endl;

    cout << "--- JSON 值 ---" << endl;
    JSONValue obj;
    obj.print();
    cout << endl;

    JSONValue num(42.0);
    num.print();
    cout << endl;

    JSONValue str(string("hello"));
    str.print();
    cout << endl;

    return 0;
}
