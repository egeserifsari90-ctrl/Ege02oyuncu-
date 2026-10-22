#include "sbagold_patch.h"
#include <cassert>
#include <cmath>
#include <limits>
int main() {
 sbagold::State s;
 assert(!s.jump&&!s.hover&&!s.noclip&&s.speed==1.f);
 assert(sbagold::jump_allowed(true,s)&&!sbagold::jump_allowed(false,s));
 assert(sbagold::gravity(-9.8f,s)==-9.8f);
 assert(sbagold::collision(true,s));
 assert(sbagold::set(s,2,1)&&sbagold::jump_allowed(false,s));
 assert(sbagold::set(s,1,1)&&sbagold::gravity(-9.8f,s)==0.f);
 assert(sbagold::set(s,3,1)&&!sbagold::collision(true,s));
 assert(!sbagold::set(s,4,0.24f));
 assert(!sbagold::set(s,4,3.01f));
 assert(!sbagold::set(s,4,std::numeric_limits<float>::quiet_NaN()));
 assert(!sbagold::set(s,4,std::numeric_limits<float>::infinity()));
 assert(sbagold::set(s,4,2)&&s.speed==2.f);
 assert(!sbagold::set(s,99,1));
 sbagold::reset(s);
 assert(!s.jump&&!s.hover&&!s.noclip&&s.speed==1.f);
 return 0;
}
