#include "PolicyHandler.h"
#include "GameNetHandler.h"

string PolicyHandler::policy_content;

PolicyHandler::PolicyHandler(GameNetHandler *nh, int fd)
{
	this->nh = nh;
	this->fd = fd;
	PolicyHandler::policy_content = \
		string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
			   <cross-domain-policy>\
			   <allow-access-from domain=\"*\" to-ports=\"*\" secure=\"false\" />\
			   </cross-domain-policy>");
}

PolicyHandler::~PolicyHandler(void)
{
}

int PolicyHandler::handlerType()
{
	return ProtocolHandler::POLICY;
}

void PolicyHandler::handle(int64 uid, string &req)
{
	processPolicy();
}

void PolicyHandler::processPolicy()
{
	nh->sendFdString(fd, policy_content.c_str(), policy_content.size());
	nh->closeConnection(fd);
}

void PolicyHandler::leave(int64 uid)
{
}
