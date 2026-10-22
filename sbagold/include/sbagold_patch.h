#pragma once
#include <cmath>
namespace sbagold {
struct State { bool jump=false; bool hover=false; bool noclip=false; float speed=1.f; };
inline bool valid_speed(float x) { return std::isfinite(x) && x>=0.25f && x<=3.f; }
inline bool set(State& s,int feature,float value) {
  if(!std::isfinite(value)) return false;
  switch(feature) {
    case 1: s.hover=value>0.5f; return true;
    case 2: s.jump=value>0.5f; return true;
    case 3: s.noclip=value>0.5f; return true;
    case 4: if(!valid_speed(value))return false; s.speed=value; return true;
    default:return false;
  }
}
inline void reset(State& s){s=State{};}
inline bool jump_allowed(bool original,const State& s){return original||s.jump;}
inline float gravity(float original,const State& s){return s.hover?0.f:original;}
inline bool collision(bool original,const State& s){return s.noclip?false:original;}
}
