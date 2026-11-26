//RAII
//Resource Acquistion Is initialization
//是 C++ 中一种核心的资源管理惯用法：
//把资源的生命周期绑定到对象的生命周期上
/*
核心思想：
对象构造时获取资源；
对象析构时自动释放资源；
利用 C++ 栈对象离开作用域自动析构的机制，保证资源一定被释放；
即使发生异常，栈展开也会调用析构函数，因此天然具备异常安全性。

这里的“资源”不限于内存，还包括
堆内存：new/delete
文件句柄：fstream
互斥锁：lock_guard
网络连接、socket
数据库连接、事务
临时状态等
举例
#include <iostream>

class FileHandle {
public:
    FileHandle(const char* path) {
        fp_ = std::fopen(path, "r");
        if (!fp_) throw std::runtime_error("open failed");
    }

    ~FileHandle() {
        if (fp_) std::fclose(fp_);
    }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FILE* get() const { return fp_; }

private:
    FILE* fp_ = nullptr;
};


*/