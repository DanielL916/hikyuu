/*
 * MySQLConnect.cpp
 *
 *  Copyright (c) 2019, hikyuu.org
 *
 *  Created on: 2019-8-17
 *      Author: fasiondog
 */

#include "MySQLConnect.h"

namespace hku {

MySQLConnect::MySQLConnect(const Parameter& param) : DBConnectBase(param), m_mysql(nullptr) {
    m_mysql = new MYSQL;
    HKU_CHECK(m_mysql, "Failed new MYSQL instance!");
    HKU_CHECK(mysql_init(m_mysql) != NULL, "Initial MySQL handle error!");

    string host = getParamFromOther<string>(param, "host", "127.0.0.1");
    string usr = getParamFromOther<string>(param, "usr", "root");
    string pwd = getParamFromOther<string>(param, "pwd", "");
    string database = getParamFromOther<string>(param, "db", "hku_base");
    unsigned int port = getParamFromOther<int>(param, "port", 3306);
    // HKU_TRACE("MYSQL host: {}", host);
    // HKU_TRACE("MYSQL port: {}", port);
    // HKU_TRACE("MYSQL database: {}", database);

    my_bool reconnect = 1;
    HKU_CHECK(mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect) == 0,
              "Failed set reconnect options");
    HKU_CHECK(mysql_real_connect(m_mysql, host.c_str(), usr.c_str(), pwd.c_str(), database.c_str(),
                                 port, NULL, CLIENT_MULTI_STATEMENTS) != NULL,
              "Failed to connect to database! {}", mysql_error(m_mysql));
    HKU_CHECK(mysql_set_character_set(m_mysql, "utf8") == 0, "mysql_set_character_set error!");
}

MySQLConnect::~MySQLConnect() {
    if (m_mysql) {
        mysql_close(m_mysql);
        delete m_mysql;
    }
}

bool MySQLConnect::ping() {
    return m_mysql ? mysql_ping(m_mysql) == 0 : false;
}

void MySQLConnect::exec(const string& sql_string) {
    HKU_CHECK(m_mysql, "mysql connect is not open!");
    int ret = mysql_query(m_mysql, sql_string.c_str());
    HKU_CHECK(ret == 0, "SQL error： {}! error code：{}", sql_string, ret);
}

SQLStatementPtr MySQLConnect::getStatement(const string& sql_statement) {
    return make_shared<MySQLStatement>(shared_from_this(), sql_statement);
}

bool MySQLConnect::tableExist(const string& tablename) {
    return false;
}

}  // namespace hku
