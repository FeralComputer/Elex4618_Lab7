///////////////////////////////////////////////////////////////////
// Prepared for BCIT ELEX4618, April 2017, by Craig Hennessey
///////////////////////////////////////////////////////////////////
#pragma once

class Server
{
private:
	int _port;
  bool _exit;

public:
  Server(int port);
	~Server();

  void start();
};

