#pragma once

#include "Module.h"

class ModuleProgram : public Module
{
public:
	bool Start() override;
	bool CleanUp() override;

public:
	unsigned program = 0;
};
