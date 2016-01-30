# MyDict
1.基于Web的英汉字典，数据来自六级必背词汇，词汇量约15000\n
2.数据结构为字典树（26叉树）
3.底层用C编写，并将其编译成了Python库文件
4.字典数据使用json文件存储，程序运行时将词库数据直接读入内存
5.最终呈现形式为Web，使用了flask web框架、bootstrap前端框架以及js插件typeahead做单词推荐
6.代码为开发完并使用flask自带服务器软件运行成功的版本，最终部署参考：http://www.oschina.net/translate/serving-flask-with-nginx-on-ubuntu

# 运行和部署
1.若没安装flask，需先安装flask(sudo apt-get install python-pip, pip install flask)
2.下载文件后进入文件目录输入python server.py可使用flask自带的服务器运行程序，默认端口为5000，访问地址为ip:5000
3.最终部署可使用 virtualenv+nginx+uWSGI，部署参考如上
