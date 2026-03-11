#pragma once
#include "UI/LinearMenu.hpp"

enum class FavoriteRemoveChoice
{
	No = 0,
	Yes = 1,
};

class FavoriteRemoveDialog
{
private:
	std::shared_ptr<noco::Canvas> m_canvas;
	LinearMenu m_menu;
	bool m_isVisible = false;

public:
	explicit FavoriteRemoveDialog(const std::shared_ptr<noco::Canvas>& canvas);

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

	/// @brief 現在の選択内容を取得
	/// @return 選択内容(0:No、1:Yes)
	[[nodiscard]]
	FavoriteRemoveChoice selectedChoice() const;
};
