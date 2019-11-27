/*
 * 	串口通讯实现
 * 	spray0 2019-11-27
 *
 * 	-支持串口关闭开启
 * 	-接收使用线程处理，定义　void　*　RX_CallBack(void *arg)　函数即可强引用编译,参数为vector<char>
 * 	-发送支持３种输入类型：char,vector<char>,char*
 */
#include "SerialPort.h"

//接收回调函数
void* __attribute__((weak)) RX_CallBack(void *arg) {

	std::vector<char> get = *((std::vector<char>*) arg);
	for (auto c : get)
		printf("%c", c);
	get.clear();
	return NULL;
}

//监听线程　读取的数据存放在容器
void* Listen(void *arg) {

	int get;
	int fd = fd = *((int*) arg);
	std::vector<char> RX_buf(128);
	while (1) {
		get = read(fd, &RX_buf[0], 128);
		if (get > 0) {
			pthread_t process_thread;
			pthread_create(&process_thread, NULL, RX_CallBack, &RX_buf);
			pthread_detach(process_thread);
		}
	}
	return NULL;
}

//发送单个字节
bool SerialPort::Send(char byte) {
	return (write(this->fd, &byte, 1) == -1) ? false : true;
}
bool operator <<(SerialPort port, char byte) {
	return (write(port.fd, &byte, 1) == -1) ? false : true;
}
//发送多个字节
bool SerialPort::Send(std::vector<char> data) {
	return (write(this->fd, &data[0], data.size()) == -1) ? false : true;
}
bool operator <<(SerialPort port, std::vector<char> data) {
	return (write(port.fd, &data[0], data.size()) == -1) ? false : true;
}
//发送多个字节
bool SerialPort::Send(char *data, unsigned int len) {
	return (write(this->fd, data, len) == -1) ? false : true;
}
bool operator <<(SerialPort port, char const *data) {
	return (write(port.fd, data, strlen(data)) == -1) ? false : true;
}

//打开串口
bool SerialPort::Open() {
	//打开串口
	this->fd = open(this->Config.DevicePath, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (this->fd == -1)
		return false;
	if (fcntl(this->fd, F_SETFL, 0) < 0)
		return false;
	if (isatty(STDIN_FILENO) == 0)
		return false;

	//清空缓存
	tcflush(fd, TCIFLUSH);
	fcntl(fd, F_SETFL, 0);

	//开启监听线程
	pthread_create(&this->listen_thread, NULL, Listen, &this->fd);
	pthread_detach(this->listen_thread);

	return true;
}

//关闭串口
void SerialPort::Close() {
	//关闭串口入口
	close(this->fd);
	//关闭监听线程
	pthread_cancel(this->listen_thread);
}

//配置串口
bool SerialPort::LoadConfig() {
	//设置参数
	struct termios newtio, oldtio;
	if (tcgetattr(fd, &oldtio) != 0)
		return false;
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	//DataBits
	switch (this->Config.DataBits) {
	case this->DataBits7:
		newtio.c_cflag |= CS7;
		break;
	case this->DataBits8:
		newtio.c_cflag |= CS8;
		break;
	}

	//Parity
	switch (this->Config.Parity) {
	case this->ParityEven:
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case this->ParityNone:
		newtio.c_cflag &= ~PARENB;
		break;
	case this->ParityOdd:
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	}

	//BaudRate
	cfsetispeed(&newtio, this->Config.BaudRate);
	cfsetospeed(&newtio, this->Config.BaudRate);

	//StopBits
	switch (this->Config.StopBits) {
	case this->StopBits1:
		newtio.c_cflag &= ~CSTOPB;
		break;
	case this->StopBits2:
		newtio.c_cflag |= CSTOPB;
		break;
	}

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;
	tcflush(fd, TCIFLUSH);
	fcntl(fd, F_SETFL, 0);
	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
		return false;

	return true;
}
