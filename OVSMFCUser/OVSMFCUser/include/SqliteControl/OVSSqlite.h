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
	// 用户ID
	string user_id;
	// 图像数据
	string template_str;
	// roi区域x
	int center_x;
	// roi区域y
	int center_y;
	// 模糊度
	double ambiguity;

	// 平均亮度
	double mean_illumination;
	// led亮度
	int led_value;
	// 距离
	int palm_distance;
};

struct SSqliteUserInfo
{
	SSqliteUserInfo(
		string temp_user_id,
		vector<float> temp_template_str)
	{
		this->user_id = temp_user_id;
		this->template_str = temp_template_str;
	}
	// 用户ID
	string user_id;
	// 图像数据
	vector<float> template_str;
};

class OVSSqlite
{
public:
	OVSSqlite();
	~OVSSqlite();

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