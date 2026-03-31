#include "StageManager.h"
#include <fstream>
#include <sstream>

void StageManager::LoadStageDatas() {
	// ステージデータファイルのパス
	const std::string filePath = "Resources/stageDatas.csv";

	// ステージデータファイルを開く
	std::ifstream file(filePath);
	// 正常に開けたかのチェック
	assert(file.is_open() && "ステージデータファイルが存在しません");

	std::stringstream stageDataStream;
	// ファイルの内容をstringstreamにコピーする
	stageDataStream << file.rdbuf();
	// ファイルを閉じる
	file.close();


	std::string line;
	// stringstreamから1行ずつ取り出してlineに入れる
	while (std::getline(stageDataStream, line)) {
		std::stringstream lineStream(line);

		// ステージデータを格納する構造体
		StageData stageData;
		// カンマ区切りの一つ分を格納するstringの宣言
		std::string word;

		// カンマ区切りで次のデータを取得する
		std::getline(lineStream, word, ',');
		// ステージ名を格納する
		stageData.name = word;

		// カンマ区切りで次のデータを取得する
		std::getline(lineStream, word, ',');
		// 整数に変換して制限時間を格納する
		stageData.timeLimit = std::stoi(word);

		// ステージデータテーブルに格納する
		stageDatas_.push_back(stageData);
	}
}

void StageManager::SetCurrentStageIndexByName(const std::string& name) {
	// 全ステージデータを検索
	for (size_t i = 0; i < stageDatas_.size(); ++i) {
		// ステージ名が一致したら現在ステージ番号を設定する
		if (stageDatas_[i].name == name) {
			currentStageIndex_ = static_cast<int32_t>(i);
			return;
		}
	}
	// 見つからなかった場合はエラーで止める
	assert(false && "指定されたステージ名は存在しません");
}