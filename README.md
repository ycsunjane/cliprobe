介绍
=======

cliprobe 用于探测网络中的wifi客户，无需用户链接，进入网络后主动探测。并根据多台ap的反馈信号，计算出设备在场地内的坐标。

核心代码:

ap等路由系统上的报文抓取（未开源）
server端根据汇报数据计算client位置

对应web:

一、基于用户做统计
1、用户mac地址
2、设备类型
3、首次到场时间
4、最近到场时间
5、平均每次在场时间与在场总时间
6、指定时间用户活跃排名（自选默认）

二、基于场地做统计
1、用户mac地址，用户到场时间，
1、场地每天总人数
2、平均在店时间
3、在场人数在不同时间的曲线（采样时间自选）
4、用户活跃区域图

三、报表
基于以上统计信息，每天，每周，每月，每季度出统计表


编译执行
--------------------

make
只生成client

make server
生成服务端

运行
--------------------

客户端

客户端运行在任何支持monitor mode的机器上。不仅仅可以运行在路由器。
检查是否支持:`iw phy`查看`Supported interface modes:`

```
./cliprobe 
USAGE: probe wlan0 mon0 server minsig debug
e.g:
./cliprobe wlan0 mon0 192.168.10.119 70 0
```
minsig: 最小关心信号强度，注意没有负号

服务端

配置文件: `/etc/clipos/serprobe.conf`
ap位置配置文件: `/etc/clipos/ap.conf`

```
[jianxi@Jianxi cliprobe]$ ./server/serprobe  --help
USAGE: serprobe [options]
  -a, --apconf           ap position config file (default /etc/clipos/ap.conf)
  -t, --agetime          ap report age time (default 5s)
  -l, --losttime         client lost age time (default 300s)
  -i, --dis              sample distance  (default 1m)
  -s, --sig              sample distance signal (default 53)
  -f, --fac              default factory 2.5 ~ 5 (default 4.5)
  -r, --drift            default drift (default 1)
  -m, --minsig           min signal get in calculate (default -70)
  -d, --daemon           daemon mode
  -b, --debug            enable debug will auto disable daemon_mode
  --help                 help info
```

正常参数不需要修改， 长期修改请修改配置文件。
正式部署时，请启动daemon 模式 `serprobe -d`

数据库
-----------------------

mysql数据库, 数据库配置使用`my.cnf`, 使用`[serprobe]` section.

数据库：cliprobe 表: clipos, user

climac： 8字节存储，后6字节为mac地址

macstr: mac地址字符串表示形式

x，y，z: 坐标

time: 时间戳

first_time: 客户端首次出现时间

last_time: 客户端最近一次出现时间

```sql
CREATE TABLE IF NOT EXISTS clipos (
	climac BIGINT NOT NULL,
	macstr CHAR(17) NOT NULL, 
	x DOUBLE, 
	y DOUBLE, 
	z DOUBLE, 
	time TIMESTAMP
);

CREATE TABLE IF NOT EXISTS  user (
	climac BIGINT NOT NULL,
	macstr CHAR(17) NOT NULL,
	first_time DATETIME NOT NULL,
	last_time DATETIME NOT NULL,
	primary key (climac)
);
```

存储
------------

* 8 + 17 + 8 + 8 + 8 + 4 = 53B
* 活跃信息，每2秒一条记录
* 程序设计最大用户10000人
* 每天开业12小时

综上:

12 * 3600 / 2 * 10000 * 53 = 11,448,000,000B  (11,179,687KB, 10,917.7MB, 10.7GB)

