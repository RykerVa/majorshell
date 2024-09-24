#include "history.h"

int shiftHistory()
{
  for (int i = 0; i < (HISTORY_SIZE - 1); i++)
    history[i] = history[i + 1];
  
  history[HISTORY_SIZE - 1] = NULL;

  return 1;
}

int addToHistory(char * input)
{
  char *cp;

  if (history[HISTORY_SIZE - 1] != NULL)
    shiftHistory();

  for (int i = 0; i < HISTORY_SIZE; i++)
  {
    if (history[i] != NULL)
      continue;
    
    cp = malloc(MAX_INPUT_SIZE*sizeof(char));
    for (int j = 0; j < MAX_INPUT_SIZE; j++)
      *(cp + j) = *(input + j);
    
    history[i] = cp;
    
    return 1;
  }
  
  return 0;
}

int clearHistory()
{
  printf("Clearing history");
  for (int i = 0; i < HISTORY_SIZE; i++)
  {
    if (history[i] == NULL)
      break;
    free(history[i]);
    history[i] = NULL;
    history_value++;
  }
  history_value--;

  return 1;
}

void printHistory()
{
  int in = history_value;
  for (int i = 0; i < HISTORY_SIZE; i++)
  {
    if (history[i] == NULL)
      continue;
    printf("[%i]: ", in++);
    for (int j = 0; j < MAX_INPUT_SIZE && history[i][j] != '\n'; j++)  
      printf("%c", history[i][j]);
    printf("\n");
  }
}

int getCommand(int cmd_num, char ** cmd_to_execute)
{
  if (history[cmd_num] == NULL)
  {
    printf("History is null");
    return 0;
  }

  *cmd_to_execute = history[cmd_num];

  printf("historycmd: %s", history[cmd_num]);
  printf("cmdte: %s", *cmd_to_execute);

  return 1;
}
