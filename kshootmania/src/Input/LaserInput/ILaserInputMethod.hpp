#pragma once

/// @brief レーザー入力方式のインタフェース
class ILaserInputMethod
{
public:
	virtual ~ILaserInputMethod() = default;

	/// @brief 入力からカーソルの移動量を取得
	/// @param deltaTimeSec 前フレームからの経過時間(秒)
	/// @return カーソルのX方向移動量
	[[nodiscard]]
	virtual double getDeltaCursorX(double deltaTimeSec) = 0;

	/// @brief 設定が変更され、再構築が必要かどうか
	/// @return 再構築が必要な場合true
	[[nodiscard]]
	virtual bool reconstructionNeeded() const = 0;
};
