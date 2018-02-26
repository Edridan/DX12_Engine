#include "Console.h"

// vsnprintf, sprintf_s
#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>

#include "engine/Debug.h"
#include "engine/Engine.h"
#include "engine/World.h"
#include "ui/UILayer.h"
#include "ui/UIConsole.h"

Console::Console()
{
}

Console::~Console()
{
	// delete functions
	auto itr = m_Functions.begin();

	while (itr != m_Functions.end())
	{
		delete (itr->second);
		++itr;
	}
}

bool Console::RegisterFunction(Console::Function * i_Command)
{
	if (m_Functions[i_Command->m_Name] != nullptr)
		return false;

	// add the function to the command console
	m_Functions[i_Command->m_Name] = i_Command;
	i_Command->m_Console = this;

	return true;
}

bool Console::FunctionExist(const std::string & i_FuncName)
{
	if(m_Functions[i_FuncName] == nullptr)
		return false;

	return false;
}

void Console::GetHelp(std::vector<std::string> & o_Help) const
{
	auto itr = m_Functions.begin();

	while (itr != m_Functions.end())
	{
		o_Help.push_back(itr->second->m_Name + " " + itr->second->GetHelp());
		++itr;
	}
}

bool Console::PushCommand(const std::string & i_Command)
{
	// print the command
	CommandLine line(i_Command.c_str());

	Print(i_Command.c_str());

	if (m_Functions[line.GetFunctionName()] == nullptr)
	{
		// print error
		Print("[Error] Function %s does not exist", line.GetFunctionName().c_str());
		return false;
	}

	// e
	Function * const func = m_Functions[line.GetFunctionName()];
	bool ret = func->Execute(line);

	if (!ret)
	{
		// print error
		Print("[Error] Function %s error when called", line.GetFunctionName().c_str());
		Print("usage : %s %s", func->GetName(), func->GetHelp().c_str());
	}

	return ret;
}

void Console::RegisterPrintCallback(const OutputFunc & i_Callback, void * i_Data /* = nullptr */ )
{
	// push back the function
	m_PrintCallback.push_back(
		std::make_pair(i_Callback, i_Data)
	);
}

void Console::Print(const char * i_Text, ...)
{
	static char buffer[CONSOLE_OUTPUT_BUFFER_SIZE];
	const char * p = buffer;

	va_list args;
	va_start(args, i_Text);
	vsnprintf(buffer, CONSOLE_OUTPUT_BUFFER_SIZE, i_Text, args);
	va_end(args);

	// safe impl
	buffer[CONSOLE_OUTPUT_BUFFER_SIZE - 1] = '\0';	// be sure the string is null terminated

	auto itr = m_PrintCallback.begin();

	while (itr != m_PrintCallback.end())
	{
		// call the function with the paramter
		((*itr).first)(buffer, (*itr).second);
		++itr;
	}
}


// function decl

Console::Function::Function(const std::string & i_Name, const std::string & i_Help, const std::string & i_Sum)
	:m_Name(i_Name)
	,m_Helpers(i_Help)
	,m_Summary(i_Sum)
{
}

Console::Function::~Function()
{
}

const std::string & Console::Function::GetName() const
{
	return m_Name;
}

const std::string & Console::Function::GetHelp() const
{
	return m_Helpers;
}

const std::string & Console::Function::GetSummary() const
{
	return m_Summary;
}

Console * Console::Function::GetConsole() const
{
	return m_Console;
}

// command line decl

const std::string & Console::CommandLine::GetFunctionName() const
{
	return m_FuncName;
}

bool Console::CommandLine::IsNumber(const Value & i_Value) const
{
	if (i_Value.m_Type == Console::CommandLine::EType::eFloat
		|| i_Value.m_Type == Console::CommandLine::EType::eInt)
		return true;

	return false;
}

int Console::CommandLine::ToInt(const Value & i_Val) const
{
	if (i_Val.m_Type == eFloat)
		return (int)atof(i_Val.m_Value.c_str());
	else if (i_Val.m_Type == eInt)
		return atoi(i_Val.m_Value.c_str());

	return 0;
}

float Console::CommandLine::ToFloat(const Value & i_Val) const
{
	if (i_Val.m_Type == eFloat)
		return (float)atof(i_Val.m_Value.c_str());
	else if (i_Val.m_Type == eInt)
		return (float)atoi(i_Val.m_Value.c_str());

	return 0.0f;
}

std::string Console::CommandLine::ToString(const Value & i_Val) const
{
	if (i_Val.m_Type == eString)
		return i_Val.m_Value;

	return "Error";
}

