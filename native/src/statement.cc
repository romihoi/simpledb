#include "simpledb.h"
#include <unistd.h>
#include "log_message.h"

namespace util {
namespace db {

Statement::Statement(const sqlite3* conn)
: conn_(conn) {
}

Statement::Statement(const sqlite3* conn, std::unique_ptr<sqlite3_stmt, std::function<void(sqlite3_stmt *)>> stmt)
: Statement(conn) {
  stmt_ = std::move(stmt);
  assert(stmt_);
}

bool Statement::Execute(const std::string& sql) {
  return ExecuteUpdate(sql);
}

bool Statement::ExecuteUpdate(const std::string &sql) {
  int err;
  err = Prepare(sql);
  if(err) {
    return false;
  }

  for(int ret = 0; (ret = sqlite3_step(stmt_.get())) != SQLITE_DONE;) {
    if(ret == SQLITE_BUSY) {
      sleep(1);
    }
    else {
      return false;
    }
  }
  return true;
}

std::unique_ptr<ResultSet> Statement::ExecuteQuery(const std::string &sql) {
  int err{0};
  err = Prepare(sql);
  if(err) {
    LOG(ERROR) << __func__ << " failed to Prepare()";
    return nullptr;
  }

  std::unique_ptr<ResultSet> result{new ResultSet(std::move(stmt_))};
  return result;
}

int Statement::Prepare(const std::string &sql) {
  sqlite3_stmt *stmt{};
  assert(conn_);
  int err = sqlite3_prepare_v2((sqlite3*)conn_, sql.c_str(), sql.length(), &stmt, NULL);
  if(err) {
    throw std::runtime_error(sqlite3_errmsg((sqlite3*)conn_));
  }

  stmt_ = std::unique_ptr<sqlite3_stmt, std::function<void(sqlite3_stmt *)>> {
    stmt,
    [](sqlite3_stmt *ptr){ if(ptr) sqlite3_finalize(ptr);}
  };

  return err;
}

void update_callback(void *user_data, int operation_type, char const *database, char const *table, sqlite3_int64 rowid) {
  LOG(ERROR) << "operation type: " << operation_type;
  LOG(ERROR) << "database: " << database;
  LOG(ERROR) << "table: " << table;
  LOG(ERROR) << "rowid: " << rowid;
  LOG(ERROR) << "userdata: " << user_data;
}


void Statement::Hook(const std::string &sql) {
  assert(conn_);
  void *param = const_cast<void*>(reinterpret_cast<const void *>(sql.c_str()));
  sqlite3_update_hook((sqlite3*)conn_, update_callback, (void*)param);
}

Statement *Statement::Unref() {
  return new Statement(conn_, std::move(stmt_));
}

} // namespace db

} // namespace util
