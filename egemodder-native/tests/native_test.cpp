#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
extern "C" int egemodder_apply_game_time_scale(float);
extern "C" int egemodder_set_feature(int,float);
extern "C" int egemodder_hook_state();
extern "C" const char* egemodder_time_scale_status(int);
extern "C" float fake_last_scale();
extern "C" int fake_call_count();
extern "C" void fake_raise_exception(int);
#define CHECK(expr) do { if (!(expr)) { std::cerr << "FAILED " << __FILE__ << ":" << __LINE__ << ": " #expr "\n"; return EXIT_FAILURE; } } while (false)
int main() {
  CHECK(egemodder_hook_state() == 0);
  CHECK(egemodder_set_feature(0,1.0f) == -1);
  CHECK(egemodder_set_feature(1,1.0f) == 0);
  CHECK(egemodder_set_feature(1,0.0f) == 0);
  CHECK(egemodder_set_feature(2,1.0f) == 0);
  CHECK(egemodder_set_feature(3,1.0f) == 0);
  CHECK(egemodder_set_feature(4,0.24f) == -1);
  CHECK(egemodder_set_feature(4,3.01f) == -1);
  CHECK(egemodder_set_feature(4,1.5f) == 0);
  CHECK(egemodder_set_feature(4,std::numeric_limits<float>::quiet_NaN()) == -1);

  CHECK(egemodder_apply_game_time_scale(std::numeric_limits<float>::quiet_NaN()) == -1);
  CHECK(egemodder_apply_game_time_scale(std::numeric_limits<float>::infinity()) == -1);
  CHECK(egemodder_apply_game_time_scale(0.24f) == -1);
  CHECK(egemodder_apply_game_time_scale(3.01f) == -1);
  CHECK(fake_call_count() == 0);
  CHECK(egemodder_apply_game_time_scale(0.25f) == 0);
  CHECK(std::fabs(fake_last_scale() - 0.25f) < 0.0001f);
  CHECK(egemodder_apply_game_time_scale(1.5f) == 0);
  CHECK(std::fabs(fake_last_scale() - 1.5f) < 0.0001f);
  CHECK(egemodder_apply_game_time_scale(3.0f) == 0);
  CHECK(fake_call_count() == 3);
  fake_raise_exception(1);
  CHECK(egemodder_apply_game_time_scale(1.0f) == -7);
  CHECK(fake_call_count() == 4);
  CHECK(std::strstr(egemodder_time_scale_status(-7), "exception") != nullptr);
  std::cout << "PASS: validation, method binding, args, exception status\n";
}
