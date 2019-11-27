# SerialPort
Linux  串口通信。



### 使用方法：



#### 定义接受函数

```c++
void* RX_CallBack(void *arg) {
	std::vector<char> get = *((std::vector<char>*) arg);
	for (auto c : get) std::cout<<c;
	get.clear();
	return NULL;
}
```

#### 串口配置

```c++
	SerialPort UART;
	UART.Config.BaudRate=SerialPort::BR115200;
	UART.Config.DataBits=SerialPort::DataBits8;
	UART.Config.StopBits=SerialPort::StopBits1;
	UART.Config.Parity=SerialPort::ParityNone;
	UART.Config.DevicePath=(char*)&"/dev/ttyUSB2";

```

#### 初始化