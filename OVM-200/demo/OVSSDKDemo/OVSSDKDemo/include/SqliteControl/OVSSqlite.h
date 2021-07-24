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

struct SImageInfo
{
	SImageInfo(
		string temp_user_id,
		string temp_template_str,
		int temp_center_x,
		int temp_center_y,
		double temp_ambiguity,
		double temp_mean_illumination,
		int temp_led_value,
		int temp_palm_distance
		)
	{
		this->user_id = temp_user_id;
		this->template_str = temp_template_str;
		this->center_x = temp_center_x;
		this->center_y = temp_center_y;
		this->ambiguity = temp_ambiguity;
		this->mean_illumination = temp_mean_illumination;
		this->led_value = temp_led_value;
		this->palm_distance = temp_palm_distance;
	}
	// �û�ID
	string user_id;
	// ͼ������
	string template_str;
	// roi����x
	int center_x;
	// roi����y
	int center_y;
	// ģ����
	double ambiguity;

	// ƽ������
	double mean_illumination;
	// led����
	int led_value;
	// ����
	int palm_distance;
};

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

class OVSSqlite
{
public:
	OVSSqlite();
	~OVSSqlite();

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