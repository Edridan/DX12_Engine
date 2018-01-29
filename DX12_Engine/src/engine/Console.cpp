#include "Console.h"

// vsnprintf, sprintf_s
#include <cstdarg>
#include <stdio.h>

#include "engine/Debug.h"
#include "engine/Engine.h"
#include "ui/UILayer.h"
#include "ui/UIConsole.h"

Console::Console()
{
}

Console::~Console()
{
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
	bool ret = m_Functions[line.GetFunctionName()]->Execute(line);

	if (!ret)
	{
		// print error
		Print("[Error] Function %s error when called", line.GetFunctionName().c_str());
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

// command line decl

const std::string & Console::CommandLine::GetFunctionName() const
{
	return m_FuncName;
}

Console::CommandLine::CommandLine(const std::string & i_Line)
{
	// create line with parameters
	std::string buffer = i_Line;
	size_t i = i_Line.find_first_of(' ');

	// retreive function name
	if (i == std::string::npos)	m_FuncName = i_Line;
	else						m_FuncName = i_Line.substr(0, i);
	
	// erase all characters
	buffer.erase(0, i + 1);

	// get params
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
