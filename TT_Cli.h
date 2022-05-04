#pragma once
#include <string>
#include <set>
#include <vector>

//function pointer for any commands what we will use 
typedef bool (*CmdFuncPtr)(std::vector<std::string>);

//struct which contains all needed information about command

struct CommandDefinition {
	CommandDefinition();
	CommandDefinition(const std::string&, const std::string&, CmdFuncPtr);
	bool operator<(const CommandDefinition&) const;
	bool operator==(const CommandDefinition&) const;
	std::string cmdName;
	std::string cmdDefinition;
	CmdFuncPtr cmdFunc;
};

//using set to find command in log(n) complexity , also it provides much more functionality
typedef std::set<CommandDefinition> CMDListArr;

class TT_Cli {
public:
	TT_Cli();

	//functions below describe CLI functionality
	
	void RegisterCMDList(CMDListArr&);
	void SetCommand(const std::string&);
    bool GetLine();
	bool CommandHandler();
	bool _GetCLIVersion(std::vector<std::string>);
    bool _UserHelper(std::vector<std::string>);
    bool _PromptChanger(std::vector<std::string>);
private:
	std::vector<std::string> ParseString(const std::string&);
	std::string m_currentCommand;
	std::vector<std::string> m_CommandVec;
    CMDListArr* m_CMDListPtr;
    std::string prompt;
};

// this function will return static object of CLI 
TT_Cli* GetCLI();

