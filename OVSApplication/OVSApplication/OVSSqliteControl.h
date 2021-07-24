/**
*  Sqlite control Functions Declaration
*
*  @author		Xu huabin, Ji renze
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			OVS Sqlite Control API Declaration
*
**/

/**
*
*  Copyright (c) 2020 Qingdao NovelBeam Technology Co., Ltd. All Rights Reserved.
*
*  This software is the confidential and proprietary information of Qingdao NovelBeam Technology Co., Ltd. ("Confidential Information").
*  You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement
*  you entered into with Qingdao NovelBeam Technology Co., Ltd.
*
**/
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <sqlite3.h>
#include <iostream>
#include <fstream>

using namespace std;

//struct SSqliteUserInfo
//{
//	SSqliteUserInfo(
//		string temp_user_id,
//		char* temp_template_str)
//	{
//		this->user_id = temp_user_id;
//		this->template_str = (char*) calloc(256, sizeof(char));
//		memcpy(this->template_str, temp_template_str,256);
//	}
//	// 用户ID
//	string user_id;
//	// 图像数据
//	char* template_str;
//};
//
struct SSqliteUserInfo
{
	SSqliteUserInfo(
		string temp_user_id,
		string temp_template_str)
	{
		this->user_id = temp_user_id;
		this->template_str = temp_template_str;
	}
	// 用户ID
	string user_id;
	// 图像数据
	string template_str;
};

class OVSSqliteControl
{
public:
	OVSSqliteControl();
	~OVSSqliteControl();

	// 连接数据库
	bool OpenSqlite();
	// 获取数据库所有用户信息
	bool GetUserData(vector<shared_ptr<SSqliteUserInfo>>& in_all_user_info_vector);
	// 添加用户
	bool AddUserData(vector<shared_ptr<SSqliteUserInfo>>& in_user_info_vector);
	// 删除用户
	bool DeleteUserData(string& in_user_id);
	// 查找用户
	bool SelectUserData(string& in_user_id, vector<shared_ptr<SSqliteUserInfo>>& in_user_info_vector);

private:
	// db路径
	const string db_path = "./userdb.db";

	// 用户是否存在
	bool b_user_exist;

	// 数据库
	sqlite3* p_db;
};