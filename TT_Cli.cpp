#include "TT_Cli.h" 
#include <iostream>
#include <sstream>

#define CREATE_USAGE_INFO(CMD_NAME, CMD_ARGS) "usage: " CMD_NAME " " CMD_ARGS "\n"
#define CONCAT_CMD_DEF(CMD_NAME, CMD_DEF) "type " CMD_NAME " to " CMD_DEF
#define COMPLETE_DEF(LINE1, LINE2) LINE1 LINE2

#define CREATE_CMD_STR(NAME) c_CMD_ ## NAME ## _STR
#define CREATE_CMD_DEF(CMD_DEF) c_CMD_ ## CMD_DEF ## _DEF
#define CREATE_CLI_CMD(NAME, VALUE, ARGS, ABOUT) \
	const std::string CREATE_CMD_STR(NAME) = VALUE; \
	const std::string CREATE_CMD_DEF(NAME) = COMPLETE_DEF(CREATE_USAGE_INFO(VALUE, ARGS) , CONCAT_CMD_DEF(VALUE, ABOUT));

CREATE_CLI_CMD(HELP, "help", " ", "get information about commands");
CREATE_CLI_CMD(CHOOSE_PROMPT, "choose_prompt", "<PROMPT>", "change actual prompt in program");

static const std::string sc_VERSION = std::to_string(VERSION_CLI_MAJOR) + "." +
									  std::to_string(VERSION_CLI_MINOR) + "." +
									  std::to_string(VERSION_CLI_PATCH);

bool UserHelper(std::vector<std::string>);
bool PromptChanger(std::vector<std::string>);
bool GetCLIVersion(std::vector<std::string>);

CMDListArr CMDListCLI;

CommandDefinition::CommandDefinition()
:cmdName("")
,cmdDefinition("")
,cmdFunc(nullptr)
{
	
}

CommandDefinition::CommandDefinition(const std::string& name, const std::string& definition, CmdFuncPtr funcPtr)
:cmdName(name)
,cmdDefinition(definition)
,cmdFunc(funcPtr)
{
	
}

bool CommandDefinition::operator<(const CommandDefinition& CdObj) const
{
	return this->cmdName < CdObj.cmdName;
}

bool CommandDefinition::operator==(const CommandDefinition& CdObj) const
{
	return this->cmdName == CdObj.cmdName;
}

TT_Cli* GetCLI()
{
    static TT_Cli s_CLI;
    return &s_CLI;
} 

TT_Cli::TT_Cli()
:prompt("$ ")
,m_CMDListPtr(nullptr)
{
	CMDListCLI.insert(CommandDefinition(c_CMD_HELP_STR, c_CMD_HELP_DEF, &UserHelper));
	CMDListCLI.insert(CommandDefinition(c_CMD_CHOOSE_PROMPT_STR, c_CMD_CHOOSE_PROMPT_DEF, &PromptChanger));
	
	std::cout << "################################################################################" << std::endl;
    std::cout << "#                             CLI Version " << sc_VERSION <<"                                #" << std::endl;
    std::cout << "################################################################################" << std::endl;
}

std::vector<std::string> TT_Cli::ParseString(const std::string& str)
{
	std::vector<std::string> parsedString;
	std::stringstream ss(str);
	std::string s;

	while (std::getline(ss, s, ' ')) {
		parsedString.push_back(s);
	}
	return parsedString;
}

void TT_Cli::RegisterCMDList(CMDListArr& CMDList)
{
	m_CMDListPtr = &CMDList;
}

bool GetCLIVersion(std::vector<std::string> cmdArgs)
{
	return GetCLI()->_GetCLIVersion(cmdArgs);
}

bool TT_Cli::_GetCLIVersion(std::vector<std::string> cmdArgs)
{
    std::cout << " CLI Version " << sc_VERSION << std::endl;
    return true;
}

void TT_Cli::SetCommand(const std::string& str)
{
	m_currentCommand = str;
	return;
}

bool TT_Cli::_UserHelper(std::vector<std::string> cmd_args)
{
    std::cout << std::endl << "Client Application version 0.3 supported commands are" << std::endl;
	for (auto it = (*m_CMDListPtr).begin(); it != (*m_CMDListPtr).end(); ++it) {
        std::cout << it->cmdName << " | ";
        std::cout << it->cmdDefinition << std::endl;
    }
    std::cout << std::endl << "CLI version 0.3 supported commands are" << std::endl;
    for (auto it = CMDListCLI.begin(); it != CMDListCLI.end(); ++it) {
        std::cout << it->cmdName << " | ";
        std::cout << it->cmdDefinition << std::endl;
    }
    return true;
}

bool TT_Cli::_PromptChanger(std::vector<std::string> cmdArgs)
{
    bool retVal = true;
	if (cmdArgs.size() == 1) {
        std::cout << "You need to specify prompt as choose_prompt <prompt>" << std::endl;
        return retVal;
    }
  
	prompt = cmdArgs[1] + " ";
    return retVal;
}

bool TT_Cli::GetLine()
{
    std::cout << prompt;
    std::getline(std::cin, m_currentCommand);
    return CommandHandler();
}

bool TT_Cli::CommandHandler()
{
	bool retVal = true;
  	m_CommandVec = ParseString(m_currentCommand);
    if (m_CommandVec.empty()) {
		return retVal;
    }
    if (m_CommandVec[0] == "clear") {
        system("clear");
        return retVal;
    }
    
    auto it1 = (*m_CMDListPtr).find(CommandDefinition(m_CommandVec[0], "", nullptr));
    
	if (it1 != (*m_CMDListPtr).end()) {
		return it1->cmdFunc(m_CommandVec);
    }

	auto it2 = CMDListCLI.find(CommandDefinition(m_CommandVec[0] , "" , nullptr));
    if (it2 != CMDListCLI.end()) {
        return it2->cmdFunc(m_CommandVec);
    }

    std::cout << " Invalid Command" << std::endl;
    return retVal;
}

bool UserHelper(std::vector<std::string> cmdArgs)
{
    return GetCLI()->_UserHelper(cmdArgs);
}

bool PromptChanger(std::vector<std::string> cmdArgs)
{
	return GetCLI()->_PromptChanger(cmdArgs);
}

