//异常处理
//try catch throw
//throw 抛出异常
//try 把可能出错的代码包起来
//catch 捕获并且处理异常
/*
try{
    //the code which may throw excption
}catch(type 1& e){
    //dealing method 1
}catch(type 2&e ){
    //dealing method 2
}
抛出异常之后，程序会从throw开始向上寻找匹配的catch
如果当前函数没有，继续从调用者里面找
这个过程叫栈展开 栈展开会调用已经构造对象的析沟函数
RAII
*/
/*
catch的匹配规则
catch 按类型匹配，可以有多个
try {
    throw std::out_of_range("index error");
} catch (const std::out_of_range& e) {
    std::cout << "越界: " << e.what() << '\n';
} catch (const std::exception& e) {
    std::cout << "其他标准异常: " << e.what() << '\n';
}
派生类catch要放在基类面前
因为 std::out_of_range 是 std::exception 的派生类。
如果先写 catch (const std::exception&)，
后面的就永远匹配不到。
推荐用 const 引用 捕获，避免拷贝，也避免对象切片。
catch (const std::exception& e)
catch (...) 捕获所有异常
catch (...) {
    // 通常用于清理，然后重新抛出
    throw;
}
*/