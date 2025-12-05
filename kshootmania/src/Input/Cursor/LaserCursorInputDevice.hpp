#pragma once
#include "ICursorInputDevice.hpp"

class LaserCursorInputDevice : public ICursorInputDevice
{
private:
	int32 m_laneIdx;
	double m_accumulatedDelta = 0.0;
	int32 m_deltaCursor = 0;
	int32 m_movementCount = 0;
	Stopwatch m_idleStopwatch{ StartImmediately::Yes };

public:
	/// @brief コンストラクタ
	/// @param laneIdx レーンのインデックス(0=左LASER, 1=右LASER)
	explicit LaserCursorInputDevice(int32 laneIdx);

	virtual ~LaserCursorInputDevice();

	/// @brief フレーム毎の更新(毎フレーム、deltaCursor使用前に呼ぶ)
	virtual void update() override;

	/// @brief 現在フレームでのカーソル移動数を返す
	/// @return カーソル移動数(移動していない場合は0)
	virtual int32 deltaCursor() const override;
};
