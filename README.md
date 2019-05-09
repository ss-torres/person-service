# person-service  
C++实现的一个grpc调用的简单示例  
注:   
客户端代码:  
(1) person_manip.h  
(2) person_client.cpp  
服务端代码:  
(1) person_manage.h  
(2) person_server.cpp  
  
编译上述代码依赖protobuf, grpc和folly库, 这些可以参考:  
grpc安装: https://www.cnblogs.com/albizzia/p/10803032.html  
folly库安装: https://www.cnblogs.com/albizzia/p/10824721.html  
如果你不想使用folly库, 而且只有一个客户端单线程调用服务, 可以将folly库中的folly::ConcurrentHashMap替换成std::unordered_map,  
然后移除CMakeLists中相关的target_link_libraries就可以了.  

编译过程如下(在person-service文件夹):  
$ mkdir build  
$ cd build  
$ cmake ..  
$ make  
其中生成的person-client和person-server分别是客户端和服务端, 使用./person-client和./person-server启动这两个应用就可以在person-client中  
看到相关输出.  
  
文档说明可以参考: https://www.cnblogs.com/albizzia/p/10830878.html
