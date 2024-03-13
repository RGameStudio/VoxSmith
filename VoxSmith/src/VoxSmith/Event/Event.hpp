#pragma once

#include <stdint.h>

namespace VoxSmith
{
	enum class WindowEventType : uint8_t
	{
		Keyboard,
		MouseMove,
		MouseClick,
		Close
	};

	struct WindowEvent
	{
		virtual inline WindowEventType getType() const = 0;
	};

	struct KeyboardEvent : public WindowEvent
	{
		int32_t m_key;
		int32_t m_action;

		inline WindowEventType getType() const override { return WindowEventType::Keyboard; }
	};

	struct MouseMoveEvent : public WindowEvent
	{
		int32_t x;
		int32_t y;

		inline WindowEventType getType() const override { return WindowEventType::MouseMove; }
	};

	struct MouseClickEvent : public WindowEvent
	{
		int32_t m_action;
		int32_t m_button;

		inline WindowEventType getType() const override { return WindowEventType::MouseClick; }
	};

	struct CloseEvent : public WindowEvent
	{
		inline WindowEventType getType() const override { return WindowEventType::Close; }
	};
}
