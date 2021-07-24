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
//	// �û�ID
//	string user_id;
//	// ͼ������
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
	// �û�ID
	string user_id;
	// ͼ������
	string template_str;
};

class OVSSqliteControl
{
public:
	OVSSqliteControl();
	~OVSSqliteControl();

	// �������ݿ�
	bool OpenSqlite();
	// ��ȡ���ݿ������û���Ϣ
	bool GetUserData(vector<shared_ptr<SSqliteUserInfo>>& in_all_user_info_vector);
	// ����û�
	bool AddUserData(vector<shared_ptr<SSqliteUserInfo>>& in_user_info_vector);
	// ɾ���û�
	bool DeleteUserData(string& in_user_id);
	// �����û�
	bool SelectUserData(string& in_user_id, vector<shared_ptr<SSqliteUserInfo>>& in_user_info_vector);

private:
	// db·��
	const string db_path = "./userdb.db";

	// �û��Ƿ����
	bool b_user_exist;

	// ���ݿ�
	sqlite3* p_db;
};