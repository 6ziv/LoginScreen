# 定制Windows登录页面 #

啊…考虑了好久，还是决定从之前的仓库里分出来。

嘛，不是因为别的什么原因，只是原来的仓库已经膨胀的越来越大了，看起来实在是有些难受…而这次的东西又要带着一大堆巨大的binary丢上去，如果merge到一起实在让人有些难受了啦！

虽然我不怎么习惯在Readme里写一堆废话，也就两年前建立那个仓库时写了一大堆…

但是毕竟这次我干不动了。一方面是调各种奇妙的坑太久了，失去热情，想要投入新的坑里；另一方面，也是毕设要开始了——据指导老师说，好像要做的内容还是有点麻烦的。

所以，总之，在各种原因的作用下，最后只能扔出来这么个一个接近半成品的玩意…

所以我觉得我还是应该大概解释一下这是个什么东西…

### 功能简介 ###

简单地说，这就是一个帮助您替换掉自己电脑的登录页的东西——不过很抱歉，我还没能找到替换锁屏页的方法，因此没办法为它们指定相似的主题或者怎么样——不过您总是可以通过系统提供的方式调整锁屏图片，或者取消锁屏页嘛。

为了尽可能保证高的自由度，这个项目采用了CEF渲染网页的方式进行加载，资源则被打包在zip文件中。zip文件的结构会在稍后进行说明。

目前，我们已经支持了本地账户登录和Microsoft账户登录，但是还不支持PIN登录、指纹登陆、面部识别（因为它们是由其它Credential Provider实现的…）

### 编译安装说明 ###

本来试图写一个漂亮的CMakeList出来——结果发现写出来的东西果然还是很难用。

所以请各位多多包涵了啦…

嘛，首先用Qt Creator（或者其它能正常找到Qt库的工具）对根目录的CMakelists.txt进行编译…

然后——很抱歉，至少Qt Creator是只编译不安装的…但是因为涉及到一些奇怪的安装操作，所以需要手动执行一下cmake --install才能安装上…

另一方面，Qt也不会帮我们设置安装路径。所以说，如果对CMake指定的默认安装路径有什么不满的话——就恐怕需要手动指定CMAKE_INSTALL_PREFIX了（好麻烦）……

安装后的配置工作倒是会自动执行——如果没能成功的话，就请手动执行一下安装路径下的LoginScreenConfig.exe（配置程序）吧。

卸载的话通过配置程序卸载之后把安装目录删掉应该就可以了…

### 资源包结构

嘛，包内最好还是用ASCII字符；包本身的路径倒是可以有Unicode字符。

然后的话…在程序启动时会加载包里的index.html，同时您也可以提供一份about.txt用来在右键菜单中对这个资源包进行说明。除此之外，您也可以提供internal_error.html——不过我想在大多数情况下都没有这个必要。

zip格式允许为文件添加备注，程序会直接将备注内容作为MIME类型传递给CEF库。如果没有备注，则根据扩展名猜测MIME类型——如果依然不是硬编码的扩展名之一，那么返回application/octet-stream。

网页内可以加载资源包中的文件，但是请不要进行网络访问、文件访问、cookie读写等操作——而且我也已经尽量在我看到的各个位置禁用了这些操作。

访问文件时请尽量使用相对路径。如果一定要使用绝对路径的话，zip根目录对应的url为http://localhost_lockscreen/。

此外，还有几个被保留的路径。

http://local_lockscreen/getuser.do为用户列表，您可以从这里获取所有用户的显示名。

http://local_lockscreen/login.do为发送登录请求的地址。您可以向这里post表单以登录。其中，用户id字段名字为“uid”，密码字段名字为"passwd"。其中，用户id为用户在用户列表中的位置（从0开始编号）

http://localhost_lockscreen/avatar/xxx则为用户xxx的头像。

具体可以参考程序中附带样例资源包的实现。

### 二次开发说明 ###

啊是的，虽然现在只有一个破破烂烂的半成品，但是它是暴露了一套简陋的接口的。

具体的话就是代码里的Login.h。

getInstance()可以取得当前程序中唯一的Login实例；

Init()则用来在程序启动时进行初始化。(当然，如果你写的也是CEF这种坑人的多进程应用，那只要在某个进程里调用一次就够了…)

之后就只要在登录时调用doLogin()就可以了。如果出错，则返回false。此时应该停止程序运行并立即退出。

理论上讲您不用判断登陆成功与否——如果成功了，这个程序就会被父进程kill掉（笑）。doLogin的返回值仅仅代表着登录过程是否出现异常行为：出现异常则返回false，通信正常但是登录失败了则返回true。

函数原型的话在头文件里都能看到——毕竟也就是不到100行的一个header-only的类，应该没有做太多解释的必要了吧.

### License ###

WTFPL协议。



使用到的开源组件：

[Windows Classic Samples](https://github.com/microsoft/Windows-classic-samples/blob/master/LICENSE) (MIT License)

[Qt Opensource](https://www.qt.io/licensing/) (LGPL License)

[rapidjson](https://github.com/Tencent/rapidjson/blob/master/license.txt) (MIT License)

[CEF](https://github.com/chromiumembedded/cef) (MIT License)

[MiniINI](http://miniini.tuxfamily.org/license.php) (MIT License)

[libzip](https://libzip.org/license/) (BSD3 License)

[utf8cpp](http://utfcpp.sourceforge.net/) (See 'utf8.h' in source distrubution for license)



* 关于资源包的说明

资源包中的网页代码出自网络上一篇教程，根据上下文的说法判断，应该是可以免费使用的。

（而且那篇文章现在已经全网都是了…好像也没办法考据出处）

但是因为没有具体的协议，我并不是很确定以何种方式再分发才是符合作者原意的，因此暂时保守一些，请各位就暂且当它是个binary distribution好了，仅用于说明打包的格式和接口的调用——毕竟这些部分是我写的，可以随便用了啦（逃，以后有空还有钱的时候一定请室友吃饭让他帮我写个替代品…



### 应急处置 ###

啊，差点忘了…

因为我写这破玩意的时候好几次害得自己没法登录…本来想加一个应急开关来着，但是一方面是后来主要做的都是一些细节调整，就不怎么出问题了；另一方面是我实在没想好怎么样的操作才合理…

所以，总之就是没能加上…

如果出了什么问题导致无法登录的话…enmmmmm…可以试试强制关机三次，进恢复模式启动命令提示符，把C:\Windows\System32\LoginScreenLibrary.dll删掉…