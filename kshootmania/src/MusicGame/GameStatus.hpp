#pragma once
#include "kson/ChartData.hpp"
#include "GameDefines.hpp"
#include "Judgment/JudgmentDefines.hpp"
#include "Graphics/GraphicsDefines.hpp"

namespace MusicGame
{
	template <typename T, std::size_t N>
	class AnimRingBuffer
	{
	private:
		std::array<T, N> m_array;

		std::size_t m_cursor = 0U;

	public:
		AnimRingBuffer() = default;

		void push(const T& value)
		{
			m_array[m_cursor] = value;
			m_cursor = (m_cursor + 1U) % N;
		}

		const std::array<T, N>& array() const
		{
			return m_array;
		}
	};

	struct ChipAnimStatus
	{
		double startTimeSec = kPastTimeSec;
		Judgment::ChipAnimType type = Judgment::ChipAnimType::kUnspecified;
	};

	struct ButtonLaneStatus
	{
		double keyBeamTimeSec = kPastTimeSec;
		Judgment::KeyBeamType keyBeamType = Judgment::KeyBeamType::kDefault;

		// チップノーツの判定アニメーション
		AnimRingBuffer<ChipAnimStatus, Graphics::kChipAnimMaxPlaying> chipAnim;

		// 現在ロングノーツが押されているかどうか
		// (ロングノーツが判定ラインに到達するまではボタンを押していてもnoneになる。主に音声エフェクト用)
		Optional<bool> longNotePressed = none;

		// 現在押しているロングノーツの始点のPulse値
		// (ロングノーツが判定ラインに到達するまでにボタンを押した場合でもnoneではなくPulse値になる。主に判定グラフィックス用)
		Optional<kson::Pulse> currentLongNotePulse = none;

		// 直近でキーを押した、または離した時間(秒)
		double currentLongNoteAnimOffsetTimeSec = kPastTimeSec;

		// 最後にチップノーツを判定した時間(FXレーンのみ使用)
		double lastChipJudgedTimeSec = kPastTimeSec;

		// 最後に判定したチップノーツのPulse値(FXレーンのみ使用)
		kson::Pulse lastJudgedChipPulse = kPastPulse;
	};

	struct LaserAnimStatus
	{
		double startTimeSec = kPastTimeSec;

		// はみ出しLASERのアニメーションかどうか
		bool wide = false;

		// 横位置(0.0～1.0)
		double x = 0.0;
	};

	struct LaserLaneStatus
	{
		// プレイヤーのカーソル横位置(0.0～1.0)
		Optional<double> cursorX = none;

		// ノーツ側の理想カーソル横位置(0.0～1.0)
		Optional<double> noteCursorX = none;

		// カーソル横位置がはみ出しLASER用のものか
		bool cursorWide = false;

		// ノーツ側の表示用理想カーソル横位置(0.0～1.0)
		// 
		// LASERの判定タイミングをずらした際にカーソルが表示ノーツからずれないように別途設けている。
		// cursorXがnoteCursorXから一定以内の距離であればnoteVisualCursorXをカーソル表示位置として使用する。
		Optional<double> noteVisualCursorX = none;

		// 現在判定しているLASERセクションの始点のPulse値
		// (クリティカル判定でない場合にもnoneではなくPulse値になる。主に判定グラフィックス用)
		Optional<kson::Pulse> currentLaserSectionPulse = none;

		// 最後に直角LASERを判定した時間
		// (先行判定の場合は直角LASERの時間、後で判定した場合は判定した時間が入る)
		double lastLaserSlamJudgedTimeSec = kPastTimeSec;

		// 最後に判定した直角LASERのPulse値
		kson::Pulse lastJudgedLaserSlamPulse = kPastPulse;

		// 折り返し時・終了時・直角判定時のアニメーション
		AnimRingBuffer<LaserAnimStatus, Graphics::kLaserRippleAnimMaxPlaying> rippleAnim;

		// カーソルがCritical判定範囲内にあるかを返す
		bool isCursorInCriticalJudgmentRange() const
		{
			return cursorX.has_value() && noteCursorX.has_value() && Judgment::IsLaserCursorInCriticalJudgmentRange(cursorX.value(), noteCursorX.value());
		}

		// 描画用のカーソル横位置を取得
		Optional<double> cursorXForDraw() const
		{
			// cursorXが存在しない場合はnoneを返す
			if (!cursorX.has_value())
			{
				return none;
			}

			// noteVisualCursorXまたはnoteCursorXが存在しない場合は、cursorXをそのまま使用
			if (!noteVisualCursorX.has_value() || !noteCursorX.has_value())
			{
				return cursorX.value();
			}

			// カーソルが理想位置に近い場合は、判定調整に影響されない描画用の理想位置を返す
			if (Judgment::IsLaserCursorInAutoFitRange(cursorX.value(), noteCursorX.value()))
			{
				return noteVisualCursorX.value();
			}

			return cursorX.value();
		}
	};

	struct PlayFinishStatus
	{
		double finishTimeSec = kPastTimeSec;
		Achievement achievement = Achievement::kNone;
		IsHardFailedYN isHardFailed = IsHardFailedYN::No;
	};

	/// @brief ゲームステータス
	/// @note ゲームプレイに関与する状態を入れる。表示にしか関与しないものはGameStatusではなくViewStatusへ入れる
	struct GameStatus
	{
		// TODO: 描画に使用するものは完全にViewStatusへ移動する(エディタ上でのプレビュー時にViewStatusさえ構築すればプレビューできるようにする想定)

		bool isPaused = false;
		double currentTimeSec = 0.0;
		double currentTimeSecForButtonJudgment = 0.0;
		double currentTimeSecForLaserJudgment = 0.0;
		double currentTimeSecForAudioProc = 0.0;
		kson::Pulse currentPulse = 0;
		double currentPulseDouble = 0.0;
		kson::Pulse currentPulseForButtonJudgment = 0;
		kson::Pulse currentPulseForLaserJudgment = 0;
		double currentBPM = kDefaultBPM;
		
		std::array<ButtonLaneStatus, kson::kNumBTLanesSZ> btLaneStatus;
		std::array<ButtonLaneStatus, kson::kNumFXLanesSZ> fxLaneStatus;
		std::array<LaserLaneStatus, kson::kNumLaserLanesSZ> laserLaneStatus;

		std::size_t lastPressedLongFXNoteLaneIdx = 0U; // For audio effect parameter priority

		Optional<PlayFinishStatus> playFinishStatus = none;
	};
}
