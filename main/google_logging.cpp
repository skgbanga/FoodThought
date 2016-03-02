#include <glog/logging.h>

int main(int argc, char* argv[]) {
  // Initialize Google's logging library.
  FLAGS_log_dir = "./logs";
  google::InitGoogleLogging(argv[0]);

  auto num_cookies = 20;
  LOG(INFO) << "Found " << num_cookies << " cookies";
}
