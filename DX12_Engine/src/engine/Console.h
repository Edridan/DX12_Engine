// basic console manager
// can push command into the engine such as : spawn actor or draw debug stuff

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <map>


#define CONSOLE_OUTPUT_BUFFER_SIZE	2048

// class that define the console
class Console
{
public:
	// command line that manage the call of the function and parameters
	class CommandLine
	{
	public:
		// paramters of the commandline
		std::map <std::string, std::string>		m_Parameters;

		// informations
		const std::string &	GetFunctionName() const;

		// friend class
		friend class Console;
	private:
		std::string m_FuncName;

		CommandLine(const std::string & i_Line);
		~CommandLine();
	};


	// base class for registering command line
	class Function
	{
	public:
		Function(const std::string & i_Name, const std::string & i_Help, const std::string & i_Sum);
		~Function();

		// info
		const std::string & GetName() const;
		const std::string & GetHelp() const;
		const std::string & GetSummary() const;

		friend class Console;
	private:
		// virtual pure to override to create command
		virtual bool		Execute(const CommandLine & i_CommandLine) = 0;

		// function helper
		const std::string	m_Name;
		const std::string	m_Helpers;	// param def
		const std::string	m_Summary;	// summary of the function
		// other
		const Console *		m_Console;	// auto filled when added to a console
	};

	Console();
	~Console();

	// console management
	bool		RegisterFunction(Console::Function * i_Command);
	bool		FunctionExist(const std::string & i_FuncName);

	// push command
	bool		PushCommand(const std::string & i_Command);

	// output/input management
	typedef std::function<void(const char*, void*)> OutputFunc;
	void	RegisterPrintCallback(const OutputFunc & i_Callback, void * i_Data = nullptr);
	void	Print(const char * i_Text, ...);	// this is used for print debug

private:
	// list of available commands
	std::map<const std::string, Console::Function *>	m_Functions;
	
	// this allow to output on UI or other
	std::vector<std::pair<OutputFunc, void*>>		m_PrintCallback;
};


// create default command here
// start by CF for CommandFunction class
class CFClear : public Console::Function
{
public:
	CFClear();
private:
	// virtual pure to override to create command
	virtual bool		Execute(const Console::CommandLine & i_CommandLine);
};