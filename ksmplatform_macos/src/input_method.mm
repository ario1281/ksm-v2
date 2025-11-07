#include "ksmplatform_macos/input_method.h"
#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <CoreGraphics/CoreGraphics.h>

void KSMPlatformMacOS_DisableIME()
{
	// ASCII入力ソースのリストを取得
	CFArrayRef asciiSources = TISCreateASCIICapableInputSourceList();
	if (asciiSources == NULL)
	{
		return;
	}

	if (CFArrayGetCount(asciiSources) > 0)
	{
		// 最初のASCII入力ソースを選択(通常は英語キーボード)
		TISInputSourceRef asciiSource = static_cast<TISInputSourceRef>(
			const_cast<void*>(CFArrayGetValueAtIndex(asciiSources, 0))
		);
		TISSelectInputSource(asciiSource);
	}

	CFRelease(asciiSources);
}

bool KSMPlatformMacOS_IsKeyPressed(int keyCode)
{
	// CGKeyCodeは0〜127の範囲なので、範囲チェック
	if (keyCode < 0 || keyCode > 127)
	{
		return false;
	}

	return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, static_cast<CGKeyCode>(keyCode));
}

namespace
{
	id s_eventMonitor = nil;
	bool s_isEditingText = false;
}

void KSMPlatformMacOS_StartBlockingIMEKeys()
{
	// 既に開始している場合は何もしない
	if (s_eventMonitor != nil)
	{
		return;
	}

	// NSEventのローカルモニターを追加
	// flagsChangedも監視してmodifierキーと組み合わせた場合にも対応
	s_eventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:(NSEventMaskKeyDown | NSEventMaskKeyUp | NSEventMaskFlagsChanged)
		handler:^NSEvent*(NSEvent* event)
		{
			// キーダウン・キーアップイベントのみ処理
			if (event.type != NSEventTypeKeyDown && event.type != NSEventTypeKeyUp)
			{
				return event;
			}

			unsigned short keyCode = [event keyCode];

			// 英数(0x66)またはかな(0x68)キーの場合
			if (keyCode == kVK_JIS_Eisu || keyCode == kVK_JIS_Kana)
			{
				// テキスト編集中の場合はイベントを通す(IME切り替えを許可)
				if (s_isEditingText)
				{
					return event;
				}

				// テキスト編集中でない場合はイベントを破棄(IME切り替えを防ぐ)
				return nil;
			}

			return event;
		}];
}

void KSMPlatformMacOS_StopBlockingIMEKeys()
{
	// イベントモニターを解除
	if (s_eventMonitor != nil)
	{
		[NSEvent removeMonitor:s_eventMonitor];
		s_eventMonitor = nil;
	}
}

void KSMPlatformMacOS_SetIsEditingText(bool isEditing)
{
	s_isEditingText = isEditing;
}
