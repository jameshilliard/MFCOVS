#include "OVSSqlite.h"

OVSSqlite::OVSSqlite()
{
	p_db = nullptr;
	bool b_result = OpenSqlite();
}

OVSSqlite::~OVSSqlite()
{
	sqlite3_close(p_db);
}

bool OVSSqlite::OpenSqlite()
{
	int res = sqlite3_open(db_path.c_str(), &p_db);
	if (res != SQLITE_OK)
	{
		string tem_str = "¶ÁÈ¡Êý¾Ý¿âÊ§°Ü";
	}

	string sql = "create table if not exists user("
		"user_id text not null,"
		"image_str text not null);";
	char* errstr = nullptr;

	if (sqlite3_exec(p_db, sql.c_str(), nullptr, 0, &errstr) != SQLITE_OK)
	{
		sqlite3_close(p_db);
		sqlite3_free(errstr);
		return true;
	}
	return true;
}

bool OVSSqlite::GetUserData(vector<shared_ptr<SSqliteUserInfo>>& in_all_user_info_vector)
{
	int res = sqlite3_open(db_path.c_str(), &p_db);
	sqlite3_exec(p_db, "BEGIN", 0, 0, 0);
	string sql = "select * from user;";
	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(p_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			shared_ptr<SSqliteUserInfo> p_user_info = make_shared<SSqliteUserInfo>((char*)sqlite3_column_text(stmt, 0), (char*)sqlite3_column_text(stmt, 1));
			in_all_user_info_vector.push_back(p_user_info);
		}
	}
	sqlite3_exec(p_db, "END", 0, 0, 0);
	sqlite3_finalize(stmt);
	sqlite3_close(p_db);
	return true;
}

bool OVSSqlite::AddUserData(vector<shared_ptr<SSqliteUserInfo>>& in_user_info_vector)
{
	int res = sqlite3_open(db_path.c_str(), &p_db);
	for (auto& temp_user_info : in_user_info_vector)
	{
		string strSql = "insert into user(user_id, image_str) values(?,?);";
		sqlite3_stmt* p_stmt;
		int ret = sqlite3_prepare_v2(p_db, strSql.c_str(), strlen(strSql.c_str()), &p_stmt, nullptr);
		if (ret!=SQLITE_OK)
		{
			sqlite3_close(p_db);
			return false;
		}
		sqlite3_bind_text(p_stmt, 1, temp_user_info->user_id.c_str(), strlen(temp_user_info->user_id.c_str()), nullptr);
		sqlite3_bind_text(p_stmt, 2, temp_user_info->template_str.c_str(), 256, nullptr);
		/*sqlite3_bind_text(p_stmt, 1, temp_user_info->user_id.c_str(), strlen(temp_user_info->user_id.c_str()), nullptr);
		sqlite3_bind_text(p_stmt, 2, temp_user_info->template_str, 256, nullptr);*/
		sqlite3_step(p_stmt);
		sqlite3_finalize(p_stmt);
	}
	sqlite3_close(p_db);
	return true;
}

bool OVSSqlite::DeleteUserData(string& in_user_id)
{
	int res = sqlite3_open(db_path.c_str(), &p_db);
	string strSql = "delete from user where user_id = "+in_user_id+";";
	sqlite3_stmt* p_stmt = nullptr;
	int nRes = sqlite3_prepare_v2(p_db, strSql.c_str(), -1, &p_stmt,nullptr);
	if (nRes != SQLITE_OK)
	{
		return false;
	}
	sqlite3_step(p_stmt);
	sqlite3_finalize(p_stmt);
	sqlite3_close(p_db);
	return true;
}

bool OVSSqlite::SelectUserData(string& in_user_id, vector<shared_ptr<SSqliteUserInfo>>& in_user_info_vector)
{
	int res = sqlite3_open(db_path.c_str(), &p_db);
	sqlite3_exec(p_db, "BEGIN", 0, 0, 0);
	string sql = "select * from user where user_id = " + in_user_id + ";";
	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(p_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			shared_ptr<SSqliteUserInfo> p_user_info = make_shared<SSqliteUserInfo>((char*)sqlite3_column_text(stmt, 0), (char*)sqlite3_column_text(stmt, 1));
			in_user_info_vector.push_back(p_user_info);
		}
	}
	sqlite3_exec(p_db, "END", 0, 0, 0);
	sqlite3_finalize(stmt);
	sqlite3_close(p_db);
	return true;
}

