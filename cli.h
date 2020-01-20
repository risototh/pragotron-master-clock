#ifndef _CLI_H_
  #define _CLI_H_

  #include <Arduino.h>

  #ifndef CLI_CONSOLE
    #define CLI_CONSOLE Serial
  #endif 

  #ifndef CLI_LINE_BUF_SIZE
    #define CLI_LINE_BUF_SIZE 300 // maximum length of one command line (FQDN argument can be 255 bytes long! + command)
  #endif

  #ifndef CLI_ARG_BUF_SIZE  
    #define CLI_ARG_BUF_SIZE  256 // maximum argument string length
  #endif

  #ifndef CLI_MAX_NUM_ARGS
    #define CLI_MAX_NUM_ARGS  4   // maximum number of arguments
  #endif

  #ifndef CLI_COMMAND_SIZE
    #define CLI_COMMAND_SIZE  8   // maximum length of command
  #endif

  struct cliCommandItem {
    char command[CLI_COMMAND_SIZE];
    void (*callback)(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
    void (*help)();
  };

  typedef cliCommandItem cliCommandItems[];

  class CLI {
    public:
      CLI();

      void setCommands(struct cliCommandItem *commands, int length);
      void printCommandList(char *prepend);
      void handle();

    private:
      int commandsCount;
      struct cliCommandItem *commandsList;
      bool errorFlag = false;
      char line[CLI_LINE_BUF_SIZE];
      char args[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE];

      bool readLine();
      void parseLine();
      void executeCommand();
      void executeHelp();
      void printHelp();
  };
  
  
#endif
