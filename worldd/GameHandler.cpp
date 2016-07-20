#include "GameHandler.h"
#include "WorldNetHandler.h"
#include "../common/string-util.h"
#include "event/UpdateWorkingStatus.h"

GameHandler::GameHandler(WorldNetHandler* nh, int fd): nh_(nh), fd_(fd)
{}

void GameHandler::handle(int64 uid, string &cmd) {
	if (cmd.substr(0,3)=="ev,") {
		nh_->eq_->pushStringEvent(cmd.substr(3));
	} else if (cmd=="ka") {
		// keep alive message, do nothing
	} else if (cmd.substr(0, 22) == "<policy-stat-request/>") {
		nh_->sendFdString(fd_, "<policy-stat-response/>", 23);
		nh_->closeConnection(fd_);
	} else {
		vector<string> tokens;
		static string delims = ",";
		tokenize(cmd, tokens, delims);
		if (tokens.size()<1) return;
		string &cmd = tokens[0];
		if (checkCmd(cmd, "a", tokens, 2)) {
			int nid = -1;
			if (safe_atoi(tokens[1].c_str(), nid) && uid==-1 && nh_->registerConnection(fd_, nid)) {  // register firstly
				nh_->sendIntSizedFdString(fd_, string("pass"));
				UpdateWorkingStatus::addEvent(nh_->eq_, UpdateWorkingStatus_GW_Conn, nid );
			} else {
				nh_->sendIntSizedFdString(fd_, string("fail"));
			}
		}
	}
}
void GameHandler::leave(int64 nid) 
{
	UpdateWorkingStatus::addEvent(nh_->eq_, UpdateWorkingStatus_GW_Disconn, (int)nid);
}
