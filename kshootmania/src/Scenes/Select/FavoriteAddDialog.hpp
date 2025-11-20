#pragma once
#include "UI/LinearMenu.hpp"

class FavoriteAddDialog
{
private:
	std::shared_ptr<noco::Canvas> m_canvas;
	LinearMenu m_menu;
	int32 m_maxSelectableNumber;
	bool m_isVisible = false;

public:
	explicit FavoriteAddDialog(const std::shared_ptr<noco::Canvas>& canvas);

	/// @brief ダイアログを表示
	void show();

	/// @brief ダイアログを非表示
	void hide();

	/// @brief ダイアログが表示中かどうか
	/// @return 表示中ならtrue
	[[nodiscard]]
	bool isVisible() const;

	/// @brief 更新処理
	void update();

	/// @brief 楽曲をお気に入りに追加
	/// @param songPath 楽曲への相対パス
	/// @return 追加に成功したらtrue(既に含まれている場合はfalse)
	bool addToFavorite(StringView songPath);

	/// @brief 現在選択中のお気に入り番号を取得
	/// @return お気に入り番号(1-9)
	[[nodiscard]]
	int32 selectedNumber() const;
};
