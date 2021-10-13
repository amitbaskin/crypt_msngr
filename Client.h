// built-in warning in boost: ADDRESS_FAMILY is unsigned short and an int is being assigned to it
// boost_1_77_0\boost\asio\detail\impl\win_iocp_socket_service_base.ipp(630,32): 
// warning C4242: '=': conversion from 'int' to 'ADDRESS_FAMILY', possible loss of data
#pragma warning( disable : 4242 )

#include "utils.h"
#include "Keys.h"
#include "Sender.h"
#include "Receiver.h"

#pragma once
class Client {
private:
	const ver_t ver = VER;
	sock_t* sock = nullptr;
	Keys* keys = nullptr;
	Receiver* receiver = nullptr;
	Sender* sender = nullptr;
	clientid_t* myid = nullptr;

	Client(const Client&) = delete;
	Client(const Client&&) = delete;
	Client& operator=(const Client&) = delete;
	Client& operator=(Client&&) = delete;

public:
	Client();
	~Client();
	void connect(const str_t& address, const str_t& port);
	void main();
};
