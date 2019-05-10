#include <QDebug>
#include "debug.h"

namespace rviz
{

RvizDbg::RvizDbg()
{
}

RvizDbg::~RvizDbg()
{
}

void RvizDbg::print(const std::string &msg)
{
	std::string info = "********** [RVIZ DEBUG] ";
	info += msg;
	qDebug() << info.c_str();
}

} // end namespace rviz
