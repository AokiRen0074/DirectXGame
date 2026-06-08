#pragma once
#include <variant>
#include <KamataEngine.h>
#include<map>
#include<json.hpp>

class GlobalVariables {
public:

	static GlobalVariables* GetInstance();
	void CreateGroup(const std::string& groupName);

	// 値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);

	// 値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);

	// 値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const KamataEngine::Vector3 value);

	// 毎フレーム更新処理
	void Update();

	// ファイルに書き出し
	void SaveFile(const std::string& groupName);

	private:

		// コンストラクタ
		GlobalVariables() = default;
		
		// デストラクタ
	    ~GlobalVariables() = default;

		// コピーコンストラクタ
	    GlobalVariables(const GlobalVariables&) = delete;

		//　コピー代入演算子
	    GlobalVariables& operator=(const GlobalVariables&) = delete;

		struct Item {
			// 項目の値
		    std::variant < int32_t, float, KamataEngine::Vector3 > value;
		};

		// グループ
		struct Group {
		    std::map<std::string, Item> items;
		    std::map<std::string, Item> key;
		};

		// 全データ
	    std::map<std::string, Group> datas_;

		using json = nlohmann::json;

		// グローバル変数の保存先ファイルパス 
		const std::string kDirectoryPath = "Resources/GlobalVariables/";


};
