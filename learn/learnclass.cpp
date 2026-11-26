//类和对象
#include <stdexcept>
//定义了一组标准异常类，主要用于表示程序中的逻辑错误
//和运行时错误 
//这些异常都直接或者间接继承自std::exception
//并用e.what()获取错误信息
class Counter{
public:
    explicit Counter(int initial =0)
    : value_{initial}{
        if(initial <0){
            throw std::invalid_argument("初始值不能为负");
        }
    }

    void add(int amount){
        if(amount<0){
            throw std::invalid_argument("增量不能为负");
        }
        value_ +=amount;
    }
    int value()const noexcept{
        return value_;
    }
private:
    int value_;
};
/*
重点掌握：
class 与 struct：默认访问权限不同，
class 默认 private，struct 默认 public。
成员变量、成员函数、this 指针。
构造函数：
默认构造、带参构造、拷贝构造、移动构造。
初始化列表优先于函数体赋值。
析构函数：对象离开作用域或 delete 时调用。
const 成员函数：不修改对象状态。
static 成员：属于类而非对象。
friend：谨慎使用，主要用于运算符重载或紧密协作类。

构造、析构、拷贝、移动：C++ 最大特色

*/
/*
继承与多态
class Animal {
public:
    void eat() { std::cout << "Animal eat\n"; }
    virtual void speak() { std::cout << "Animal speak\n"; }
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak() override { std::cout << "Dog bark\n"; }
};
Dog 继承 Animal，Dog 是一种 Animal。
多态：同一接口，不同行为
C++ 多态主要分：
编译期多态：函数重载、模板
运行期多态：虚函数 + 基类指针/引用
#include <iostream>
#include <vector>
#include <memory>

class Animal {
public:
    virtual void speak() const {
        std::cout << "Animal speak\n";
    }
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "Dog bark\n";
    }
};

class Cat : public Animal {
public:
    void speak() const override {
        std::cout << "Cat meow\n";
    }
};

int main() {
    std::vector<std::unique_ptr<Animal>> animals;
    animals.push_back(std::make_unique<Dog>());
    animals.push_back(std::make_unique<Cat>());

    for (const auto& a : animals) {
        a->speak();  // 动态绑定：实际调用 Dog/Cat 的 speak
    }
}

override、final、纯虚函数
建议所有重写虚函数都写 override
class Dog : public Animal {
public:
    void speak() const override;
};
final
禁止继续重写或继承
纯虚函数与抽象类
含纯虚函数的类是抽象类，不能实例化，只能作为接口使用。
class Shape {
public:
    virtual double area() const = 0; // 纯虚函数
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double r;
public:
    explicit Circle(double r) : r(r) {}
    double area() const override { return 3.14159 * r * r; }
};
重载、重写、隐藏
重载 overload：同一作用域，函数名相同参数不同。
重写 override：派生类重写基类虚函数，签名相同。
隐藏 hiding：派生类同名函数会隐藏基类所有同名函数，
除非 using
class Base {
public:
    virtual void f(int) {}
    void g() {}
};

class Derived : public Base {
public:
    void f(double) {}  // 不是 override，是隐藏
    // using Base::f;   // 可把 Base::f 引入作用域
};
*/