#include "pch.h"
#include "vanillasupport.h"
#include "gameutils.h"
#include "squirrel.h"
#include "dedicated.h"

bool g_usingVanillaSupport = false;

// bool function NSIsSupportingVanilla()
SQRESULT SQ_IsSupportingVanilla(void* sqvm)
{
	ClientSq_pushbool(sqvm, g_usingVanillaSupport);
	return SQRESULT_NOTNULL;
}

void InitialiseVanillaSupport(HMODULE baseAddress)
{
	if (CommandLine()->CheckParm("-vanillasupport"))
	{
		spdlog::info("Vanilla support enabled");
		g_usingVanillaSupport = true;
	}
}

void InitialiseVanillaSupportSquirrel(HMODULE baseAddress)
{
	if (!IsDedicated())
	{

		g_ClientSquirrelManager->AddFuncRegistration("bool", "NSIsSupportingVanilla", "", "Reloads mods", SQ_IsSupportingVanilla);
		g_UISquirrelManager->AddFuncRegistration("bool", "NSIsSupportingVanilla", "", "Reloads mods", SQ_IsSupportingVanilla);
	}
	else
	{
		// should we add server vanilla support somehow in future? maybe using dns-based stryder interception?
		// this won't be needed until / if that happens
		// g_ServerSquirrelManager->AddFuncRegistration("bool", "NSIsSupportingVanilla", "", "", SQ_IsSupportingVanilla);
	}
}
