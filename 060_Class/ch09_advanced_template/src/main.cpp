/**
 * 第九章：模板元编程 —— 编译期计算
 *
 * 本章要点：
 * 1. 编译期计算（阶乘、斐波那契）
 * 2. 类型萃取（type traits）
 * 3. SFINAE 原则
 * 4. if constexpr（C++17）
 * 5. 概念与约束（C++20）
 * 6. 编译期类型列表
 */

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>
#include <array>
#include <tuple>
using namespace std;

// ============================================================
// 9.1 编译期计算
// ============================================================
// 编译期阶乘
template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr int value = 1;
};

// 编译期斐波那契
template <int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template <>
struct Fibonacci<0> { static constexpr int value = 0; };

template <>
struct Fibonacci<1> { static constexpr int value = 1; };

// 编译期判断是否为质数
template <int N, int D = 2>
struct IsPrime {
    static constexpr bool value = (N % D != 0) && IsPrime<N, D + 1>::value;
};

template <int N>
struct IsPrime<N, N> {
    static constexpr bool value = true;
};

template <>
struct IsPrime<1, 2> {
    static constexpr bool value = false;
};

template <>
struct IsPrime<2, 2> {
    static constexpr bool value = true;
};

// ============================================================
// 9.2 自定义类型萃取
// ============================================================
// 判断两个类型是否相同
template <typename T, typename U>
struct IsSame {
    static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T> {
    static constexpr bool value = true;
};

// 移除 const
template <typename T>
struct RemoveConst {
    using type = T;
};

template <typename T>
struct RemoveConst<const T> {
    using type = T;
};

// 判断是否是数组
template <typename T>
struct IsArray {
    static constexpr bool value = false;
};

template <typename T>
struct IsArray<T[]> {
    static constexpr bool value = true;
};

template <typename T, size_t N>
struct IsArray<T[N]> {
    static constexpr bool value = true;
};

// 条件类型选择
template <bool Cond, typename TrueType, typename FalseType>
struct Conditional {
    using type = TrueType;
};

template <typename TrueType, typename FalseType>
struct Conditional<false, TrueType, FalseType> {
    using type = FalseType;
};

// ============================================================
// 9.3 SFINAE：根据类型启用不同实现
// ============================================================
// 检测类型是否有 size() 方法
template <typename T, typename = void>
struct HasSize : false_type {};

template <typename T>
struct HasSize<T, void_t<decltype(declval<T>().size())>> : true_type {};

// 使用 SFINAE 选择实现
template <typename T>
enable_if_t<HasSize<T>::value, void>
print_size(const T& container) {
    cout << "  容器大小: " << container.size() << endl;
}

template <typename T>
enable_if_t<!HasSize<T>::value, void>
print_size(const T&) {
    cout << "  该类型没有 size() 方法" << endl;
}

// ============================================================
// 9.4 if constexpr（C++17 简化版）
// ============================================================
template <typename T>
string to_string_custom(const T& value) {
    if constexpr (is_integral_v<T>) {
        return "整数: " + std::to_string(value);
    } else if constexpr (is_floating_point_v<T>) {
        return "浮点数: " + std::to_string(value);
    } else if constexpr (is_same_v<T, string>) {
        return "字符串: " + value;
    } else if constexpr (is_same_v<T, bool>) {
        return value ? "布尔: true" : "布尔: false";
    } else {
        return "未知类型";
    }
}

// 编译期递归展开 tuple
template <typename Tuple, size_t I = 0>
void print_tuple(const Tuple& t) {
    if constexpr (I < tuple_size_v<Tuple>) {
        cout << get<I>(t);
        if constexpr (I + 1 < tuple_size_v<Tuple>) cout << ", ";
        print_tuple<Tuple, I + 1>(t);
    }
}

// ============================================================
// 9.5 编译期类型列表与算法
// ============================================================
// 类型列表
template <typename... Ts>
struct TypeList {};

// 获取长度
template <typename List>
struct Length;

template <typename... Ts>
struct Length<TypeList<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
};

