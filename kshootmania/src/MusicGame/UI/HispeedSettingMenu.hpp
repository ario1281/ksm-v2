#pragma once
#include "MusicGame/Scroll/HispeedSetting.hpp"
#include "MusicGame/Scroll/HighwayScroll.hpp"

namespace MusicGame
{
	/// @brief ハイスピード設定のメニュー
	class HispeedSettingMenu
	{
	private:
		ArrayWithLinearMenu<HispeedType> m_typeMenu;
		LinearMenu m_valueMenu;

		double m_stdBPM = kDefaultBPM;

		int32 m_currentHispeed = 0;

		void refreshValueMenuConstraints();

		void setHispeedSetting(const HispeedSetting& hispeedSetting);

	public:
		/// @brief コンストラクタ
		/// @param availableHispeedTypes 使用可能なハイスピードの種類
		/// @param hispeedSetting 初期ハイスピード設定
		/// @param stdBPM O-mod用の基準BPM
		/// @param initialBPM 初期BPM(m_currentHispeedの初期化に使用)
		HispeedSettingMenu(const Array<HispeedType>& availableHispeedTypes, const HispeedSetting& hispeedSetting, double stdBPM, double initialBPM);

		/// @brief 毎フレームの更新
		/// @param currentBPM 現在のBPM(最も近い値でtypeを切り替えるために使用)
		/// @return 値に変更があった場合はtrueを返す
		bool update(double currentBPM);

		/// @brief ConfigIniから読み込んでメニューに反映
		/// @remarks ハイスピード設定の表示/非表示設定に更新があっても反映されない点に注意
		void loadFromConfigIni();

		/// @brief ConfigIniへメニューの状態を保存
		void saveToConfigIni() const;

		/// @brief 現在の値をHispeedSetting構造体として取得
		/// @return HispeedSetting構造体
		HispeedSetting hispeedSetting() const;
	};
}
