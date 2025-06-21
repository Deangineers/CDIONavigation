//
// Created by localadmin on 21/06/2025.
//

#ifndef OBJECTCOUNTER_H
#define OBJECTCOUNTER_H
#include <string>
#include <unordered_map>


class ObjectCounter
{
public:
  static void objectDetected(const std::string& label);
  static void reset();
  static int getCount(std::string label);
private:
  static inline std::unordered_map<std::string,int> map_;
};



#endif //OBJECTCOUNTER_H
