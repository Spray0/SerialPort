# SerialPort
![cpp](https://img.shields.io/badge/Use-C++-red) ![ttt](https://img.shields.io/badge/OS-Linux-blue) ![ver](https://img.shields.io/badge/Verison-1.1-green)



### 使用方法：



#### 串口配置

```c++
SerialPort UART;
UART.Config.BaudRate=SerialPort::BR115200;
UART.Config.DataBits=SerialPort::DataBits8;
UART.Config.StopBits=SerialPort::StopBits1;
UART.Config.Parity=SerialPort::ParityNone;
UART.Config.DevicePath=(char*)&"/dev/ttyUSB0";

```

#### 定义接受函数（强引用）

```c++
void RxData_CallBack(std::vector<unsigned char> &data, int fd) {
	if(fd == UART.fd){
		printf("\033[1;36;40m%c\033[0m", byte); //以青色打印在终端
	}
}
```

#### 初始化

```c++
if(UART.Open()==false)printf("OPEN error!\n");
	else printf("OPEN OK!\n");
if(UART.LoadConfig()==false)printf("Set error!\n");
	else printf("Set OK!\n");
```

#### 发送

```c++
std::vector<char> txdata;
txdata.push_back('s');
txdata.push_back('p');
txdata.push_back('\r');

UART<<txdata;
UART<<'\n';
UART<<"Hello spray0!\r\n";
```

