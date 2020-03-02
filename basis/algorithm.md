# 算法

## 排序算法
+ 冒泡排序
    + 快速排序算法
    + 鸡尾酒排序算法
+ 插入排序
    + 二分插入排序
    + 希尔排序
+ 归并排序
+ 堆排序
+ 选择排序
### 快速排序
通过一趟排序将要排序的数据分割成独立的两个部分，其中一部分比另一部分所有数据都小，然后再按此法对着两部分分别排序。整个排序可以递归并行，最终有序。
有些像堆排序
### 鸡尾酒排序
即定向冒泡排序/搅拌排序，先找最小数放在第一位，再找最大数放在最后一位；依次类推O(n^2)，接近O(n)有序情况

### 堆排序
利用堆积树

### 比较
std的sort()在数据量大的时候使用快速排序，效率为O(log n);
分段后的数据量小于某阈值，改成插入排序，此时基本有序，所以复杂度可达O(n)；
在递归过程中，如果递归层次过深，分割有恶化倾向，则侦测出来后，使用堆排序处理，效果维持在O(n * log n);

## 加密算法
即Encryption Algorithm。
分为对称式加密算法和非对称式加密算法。

数学符号： S = {P, C, K, E, D},P - 明文空间; C - 密文空间; K - 密钥空间（变参）; E - 加密算法（公式/法则/程序）; D - 解密算法(E的逆);
C = Ek(P)，对明文加密后得到密文C;
P = Dk(C)，对密文解密后得到明文P;
Ek = D(k-1); Dk = E(k-1);(互为逆)
设计加密算法主要是确定 E/D/K
### 对称加密
加密解密使用相同的密钥。速度快，通常在发送方需要加密大量数据时使用。
加密的安全性不止取决于加密算法，密钥的管理也很重要。
如何把密钥安全地传递到解密者成了一个问题。

常见算法：DES系列（DES 3DES） AES RC系列（RC2 RC4 RC5）

优点：算法公开、计算量小、加密速度快、加密效率高

问题：
1.密钥本身如何传递
2.每对用户都需要使用独立的密钥，密钥管理成为双方的负担

### 非对称加密
需要使用不同的密钥来做加密和解密，公开密钥：Public Key；私有密钥：Private Key;
使用公钥加密的需要使用私钥才能解密；如果使用私钥加密的使用公钥才能解密；

常见算法：RSA Elgarral 背包算法 Rabin D-H ECC（椭圆曲线加密算法）

优点：安全性更好，无需同步私钥

问题：加解密花费时间长，速度慢，只适合少量数据加密。

## 摘要算法
即Message-Digest算法，不能称为加密算法，不需要密钥，也不能解密。
主要应用在“数字签名”领域。
只有算法，不存在密钥管理和分发，适合分布式网络内部互信使用。
算法工作量大，只用于有限数据的加密，如计算机的口令就是用不可逆加密算法加密的。

+ 摘要的数据不可破解，主要用于“数字签名”领域
+ 把任意长度的输入糅合成长度固定的伪随机数的算法

- 输入不定长
- 看起来随机
- 输入不同，输出不同
- 消息摘要算法是无陷门的单向函数
- 好的摘要算法，无人能找到“碰撞”（实际碰撞肯定存在？是，无限输入，有限输出）
也即存在两条消息，其摘要相同，却很难找到

常见算法：MD系列(MD5) SHA家族(SHA-1、SHA-224、SHA-256、SHA-384，和SHA-512)
```
md message digest algorithm
sha secure hash algorithm
```
一般地，把一个信息的摘要成为该消息的指纹或者数字签名、数字签章。
数字签名是保证信息完整性和不可否认的方法。应具有唯一性，不可伪造。目的：签名/验证

### 数字签名
+ 1.A对外发布公钥，持有私钥
+ 2.A对消息M进行计算摘要，得到摘要D
+ 3.A使用私钥对D签名，得到签名S
+ 4.将M和S一起发送给对方
+ 5.验证
    + 1.接收者首先对M使用摘要算法得到摘要D
    + 2.使用A的公钥对S解签，得到D'
    + 3.如果D和D'相同，就证明M是A发送的，且没有篡改过
使用场景：网站认证、代码签名、比特币（完全匿名的货币，身份认证基于ECDSA）

有一个可信任的签名验证服务，提供公钥和文件摘要的验证？

## 分布式ID生成算法
uuid 128太长，zk性能不满足，snowflake需要多研究
### uuid算法

### snowflake算法
#### 自定义
[39bit毫秒数][4bit业务线id][2bit机房id][7bit机器id][5bit预留][7bit毫秒内序号]
每台服务器绝对递增，全局看只是趋势递增
必然按msg-id/order-id/worker-id分库分表，取模运算要求ID生成具有“取模随机性",所以毫秒内序列号放最后，保证ID随机
跨毫秒时，序号总是归0，序号尾为0的比较多，生成的ID取模不均匀，所以不是每次归零，而是归到0-9的随机数。

#### snowflake算法
[1bit不用][41bit时间戳][10bit机器ID][12bit序列号]
第一比特保留
时间戳，41比特，从2016年11月1日零点到现在的毫秒数,可以用到2156年，100多年后才会用完
机器id，10比特，这个机器id每个业务要唯一，机器id获取的策略后面会详述
序列号，12比特，每台机器每毫秒最多产生4096个id，超过这个数的话会等到下一毫秒