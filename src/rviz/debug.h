#ifndef RVIZ_DEBUG_H
#define RVIZ_DEBUG_H

#include <string>

namespace rviz
{

class RvizDbg
{
public:
  RvizDbg();
  virtual ~RvizDbg();
  
public:
  static void print(const std::string &msg);
};

} // end namespace rviz

#endif // RVIZ_DEBUG_H
