#include <glm/glm.hpp>

#include "../Logger/Log.hpp"

#include "Input.hpp"

using namespace VoxSmith;

Keyboard* Keyboard::s_keyboard = nullptr;
Mouse* Mouse::s_mouse = nullptr;

Keyboard::Keyboard()
{
	for (uint32_t iKey = 0; iKey < 1024; iKey++)
	{
		m_keys[iKey] = KeyStatus::RELEASE;
	}
}

Keyboard::~Keyboard()
{
	delete s_keyboard;
}

Keyboard& Keyboard::getInstance()
{
	if (s_keyboard == nullptr)
	{
		s_keyboard = new Keyboard();
	}

	return *s_keyboard;
}

void Keyboard::setKeyStatus(const uint32_t key, KeyStatus status)
{
	if (key >= 1024)
	{
		LOG_CORE_ERROR("Keyboard::set_status::error");
		return;
	}

	m_keys[key] = status;
}

KeyStatus Keyboard::getKeyStatus(const uint32_t key) const
{
	if (key >= 1024)
	{
		LOG_CORE_ERROR("Keyboard::get_status::error");
		return KeyStatus::RELEASE;
	}

	return m_keys[key];
}

bool Keyboard::isKeyActive(const uint32_t key) const
{
	if (key >= 1024)
	{
		LOG_CORE_ERROR("Keyboard::is_active::error");
		return false;
	}

	return (m_keys[key] & (KeyStatus::REPEAT | KeyStatus::PRESS)) != 0;
}

Mouse::Mouse()
	: m_moved(false)
{

}

Mouse::~Mouse()
{
	delete s_mouse;
}

Mouse& Mouse::getInstance()
{
	if (s_mouse == nullptr)
	{
		s_mouse = new Mouse();
	}

	return *s_mouse;
}

void Mouse::setMousePos(const float xPos, const float yPos)
{
	m_xPos = xPos;
	m_yPos = yPos;

	m_moved = true;
}

glm::vec2 Mouse::getMousePos() const
{
	return { m_xPos, m_yPos };
}
