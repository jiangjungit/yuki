<p align="center"><img src="./yuki.svg" alt="YUKI" height="200px"></p>
<div align="center">
  <a href="https://github.com/jiangjungit/yuki/actions">
    <img src="https://github.com/jiangjungit/yuki/workflows/CMake/badge.svg" alt="Build Status">
  </a>
  <a href="https://github.com/jiangjungit/yuki/releases">
    <img src="https://img.shields.io/github/release/jiangjungit/yuki.svg" alt="GitHub release">
  </a>
</div>

---

YUKI，一个可爱的B站相簿本地备份小工具(\*^▽^\*)。
 - [用前必读](#用前必读)
 - [使用方法](#使用方法)
 - [常见问题](#常见问题)
 - [为什么叫YUKI](#为什么叫YUKI)
 - [开发相关](#开发相关)
 - [许可协议](#许可协议)

# 用前必读
一旦下载和使用本开源项目库及其任何部分（以下简称 YUKI），即代表用户完全理解和同意以下全部协议条款：
<ol>
  <li>用户应当在遵守当地相关法律法规和本协议的前提下使用 YUKI，否则不应使用。</li>
  <li>用户知晓使用 YUKI 可能造成任何后果，包括但不限于：
  <ul><li>被B站限制访问；</li>
      <li>账户被封禁；</li>
      <li>用户数据发生丢失、泄漏或遭到篡改等。</li>
  </ul>
  一切后果均由用户本人承担，与 YUKI 开发者无关；
  </li>
  <li>YUKI 遵循以下规则：
  <ul><li>仅为个人用户提供有限的B站相簿本地备份功能，运作行为与单人使用普通网页浏览器的方式（以下简称“普通浏览方式”）无异；</li>
      <li>仅访问普通浏览方式可获取的公开数据，不会绕开或破解B站的访问限制；</li>
      <li>访问频率控制在与普通浏览方式相当或以下的水准；</li>
      <li>尊重用户隐私，要求用户提供的信息仅用于与B站交互以实现备份功能，不会发送给第三方。</li>
  </ul></li>
  <li>YUKI 不对设备和数据安全做任何担保，请勿在对设备和数据安全有较高要求的环境使用 YUKI。</li>
  <li>用户不得使用 YUKI 进行以下行为：
  <ul><li>规模化的爬虫用途；</li>
      <li>搜集他人隐私信息；</li>
      <li>绕开或破解B站的访问限制；</li>
      <li>解除或松弛对访问频率的控制使之超出普通浏览方式的水平；</li>
      <li>其他超出有限的个人本地备份目的的行为。</li>
  </ul>
  </li>
  <li><a href="./LICENSE">许可证协议</a>构成本协议的一部分。</li>
</ol>

# 使用方法
## 下载链接
根据自己的系统环境，前往[此处](https://github.com/jiangjungit/yuki/releases/latest)下载最新版运行文件。
> 啥是系统环境？如果你不知道这个，也不是用的苹果电脑，那就去下 Windows 版准没错(〃'▽'〃)
## 运行方法
YUKI 有两种运行方式：[交互模式](#交互模式（小白墙裂推荐）)和[参数模式](#参数模式)。

交互模式在执行下载前会等待一次用户确认，参数模式则直接开始下载。

下载开始后若要中断下载请同时按下 `Ctrl`+`C`。

两种模式均在指定路径保存下载的图片和相应动态说明，如果发现图片已存在则不会再重复下载，具体文件组织为：
```
<path_to_save> # 指定的保存路径
      |-<UID> # 指定的 UID
          |-<doc_id_01> # 动态编号
          |      |-description.txt # 该条动态文字说明，第一行为上传时间，后续为内容
          |      |-<img_id_01> # 该条动态的若干图片
          |      |-<img_id_02>
          |      ...
          |
          |-<doc_id_02>
          |      |-description.txt
          |      |-<img_id_01>
          |      |-<img_id_02>
          |      ...
          ...
```
如果运行遇到问题会提示相应错误信息。
### 交互模式（小白墙裂推荐）
直接双击运行（小白墙裂推荐）或通过命令行无参数运行：
```bash
$ cd <workdir> # 进入 YUKI 所在路径
$ ./yuki # 无参数，把 yuki 替换为你下载的版本，如 yuki-0.1.0_Linux_64bit 或 yuki-0.1.0_Windows_32bit.exe
```
根据提示输入相应信息即可。

### 参数模式
通过命令行带参数运行：
```bash
$ cd <workdir> # 进入 YUKI 所在路径
$ ./yuki [args...] # 带参数，把 yuki 替换为你下载的版本，如 yuki-0.1.0_Linux_64bit 或 yuki-0.1.0_Windows_32bit.exe
```
参数说明
- 获取帮助 `-h` ：可选参数，显示功能说明。如若指定，则其他参数均无效，不执行任何下载功能；
- 指定用户 `-u <UID>`：必选参数，指定备份用户的 UID，必须为纯数字；
- 保存路径 `-s <保存路径>`：必选参数，指定保存路径，必须为已存在的文件夹
- 时间设置 `-t <开始时间> <结束时间>`：可选参数，指定搜索时间段，只有在该时段内上传的图片会被下载；未指定时默认下载截止到运行当天上传的全部图片；时间格式为年月日组成的形如 `YYYYMMDD` 的8位数字（例如2021年1月23日为 `20210123`）；开始时间和结束时间必须同时指定。

参数模式示例
- Windows 下，UID 为 `8047632` 的用户备份全部图片到 `C:\Users\yuki\Pictures`
  ```bash
  $ .\yuki-0.1.0_Windows_32bit.exe -u 8047632 -s C:\Users\yuki\Pictures
  ```
- Linux 下，UID为 `8047632` 的用户备份从2021年1月3日至2021年1月5日上传的图片到 `/home/yuki/Pictures`
  ```bash
  $ ./yuki-0.1.0_Linux_64bit -u 8047632 -s /home/yuki/Pictures -t 20210103 20210105
  ```

# 常见问题

### 操作系统版本问题
- > 为什么 Windows 版只有32位的而没有64位的？
   
  为了兼容性，Windows 版发布的是32位版，它在32位机和64位机上都能运行。如果你有兴趣也可以使用源码自行生成64位版，但同样要遵循相关协议。
- > 那为什么 Linux 就只有64位版？
  
  因为用 Linux 的多数是码农哥哥/妹妹，这点问题他们搞得定你就别担心啦(´･ω･)ﾉ(._.`)
- > 那为什么没有 macOS 版？
  
  因为我穷，没有苹果电脑测试(#｀皿´)。欢迎帅气小哥哥和漂亮小姐姐编译提供 macOS 版(●'◡'●)ﾉ

### “查无此人, 一定是你搞错啦”
UID 输入错误。UID 是用户的唯一编号，由纯数字构成。它既不是昵称也不是用户名。查询 UID 的方法：
- 电脑端查询：点击Up头像打开Ta的主页，浏览器地址栏里形如 https://space.bilibili.com/xxxxx 中纯数字部分 xxxxx 就是 UID；
- 手机端查询：我的->设置->账号资料->UID，或者点击Up头像打开个人主页->上部信息栏->详情->UID。

### “这里连文件夹都没建”
设置的保存路径有误。请检查该路径表示的文件夹是否存在，是否有同名文件冲突，是否包含空格、中文或其他特殊字符，是否有写入权限。
> 路径太复杂老输错Σ(っ°Д°;)っ？打开你的文件夹，把地址栏里的内容复制粘贴过来又快又准哦。

### “你看看你输的日期”
时间段指定有误。
- 在交互模式下应按提示输入有效日期，不符合要求时会被重置为其他有效日期，请注意在下载开始前确认设定是否正确；
- 参数模式下应输入两次8位数字有效日期，不符合要求时直接退出运行。

### “xx参数未指定内容/错误/未知选项”
参数指定错误。请按要求设置参数格式和内容。

### “这几个动态ID失败了”
列出的动态ID对应的内容备份失败了，对B站的访问出错或保存文件出错均可能引发该问题。
以 `123456` 为例，用户可以前往地址 https://h.bilibili.com/123456 确认情况，或检查本地文件是否存在问题。

### “未完成全部页面解析”
对B站的访问遇到错误，没有完成解析和备份。此时列出的失败动态ID（如果有的话）列表不完整。

### 检测页面为0
相簿是空的，没有图片可供备份。

### 出现了一堆英文输出
一般对B站的访问出错（如联网失败或遭到封禁）时可能会出现此类情况，它有助于排查错误原因。
### 出现乱码
请注意版本号前输出的字符不一定是乱码，而是彩色图案化的 YUKI 字样（可能显示失败了）。
- 如果你看到了“版本: 0.1.0”的字样（即“版本”二字正常显示），则后续显示功能多数是正常的，使用不受太大影响。
- 如果你看到了“??: 0.1.0”的字样（无法显示“版本”二字），则显示功能不正常（但可能下载功能本身不受影响）。
- YUKI 面向简体中文用户，请确认所用操作系统本地语言已设置为简体中文。

### 卡死了/没有反应
YUKI 作为极其小巧的工具，占用资源很少，几乎不可能让电脑卡死(￣^￣)。
有可能是指定的 UID 没有符合条件的图片可供备份，此时 YUKI 没有输出，正常退出。

### 我遇到的问题比较特别，上面都没列出来
少年你抽卡必中SSR(⊙▽⊙)。
欢迎前往[此处](https://github.com/jiangjungit/yuki/issues)搜索相关问题（调戏 YUKI），
没有满意结果时再[提问](https://github.com/jiangjungit/yuki/issues/new)。

# 为什么叫YUKI
你终于问这个问题了，我都准备好久了٩(๑>◡<๑)۶ ！！！

你知道吗？台风的命名有一个名册，是许多国家一起想的。每新来一个台风就从名册里取用一个名字。
受此启发我也给自己的开源小工具计划造了一个名册。因为我是个动漫宅，所以名字……都来自……

动！漫！角！色！

太羞耻了(..›ᴗ‹..)

YUKI 来自《凉宫春日的忧郁》中的角色**长门有希**（長門 有希，Nagato Yuki），在日语里与“雪”同音（如果你看过你应该知道我在说什么T^T）。

她是“外星人”。一个经典桥段是，在参与社团游戏对战时，她以摄像机都拍出重影的手速现敲C语言，一口气令敌方作弊外挂失效（看哭多少码农啊）。这个名字再适合不过了。

LOGO 就是参考她的形象设计的，你有认出来吗？

# 开发相关
- 本人是C++入门学习者，代码写得很稚嫩，尤其是类和函数的划分比较乱。欢迎大佬提出建议和意见(<ゝω·)☆
- 功能实现：本项目以“提供有限的B站相簿本地备份功能”为原则和中心，谢绝其他功能请求（例如备份视频），拒绝改造为高并发爬虫或破解网站访问控制（[协议](#用前必读)也不准这么做）。以后如若B站相簿关闭公开访问渠道，则本项目随之终止更新。
- 隐私保护：任何改动均应坚持尊重用户隐私的原则，非实现功能的必要信息则不应要求用户提供；原则上不得要求用户提供账户密码，同时避免在本地留下缓存或使用记录。
- 数据安全：本人对网络技术了解不多，仅简单调用 curl 实现访问功能。欢迎修正代码存在的安全漏洞，尽最大努力保护数据安全。
- 调用接口：本项目功能较为简单，故仅以生成可执行文件为目标，不考虑设计可复用的 API。
- 许可协议：YUKI 遵循 [GPL-3.0 License](./LICENSE)。

## 编译链接说明
### 依赖项
- [Curl](https://github.com/curl/curl)
- [JSON for Modern C++](https://github.com/nlohmann/json)（已包含于[json.hpp](./json.hpp)中，无需额外下载安装）
- [CMake](https://cmake.org)

### 推荐环境配置
为了简化跨平台安装依赖项 curl 的流程，强烈推荐使用 [vcpkg](https://github.com/microsoft/vcpkg/)。
无论是在 Windows 下还是 Linux 下都使用相同的安装步骤。

此处示例 Windows 下在 `C:/dev` 下安装 vcpkg，再用 vcpkg 安装 curl 静态库的步骤。
1. 安装 vcpkg
   ```cmd
   cd c:/dev
   git clone https://github.com/microsoft/vcpkg
   ./vcpkg/bootstrap-vcpkg.bat
   ```
2. 安装支持 SSL 的 curl 静态库
   ```cmd
   ./vcpkg/vcpkg install curl[tool]:x64-windows-static # Linux 下换成 x64-linux 即可
   ./vcpkg/vcpkg integrate install
   ```
### 编译链接
此处示例 Windows 下编译链接的步骤（假设已按上一步在 `C:/dev` 借助 vcpkg 安装了 curl），Linux 下步骤基本一致；
命令行工具使用 x64 Native Tools Command Prompt for VS 2017
1. 以源码根目录为工作路径，新建 `build` 文件夹
   ```cmd
   cd <workdir>
   mkdir build
   ```
2. 配置和编译，注意参数 `-DCMAKE_TOOLCHAIN_FILE` 取决于 vcpkg 的安装路径，`-DVCPKG_TARGET_TRIPLET` 取决于系统和 vcpkg 配置
   ```cmd
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=c:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static # 配置
   cmake --build . --config Release # 编译链接
   ./Release/yuki # 运行 YUKI
   ```
   如果要尝试 Debug 版，将上述三处 `Release` 换为 `Debug` 即可。
# 许可协议
### 作者: [jiangjungit](https://github.com/jiangjungit) 和可爱的[贡献者们](https://github.com/jiangjungit/yuki/graphs/contributors)ヾ(❀˘︶˘)ﾉﾞ

### YUKI: [GPL-3.0 License](./LICENSE)

[Curl](https://github.com/curl/curl): [COPYRIGHT AND PERMISSION NOTICE](https://github.com/curl/curl/blob/master/COPYING)

[JSON for Modern C++](https://github.com/nlohmann/json): [MIT License](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT)