#pragma once

struct GLTest2
{
	GLTest2()
	{
		// todo
	}

	inline void Update(int const& width, int const& height, float time) noexcept
	{
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		// todo
	}
};