Console::CommandLine::EType Console::CommandLine::ParseType(const std::string & i_Val) const
{
	bool isNumber = false;
	bool havePoint = false;
	bool error = false;

	// first char
	if (i_Val[0] >= '0' && i_Val[0] <= '9' || i_Val[0] == '.')
	{
		// seems to be a number
		isNumber = true;
	}

	for (size_t i = 0; i < i_Val.size(); ++i)
	{
		if (isNumber)
		{
			if (i_Val[i] == '.')
			{
				// already have a point : error
				if (havePoint)		return eNone;

				havePoint = true;
			}
			else if (i_Val[i] < '0' || i_Val[i] > '9')
			{
				// supposed to be a number but have other character
				return eNone;
			}
		}
		else
		{
			// To do : manage string
		}
	}

	if (isNumber && havePoint)
		return eFloat;
	else if (isNumber)
		return eInt;
	
	return eString;
}

Console::CommandLine::CommandLine(const std::string & i_Line)
{
	// create line with parameters
	std::string buffer = i_Line;
	size_t i = buffer.find_first_of(' ');

	// retreive function name
	if (i == std::string::npos)	m_FuncName = i_Line;
	else						m_FuncName = i_Line.substr(0, i);
	

	// get params
	while (true)
	{
		// erase last param
		buffer.erase(0, i + 1);

		// go to the next param
		i = buffer.find_first_of(' ');

		if (i == std::string::npos)	break;

		std::string param = buffer.substr(0, i);
		
		Value val;
		val.m_Type = ParseType(param);
		val.m_Value = param;

		m_Parameters.push_back(val);
	}

	if (buffer.size() != 0)
	{
		std::string param = buffer;

		Value val;
		val.m_Type = ParseType(param);
		val.m_Value = param;

		m_Parameters.push_back(val);
	}
}

Console::CommandLine::~CommandLine()
{
}


//////////////////////////////////////////////////

CFClear::CFClear()
	:Console::Function("clear", "", "clear console output")
{
}

// basic function specific

bool CFClear::Execute(const Console::CommandLine & i_CommandLine)
{
	UILayer * layer = Engine::GetInstance().GetUILayer();
	UIConsole *	ui = dynamic_cast<UIConsole*>(layer->FindUIWindowByName("Console"));

	if (ui)
	{
		ui->Clear();
	}

	return true;
}

CFActorCount::CFActorCount()
	:Console::Function("world_count", "", "count the number of actors currently instancied in the world")
{
}

bool CFActorCount::Execute(const Console::CommandLine & i_CommandLine)
{
	GetConsole()->Print("%i actors in the current world", Engine::GetInstance().GetWorld()->GetActorCount());
	return true;
}

CFHelp::CFHelp()
	:Console::Function("help", "", "print help for console")
{
}

bool CFHelp::Execute(const Console::CommandLine & i_CommandLine)
{
	std::vector<std::string> help;
	GetConsole()->GetHelp(help);

	auto itr = help.begin();

	while (itr != help.end())
	{
		GetConsole()->Print((*itr).c_str());
		++itr;
	}

	return true;
}

CFSetFrameTarget::CFSetFrameTarget()
	:Console::Function("set_fps", "[int]", "set the target frame per second managed by the engine")
{
}

bool CFSetFrameTarget::Execute(const Console::CommandLine & i_CommandLine)
{
	if (i_CommandLine.m_Parameters.size() != 1)
		return false;

	if (!i_CommandLine.IsNumber(i_CommandLine.m_Parameters[0]))
		return false;

	Engine::GetInstance().SetFramePerSecondTarget((UINT)i_CommandLine.ToInt(i_CommandLine.m_Parameters[0]));
	return true;
}

CFPrintParam::CFPrintParam()
	:Console::Function("print", "[...]", "print all parameters passed in arguments with type")
{
}

bool CFPrintParam::Execute(const Console::CommandLine & i_CommandLine)
{
	GetConsole()->Print("The console functions available :");

	for (size_t i = 0; i < i_CommandLine.m_Parameters.size(); ++i)
	{
		std::string type;

		switch (i_CommandLine.m_Parameters[i].m_Type)
		{
		case Console::CommandLine::eInt:	type = "int"; break;
		case Console::CommandLine::eFloat:	type = "float"; break;
		case Console::CommandLine::eString: type = "string"; break;
		case Console::CommandLine::eNone:   type = "none"; break;

		default :   						type = "error"; break;	
		}

		GetConsole()->Print("[%i] : %s (%s)", i, i_CommandLine.m_Parameters[i].m_Value, type.c_str());
	}

	return false;
}
