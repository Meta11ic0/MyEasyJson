# 菜鸟的第一个小项目--JSON库

## 0. 免责声明
不知道是不是有跟我一样的人，盲从大流选了计算机，普通大学里面普通混了四年到工作继续混。现在我是在一家证券公司当外包的初级C++开发，因为技术真的水，不过相对还算稳定。但现在年岁渐长，我意识到我得努力一下，30多岁的时候不想那么容易被淘汰，所以我开始做这些尝试，巩固自己技术基础的同时学习更多新知识，对很多自己一知半解，或者闻所未闻的技术进行学习。因为看到过费曼学习法主打的就是一个输出倒帮输入，教会别人会让自己学到的东西更加巩固，同时也能督促自己保持学习。所以现在我从一个菜鸟的视角出发，希望能帮助到很多像我一样的迷茫的普通人，或者刚踏入这个领域的初学者，来讲一下JSON库。如果你遇到问题，有不理解的地方，或是有建议，都欢迎在评论中提出，让所有人一起讨论，高手、大神请轻点喷，望多多包涵。

## 1. 什么是JSON？
要做一个JSON库，我们首先还是要搞清楚JSON到底是什么？查阅维基百科可得：
~~~
JSON (JavaScript Object Notation, pronounced /ˈdʒeɪsən/; also /ˈdʒeɪˌsɒn/) is an open standard file format and data interchange format that uses human-readable text to store and transmit data objects consisting of attribute–value pairs and arrays (or other serializable values). It is a common data format with diverse uses in electronic data interchange, including that of web applications with servers.

JSON is a language-independent data format. It was derived from JavaScript, but many modern programming languages include code to generate and parse JSON-format data. JSON filenames use the extension .json.
~~~
到了英文学习时间，很简单一个长句，意思是说json是一种公开的文件标准和数据交换格式，用于存储和发送人类可读的（对比二进制码，人类不可读）数据对象，数据对象由“属性-值对”（类似于map）和数组（或者其他可以序列化的值）组合而成。这是一种很常见的数据格式，被广泛应用在电子数据交换上，包括WEB应用和服务器之间的数据交换。

JSON是一种语言独立的数据格式。它从JavaScript分离出来，但是很多现代的编程语言都有代码去生成和解析JSON格式的数据。JSON文件拓展名字使用 **.json**.