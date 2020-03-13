#pragma once

#include <memory>

#include <GLFW/glfw3.h>

class Stage;

class Phase
{
public:
	// Every Phase must have a constructor with no arguments!

	virtual void enable(Stage& stage) {};

	virtual void disable(Stage& stage) {};

	virtual void update(Stage& stage, float delta) {};

	virtual void render(Stage& stage) {};

	template<typename P>
	static std::shared_ptr<P> create()
	{
		auto result = std::make_shared<P>();
		return result;
	}
};


class Stage
{
private:
	std::shared_ptr<Phase> phase;

public:
	GLFWwindow* window;

	Stage(GLFWwindow* window);

	std::shared_ptr<Phase> get_phase() const;
	void set_phase(std::shared_ptr<Phase> phase);

	void update(float delta);
	void render();
};
