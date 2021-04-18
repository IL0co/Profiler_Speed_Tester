
#include <profiler>
#include <sourcemod>

/*
	Автор: iLoco
	Версия: 1.0.0
	
	https://hlmod.ru/members/iloco.94537/
*/

/*
	Пример использования:

	
	char buff[256];

	public void OnPluginStart()
	{
		Prof_Start(1000);
		Prof_Test("_Format");
		Prof_Test("_IntToString");
		Prof_Results();
	}

	public void _Format()
	{
		Format(buff, sizeof(buff), "657");
	}

	public void _IntToString()
	{
		IntToString(657, buff, sizeof(buff));
	}
*/

enum struct ProfData
{
	int Repeats;
	bool IsStarted;

	ArrayList FunctionsNames;
	ArrayList TestsTotalResult;

	void Reset()
	{
		this.IsStarted = false;
		delete this.FunctionsNames;
		delete this.TestsTotalResult;
	}

	void Init()
	{
		this.FunctionsNames = new ArrayList(256);
		this.TestsTotalResult = new ArrayList();
	}
}

ProfData g_ProfData;

/*
 * Начать проверку
 *	-
 * @param repeats			Количество повторов проверки одной функции
 *	-
 * @noreturn 
*/
stock void Prof_Start(int repeats = 100)
{
	if(g_ProfData.IsStarted)
		ThrowError("Проверка уже начата!");

	g_ProfData.Repeats = repeats;
	g_ProfData.IsStarted = true;
	g_ProfData.Init();
}

/*
 * Добавить функцию к проверке и сравнению
 *	-
 * @param func_name			Название функции
 *	-
 * @noreturn 
*/
stock void Prof_Test(char[] func_name)
{
	if(!g_ProfData.IsStarted)
		ThrowError("Проверка ещё не начата!");

	Function func = GetFunctionByName(INVALID_HANDLE, func_name);

	if(func == INVALID_FUNCTION)
		ThrowError("Не валидная функция! '%s'", func_name);

	g_ProfData.FunctionsNames.PushString(func_name);

	Profiler prof = new Profiler();

	prof.Start()

	for(int num; num < g_ProfData.Repeats; num++)
	{
		Call_StartFunction(INVALID_HANDLE, func);
		Call_Finish();
	}

	prof.Stop()

	g_ProfData.TestsTotalResult.Push(prof.Time);

	delete prof;
}

/*
 * Закончить проверку и вывести результаты в консоль сервера
 *	-
 * @noparams
 *	-
 * @noreturn 
*/
stock void Prof_Results()
{
	if(!g_ProfData.IsStarted)
		ThrowError("Проверка ещё не начата!");

	char buffer[23];
	float best_time = 1.0, lost_time, time;
	int best_place, lost_place;

	for(int id; id < g_ProfData.FunctionsNames.Length; id++)
	{
		time = g_ProfData.TestsTotalResult.Get(id);

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
	PrintToServer("Результат с %i тестов:", g_ProfData.Repeats);
	PrintToServer("        Функция        |   Всего   |   Среднее   | Скорость | Продуктивность");

	for(int id; id < g_ProfData.FunctionsNames.Length; id++)
	{
		g_ProfData.FunctionsNames.GetString(id, buffer, sizeof(buffer));

		if(buffer[0] == '_')
			Format(buffer, sizeof(buffer), buffer[1]);

		time = g_ProfData.TestsTotalResult.Get(id);
		PrintToServer("%23s| %.7f | %.9f |  x%.3f  | %.1f%% %s", buffer, time, (time / g_ProfData.Repeats), (lost_time / time), (lost_time / time * 100.0),
		id == best_place ? "<<== Быстрее всех" : (id == lost_place ? "<<== Самое медленное" : ""));
	}

	g_ProfData.Reset();

	PrintToServer(" ");
}