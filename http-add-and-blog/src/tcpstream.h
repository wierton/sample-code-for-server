#ifndef TCP_STREAM_H
#define TCP_STREAM_H

#include <iostream>
#include <streambuf>


class TCPBuf : public std::streambuf {
	int conn;

	static constexpr std::streamsize bufsize = 1024;
	static constexpr std::streamsize putbacksize = 4;
	char *buf;
protected:
	int overflow(int ch) override;
	std::streamsize xsputn(const char *s, std::streamsize n) override;

	int underflow() override;

public:
	TCPBuf();
	TCPBuf(int _conn);
	~TCPBuf();

	TCPBuf(const TCPBuf &) = delete;
	TCPBuf& operator= (const TCPBuf &) = delete;

	TCPBuf(TCPBuf &&);
	TCPBuf& operator= (TCPBuf &&) = delete;

	operator bool();
};

// define as a stream
class TCPStream : public std::iostream {
	TCPBuf tcpbuf;
public:
	TCPStream() = default;
	TCPStream(int conn);
	virtual ~TCPStream() = default;

	TCPStream(const TCPStream &) = delete;
	TCPStream& operator= (const TCPStream &) = delete;

	TCPStream(TCPStream &&);
	TCPStream& operator= (TCPStream &&) = delete;

	operator bool();
};


#endif
