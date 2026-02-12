#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "include/TrafficConfig.h"

class EasyDB
{
	public:
	EasyDB()  
	{
		filename = "./db.json";
		load();
	}

	template <typename T>
	bool read(T &t) 
	{
		std::string key = NamedKey<T>()();
		if (m_datas.find(key) != m_datas.end()) {
			std::string val = m_datas[key];
		printf(" 读出 [%s] : %s \n",key.c_str(),val.c_str());
			struct_json::from_json(t,val);
		}
		return true;
	}

	template <typename T>
	void write(const T &t)
	{
		std::string key = NamedKey<T>()();
		std::string value;
		struct_json::to_json(t,value);
		printf(" 写入 [%s] : %s \n",key.c_str(),value.c_str());
		m_datas[key] = value;

		dump();
	}

	void load()
	{
		std::ifstream in_file(filename);
		std::string mp;
		if (in_file.is_open()) {
			// 使用 std::istreambuf_iterator 一次性读取全部内容
			mp = std::string(
					std::istreambuf_iterator<char>(in_file),
					std::istreambuf_iterator<char>()
					);
			in_file.close();

			struct_json::from_json(m_datas,mp);
		}
	}

	void dump()
	{
		std::string mp;
		struct_json::to_json(m_datas,mp);
		printf("Mp : \n %s \n",mp.c_str());

		std::ofstream out_file(filename);
		if (out_file.is_open()) {
			out_file << mp;
			out_file.close();
		}

	}
	private:
		std::map<std::string,std::string> m_datas;
		std::string filename;
};