// 获取第 N 个类型
template <size_t N, typename List>
struct Get;

template <size_t N, typename Head, typename... Tail>
struct Get<N, TypeList<Head, Tail...>> : Get<N - 1, TypeList<Tail...>> {};

template <typename Head, typename... Tail>
struct Get<0, TypeList<Head, Tail...>> {
    using type = Head;
};

// 检查类型是否在列表中
template <typename T, typename List>
struct Contains;

template <typename T>
struct Contains<T, TypeList<>> {
    static constexpr bool value = false;
};

template <typename T, typename... Tail>
struct Contains<T, TypeList<T, Tail...>> {
    static constexpr bool value = true;
};

template <typename T, typename Head, typename... Tail>
struct Contains<T, TypeList<Head, Tail...>> {
    static constexpr bool value = Contains<T, TypeList<Tail...>>::value;
};

// ============================================================
// 9.6 编译期字符串哈希
// ============================================================
constexpr uint64_t hash_string(const char* str, size_t len, size_t i = 0) {
    return (i >= len) ? 5381 : (hash_string(str, len, i + 1) * 33) ^ str[i];
}

constexpr uint64_t operator""_hash(const char* str, size_t len) {
    return hash_string(str, len);
}

// ============================================================
// 主函数
// ============================================================
int main() {
    cout << "========== 9.1 编译期计算 ==========" << endl;
    cout << "  5! = " << Factorial<5>::value << endl;          // 编译期计算
    cout << "  10! = " << Factorial<10>::value << endl;
    cout << "  Fibonacci(10) = " << Fibonacci<10>::value << endl;
    cout << "  7 是质数? " << (IsPrime<7>::value ? "是" : "否") << endl;
    cout << "  15 是质数? " << (IsPrime<15>::value ? "是" : "否") << endl;
    cout << endl;

    cout << "========== 9.2 类型萃取 ==========" << endl;
    cout << "  int == int? " << IsSame<int, int>::value << endl;
    cout << "  int == double? " << IsSame<int, double>::value << endl;

    using T1 = RemoveConst<const int>::type;
    cout << "  RemoveConst<const int> is int? " << IsSame<T1, int>::value << endl;

    cout << "  int[] 是数组? " << IsArray<int[]>::value << endl;
    cout << "  int 是数组? " << IsArray<int>::value << endl;

    // 条件类型
    using Bigger = Conditional<(sizeof(int) > sizeof(short)), int, short>::type;
    cout << "  Bigger 类型大小: " << sizeof(Bigger) << " 字节" << endl;
    cout << endl;

    cout << "========== 9.3 SFINAE ==========" << endl;
    vector<int> v = {1, 2, 3};
    print_size(v);
    print_size(42);
    cout << endl;

    cout << "========== 9.4 if constexpr ==========" << endl;
    cout << "  " << to_string_custom(42) << endl;
    cout << "  " << to_string_custom(3.14) << endl;
    cout << "  " << to_string_custom(string("hello")) << endl;
    cout << "  " << to_string_custom(true) << endl;

    cout << "\n  元组: ";
    auto t = make_tuple(1, "Alice", 3.14, true);
    print_tuple(t);
    cout << endl << endl;

    cout << "========== 9.5 类型列表 ==========" << endl;
    using MyTypes = TypeList<int, double, string, char>;
    cout << "  类型数量: " << Length<MyTypes>::value << endl;
    cout << "  包含 double? " << Contains<double, MyTypes>::value << endl;
    cout << "  包含 float? " << Contains<float, MyTypes>::value << endl;

    using Third = Get<2, MyTypes>::type;
    cout << "  第3个类型是 string? " << IsSame<Third, string>::value << endl;
    cout << endl;

    cout << "========== 9.6 编译期字符串哈希 ==========" << endl;
    constexpr auto h1 = "hello"_hash;
    constexpr auto h2 = "world"_hash;
    cout << "  \"hello\" hash = " << h1 << endl;
    cout << "  \"world\" hash = " << h2 << endl;
    // 可用于 switch 语句匹配字符串（编译期）

    return 0;
}
