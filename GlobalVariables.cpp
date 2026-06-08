#include "GlobalVariables.h"
#include <KamataEngine.h>
#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include <cassert>
#include <fstream>

GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}

// 指定のオブジェクトがなければ追加する
void GlobalVariables::CreateGroup(const std::string& groupName) { datas_[groupName]; }

// 項目のセット(int)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {

	// グループの参照を取得
	Group& group = datas_[groupName];

	// 新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;

	// 設定した項目をstd;;mapに追加
	group.items[key] = newItem;
}

// 項目のセット(float)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {

	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd;;mapに追加
	group.items[key] = newItem;
}

// 項目のセット(Vetor3)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const KamataEngine::Vector3 value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd;;mapに追加
	group.items[key] = newItem;
}

// 更新処理
void GlobalVariables::Update() {
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginMenuBar())
		return;

	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end(); ++itGroup) {

		// グループ名を取得
		const std::string& groupName = itGroup->first;
		// グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		// 各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items.begin(); itItem != group.items.end(); ++itItem) {

			// 項目名を取得
			const std::string& itemName = itItem->first;

			// 項目の参照を取得
			Item& item = itItem->second;

			// int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item.value)) {

				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);

			} else if (std::holds_alternative<float>(item.value)) {

				float* ptr = std::get_if<float>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), ptr, 0.0f, 100.0f);

			} else if (std::holds_alternative<KamataEngine::Vector3>(item.value)) {

				KamataEngine::Vector3* ptr = std::get_if<KamataEngine::Vector3>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);
			}
		}

		ImGui::Text("\n");

		if (ImGui::Button("Save")) {

			SaveFile(groupName);

			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GroubalVariavles", 0);
		}


		ImGui::EndMenu();
	}

	

	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::SaveFile(const std::string& groupName) {
	// グループを探索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());

	json root;

	root = json::object();

	// jsonオブジェクト登録
	root[groupName] = json::object();

	// 各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin(); itItem != itGroup->second.items.end(); ++itItem) {

		// 項目を取得
		const std::string& itemName = itItem->first;

		// 　項目の参照を取得
		Item& item = itItem->second;

		// int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item.value)) {

			// int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item.value);
		} else if (std::holds_alternative<float>(item.value)) {
			// float型の値を登録
			root[groupName][itemName] = std::get<float>(item.value);
		} else if (std::holds_alternative<KamataEngine::Vector3>(item.value)) {

			// Vector3型のjson配列
			KamataEngine::Vector3 value = std::get<KamataEngine::Vector3>(item.value);
			root[groupName][itemName] = json::array({value.x, value.y, value.z});
		}
	}

	// ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";

	// 書き込むようファイルストリーム
	std::ofstream ofs;

	// 第るを書き込みように開く
	ofs.open(filePath);

	// ファイルオープン失敗？
	if (ofs.fail()) {

		std::string message = "Faild open data file for write. ";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;

	// ファイルを閉じる
	ofs.close();
}
