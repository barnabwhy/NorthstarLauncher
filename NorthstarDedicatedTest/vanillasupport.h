#pragma once
#include "pch.h"

extern bool g_usingVanillaSupport;

void InitialiseVanillaSupport(HMODULE baseAddress);
void InitialiseVanillaSupportSquirrel(HMODULE baseAddress);
