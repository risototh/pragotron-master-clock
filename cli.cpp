#include "cli.h"

CLI::CLI() {
}

void CLI::setCommands(struct cliCommandItem *commands, int length) {
  commandsCount = length;
  //CLI_CONSOLE.println(commands[0].command);
  
  commandsList = commands;
}

void CLI::printCommandList(char *prepend) {
  CLI_CONSOLE.print(prepend);
  CLI_CONSOLE.println("HELP");
  for (int i = 0; i < commandsCount; i++) {
    CLI_CONSOLE.print(prepend);
    CLI_CONSOLE.println(commandsList[i].command);
  }
}

void CLI::handle() {
  //CLI_CONSOLE.print("> ");
  
  if (!readLine()) {
    return;
  }
  
  if (!errorFlag) {
    parseLine();
  }
  
  if (!errorFlag){
    executeCommand();
  }
 
  memset(line, 0, CLI_LINE_BUF_SIZE);
  memset(args, 0, sizeof(args[0][0]) * CLI_MAX_NUM_ARGS * CLI_ARG_BUF_SIZE);
 
  errorFlag = false;
}

bool CLI::readLine() {
  String lineString;
 
  if (!CLI_CONSOLE.available()) {
    return false;
  }
 
  lineString = CLI_CONSOLE.readStringUntil('\n');
  lineString.trim();
  if (lineString.length() < CLI_LINE_BUF_SIZE) {
    lineString.toCharArray(line, CLI_LINE_BUF_SIZE);
    CLI_CONSOLE.println(lineString);
    return true;
  }
  
  CLI_CONSOLE.println("Input string too long.");
  errorFlag = true;
  return false;
}

void CLI::parseLine() {
  char *argument;
  int counter = 0;

  argument = strtok(line, " ");

  while ((argument != NULL)) {
    if (counter < CLI_MAX_NUM_ARGS) {
      if (strlen(argument) < CLI_ARG_BUF_SIZE) {
        strcpy(args[counter], argument);
        argument = strtok(NULL, " ");
        counter++;
      }
      else {
        CLI_CONSOLE.println("Input string too long.");
        errorFlag = true;
        break;
      }
    }
    else{
      break;
    }
  }
}
 
void CLI::executeCommand() {  
  /*
  String request, command;
  request = String(args[0]);
  //request.trim();
  command = String("HELP");
  
  CLI_CONSOLE.print("DEBUG = ");
  for (int i = 0; i < 10; i++) {
    CLI_CONSOLE.print(request.charAt(i), HEX);
    CLI_CONSOLE.print(" ");
  }
//  CLI_CONSOLE.print(request);
  CLI_CONSOLE.print("|\n");
  CLI_CONSOLE.println(request.equalsIgnoreCase(command) ? "eq" : "neq");
  */
  if (strcmp(args[0], "HELP") == 0) {
    executeHelp();
    return;
  }
  
  for (int i = 0; i < commandsCount; i++) {
    if (strcmp(args[0], commandsList[i].command) == 0) {
      return (*commandsList[i].callback)(args);
    }
  }

  CLI_CONSOLE.println("Invalid command. Type HELP for more.");
}

void CLI::executeHelp() {
  if (args[1] == NULL) {
    printHelp();
    return;
  }
    
  for (int i = 0; i < commandsCount; i++) {
    if (strcmp(args[1], commandsList[i].command) == 0) {
      if (commandsList[i].help != NULL) {
          (*commandsList[i].help)();
          return;
      }
      CLI_CONSOLE.println("No help for this command.");
      return;
    }
  }
  
  printHelp();
}

void CLI::printHelp() {
  char *helpExample;
  
  CLI_CONSOLE.print("The following commands are available:\n  HELP\n");
    
  for (int i = 0; i < commandsCount; i++) {
    CLI_CONSOLE.print("  ");
    CLI_CONSOLE.println(commandsList[i].command);
    if (commandsList[i].help != NULL) {
      helpExample = commandsList[i].command;
    }
  }
  CLI_CONSOLE.print("\nYou can for instance type \"HELP ");
  CLI_CONSOLE.print(helpExample);
  CLI_CONSOLE.print("\" for more info about the ");
  CLI_CONSOLE.print(helpExample);
  CLI_CONSOLE.print(" command.");
}
