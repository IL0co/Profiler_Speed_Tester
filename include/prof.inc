#if defined _prof_speed_test_included
 #endinput
#endif
#define _prof_speed_test_included

#include <profiler>
#include <sourcemod>

/*
	Author: iLoco
	Version: 1.0.1
	
	https://hlmod.ru/members/iloco.94537/
*/

/*
	Usage example:

	
	char buff[] = {"asdklastiajlkncnjansiejwij ijwoijadl knsjkld jawjd ioajsdl nasnd jaksnd kjasnhkjd najnd kjasn dkj"};

	public void OnPluginStart()
	{
		Prof_Start(10000);
		Prof_Test("_StrEqual");
		Prof_Test("_strcmp");
		Prof_Results();
	}

	public bool _StrEqual()
	{
		if(StrEqual(buff, "some text", false))
			return true;

		return false;
	}

	public bool _strcmp()
	{
		if(strcmp(buff, "some text", false) == 0)
			return true;

		return false;
	}
*/

bool g_bIsStarted;
int g_iRepeats;
ArrayList g_hFunctionsNames;
ArrayList g_hTestsTotalResult;

#define PLUGIN_TAG "[Profiling] "

/*
 * Start check
 *	-
 * @param repeats			The number of repetitions of checking one function
 *	-
 * @noreturn 
*/
stock void Prof_Start(int repeats = 100)
{
	if(g_bIsStarted)
		ThrowError("Check already started!");

	g_iRepeats = repeats;
	g_bIsStarted = true;
	g_hFunctionsNames = new ArrayList(256);
	g_hTestsTotalResult = new ArrayList();

	PrintToServer(" ");
	PrintToServer(PLUGIN_TAG ... "Started");
}

/*
 * Add function to check and compare
 *	-
 * @param func_name			Function name
 *	-
 * @noreturn 
*/
stock void Prof_Test(char[] func_name)
{
	if(!g_bIsStarted)
		ThrowError("Check not started yet!");

	Function func = GetFunctionByName(INVALID_HANDLE, func_name);

	if(func == INVALID_FUNCTION)
		ThrowError("Invalid function! '%s'", func_name);

	if(g_hFunctionsNames.FindString(func_name) != -1)
		ThrowError("Function already registered! '%s'", func_name);

	g_hFunctionsNames.PushString(func_name);
	
	PrintToServer(PLUGIN_TAG ... "Launching %s...", func_name);

	Profiler prof = new Profiler();

	prof.Start()

	for(int num; num < g_iRepeats; num++)
	{
		Call_StartFunction(INVALID_HANDLE, func);
		Call_Finish();
	}

	prof.Stop()

	g_hTestsTotalResult.Push(prof.Time);

	delete prof;
}

/*
 * Finish checking and output the results to the server console
 *	-
 * @noparams
 *	-
 * @noreturn 
*/
stock void Prof_Results()
{
	if(!g_bIsStarted)
		ThrowError("Check not started yet!");

	char buffer[23];
	float best_time = 1.0, lost_time, time;
	int best_place, lost_place;

	for(int id; id < g_hFunctionsNames.Length; id++)
	{
		time = g_hTestsTotalResult.Get(id);

		if(time < best_time)
		{
			best_time = time;
			best_place = id;
		}

		if(time > lost_time)
		{
			lost_time = time;
			lost_place = id;
		}
	}

	PrintToServer(" ");
	PrintToServer(PLUGIN_TAG ... "Result with %i tests:", g_iRepeats);
	PrintToServer(PLUGIN_TAG ... "     Function name     | Total, sec |   Avg, ms   |  Speed  | Performance");

	for(int id; id < g_hFunctionsNames.Length; id++)
	{
		g_hFunctionsNames.GetString(id, buffer, sizeof(buffer));

		if(buffer[0] == '_')
			Format(buffer, sizeof(buffer), buffer[1]);

		time = g_hTestsTotalResult.Get(id);
		PrintToServer(PLUGIN_TAG ... "%23s|  %.6f  | %.9f | x%.4f | %.1f%% %s", buffer, time, (time / g_iRepeats), (lost_time / time), (best_time/ time * 100.0),
		id == best_place ? "<<== Fastest" : (id == lost_place ? "<<== Slowest" : ""));
	}

	delete g_hFunctionsNames;
	delete g_hTestsTotalResult;
	g_bIsStarted = false;

	PrintToServer(" ");
}