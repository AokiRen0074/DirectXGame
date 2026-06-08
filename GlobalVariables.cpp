#include "GlobalVariables.h"
#include <KamataEngine.h>
#ifdef USE_IMGUI
#include <imgui.h>
#endif

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

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}
