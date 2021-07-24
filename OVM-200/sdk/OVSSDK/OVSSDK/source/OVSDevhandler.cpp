#include "OVSDevhandler.h"

DevHandler::DevHandler()
{
	p_ovs_control = make_shared<OVSDevice>();
}


DevHandler::~DevHandler()
{

}


bool DevHandler::Init()
{
	if (p_ovs_control->DeviceInit())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::UnInit()
{
	if (p_ovs_control->CloseDevice() == S_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::Set_Exposure(const int val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Set_Exposure(temp_str) == S_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::Get_Exposure(int& val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Get_Exposure(temp_str) == S_OK)
	{
		val = stoi(temp_str);
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::Set_LEDCurrent(const int val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Set_LEDCurrent(temp_str) == S_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::Get_LEDCurrent(int& val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Get_LEDCurrent(temp_str) == S_OK)
	{
		val = stoi(temp_str);
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::Set_BLOpen(const bool flag)
{
	if (p_ovs_control->Set_BL(flag) == S_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
} 

bool DevHandler::Get_BL(int& val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Get_BL(temp_str) == S_OK)
	{
		val = stoi(temp_str);
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::Set_Gain(int val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Set_Gain(temp_str) == S_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool DevHandler::Get_Gain(int& val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Get_Gain(temp_str) == S_OK)
	{
		val = stoi(temp_str);
		return true;
	}
	else
	{
		return false;
	}
}


bool DevHandler::Set_CompressionRatio(int val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Set_CompressionRatio(temp_str) == S_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool DevHandler::Get_CompressionRatio(int& val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Get_CompressionRatio(temp_str) == S_OK)
	{
		val = stoi(temp_str);
		return true;
	}
	else
	{
		return false;
	}
}

bool DevHandler::Get_Distance(int& val)
{
	string temp_str = DecToString(val);
	if (p_ovs_control->Get_Distance(temp_str) == S_OK)
	{

		val = stoi(temp_str, nullptr, 16);
		return true;
	}
	else
	{
		return false;
	}
}

string DevHandler::DecToString(int iNum)
{
	int m = iNum;
	char s[8];
	char ss[8];
	int i = 0, j = 0;
	if (iNum < 0)// 处理负数
	{
		m = 0 - m;
		j = 1;
		ss[0] = '-';
	}
	while (m > 0)
	{
		s[i++] = m % 10 + '0';
		m /= 10;
	}
	s[i] = '\0';
	i = i - 1;
	while (i >= 0)
	{
		ss[j++] = s[i--];
	}
	ss[j] = '\0';

	return ss;
}

