### 基础知识

---

#### RAII

​		RAII全称是“Resource Acquisition is Initialization”，也就是资源获取即初始化。简单来说就是在构造函数中申请分配资源，在析构函数中释放资源。这样就可以使用类来管理资源，将资源和对象的生命周期绑定。

---

* 构造函数用来创建线程池
* append用来将请求加入到队列
* worker用来调动run
* run用来处理请求
