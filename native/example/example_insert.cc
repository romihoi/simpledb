#include <string>
#include "simpledb.h"
#include "log_message.h"

using namespace util::db;

int insert_default_passwd(const std::string &path) {
  Connection conn(path);
  auto stmt = conn.PrepareStatement("INSERT INTO admin (passwd, date) VALUES(?,datetime(\'now\',\'localtime\'));");
  stmt->Hook("INSERT INTO admin (passwd, date) VALUES(?,datetime(\'now\',\'localtime\'))");

  stmt->SetString(1, "admin_passwd");
  stmt->Execute();

  stmt->SetString(2, "admin_passwd2");
  stmt->Execute();

  stmt->SetString(3, "admin_passwd3");
  stmt->Execute();
  return 0;
}

int main(int argc, char *argv[]) {
  std::string path = "example.db";
  if(argv[1])
    path = argv[1];

  insert_default_passwd(path);
  return 0;
}
