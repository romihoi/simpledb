#include <string>
#include "simpledb.h"
#include "log_message.h"
// #include <glog/logging.h>

using namespace util::db;

static int delete_row_by_id(const std::string &path, int id) {
  Connection conn(path);
  auto stmt = conn.PrepareStatement("DELETE FROM admin WHERE idx=?;");
  stmt->SetInt(1, id);
  stmt->Hook("DELETE FROM admin WHERE idx=?;");
  stmt->Execute();
  return 0;
}

int main(int argc, char *argv[]) {
  std::string path = "example.db";
  if(argv[1])
    path = argv[1];

  delete_row_by_id(path, 1);
  return 0;
}
