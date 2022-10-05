#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

// Initialzing Functions
void func_BG(char **cmd);
void add_node(pid_t pid, char **cmd);
void func_BGlist(char **cmd);
void remove_terminated_nodes();
void func_BGkill(char **cmd);
void func_BGstop(char **cmd);
void func_BGstart(char **cmd);
void func_pstat(char *str_pid);
int str_to_int(char str[]);
void print_stats(char *str_pid);
void print_status(char *str_pid);

// declaring global variables
Node *head = NULL;

int main()
{
  char user_input_str[50];
  int keep_going = 0;
  while (keep_going < 80)
  {
    printf("Pman: > ");
    fgets(user_input_str, 50, stdin);
    printf("User input: %s \n", user_input_str);
    char *ptr = strtok(user_input_str, " \n");
    if (ptr == NULL)
    {
      continue;
    }
    char *lst[50];
    int index = 0;
    lst[index] = ptr;
    index++;
    while (ptr != NULL)
    {
      ptr = strtok(NULL, " \n");
      lst[index] = ptr;
      index++;
    }
    if (strcmp("bg", lst[0]) == 0)
    {
      if (lst[1] == NULL)
      {
        printf("Invalid input\n");
      }
      else
      {
        func_BG(lst);
      }
    }
    else if (strcmp("bglist", lst[0]) == 0)
    {
      func_BGlist(lst);
    }
    else if (strcmp("bgkill", lst[0]) == 0)
    {
      if (lst[1] == NULL)
      {
        printf("Please provide the pid\n");
      }
      else
      {
        func_BGkill(lst);
      }
    }
    else if (strcmp("bgstop", lst[0]) == 0)
    {
      if (lst[1] == NULL)
      {
        printf("Please provide the pid\n");
      }
      else
      {
        func_BGstop(lst);
      }
    }
    else if (strcmp("bgstart", lst[0]) == 0)
    {
      if (lst[1] == NULL)
      {
        printf("Please provide the pid\n");
      }
      else
      {
        func_BGstart(lst);
      }
    }
    else if (strcmp("pstat", lst[0]) == 0)
    {
      if (lst[1] == NULL)
      {
        printf("Please provide the pid\n");
      }
      else
      {
        func_pstat(lst[1]);
      }
    }
    else if (strcmp("q", lst[0]) == 0)
    {
      printf("Bye Bye \n");
      exit(0);
    }
    else
    {
      printf("Invalid input\n");
    }

    keep_going++;
  }

  return 0;
}

/*
  Function: func_BG
  Description: 
    Creates a new child process. 
    In the child process, it will execute the given command. If there is an errer, an error message will be printed. 
    In the parent process, it will check to see if the child process successfully executed. Calls new_node if so. 
  Parameters: 
    - cmd: command to be executed in the child process
*/
void func_BG(char **cmd)
{
  pid_t pid = fork();
  if (pid == 0)
  {
    char *args[10];
    int i = 1;
    while (cmd[i] != NULL)
    {
      args[i - 1] = cmd[i];
      i++;
    }
    args[i - 1] = NULL;

    execvp(cmd[1], args);
    if (execvp(cmd[1], args) < 0)
    {
      printf("Error: execution of %s failed\n", cmd[0]);
      kill(getpid(), SIGKILL);
    }
  }
  else
  {
    usleep(50000);
    int status;
    pid_t return_pid = waitpid(pid, &status, WNOHANG);
    if (return_pid == 0)
    {
      add_node(pid, cmd);
    }
  }
}

/*
  Function: add_node
  Description: 
    Calls add_newNode to add a node to the head of the linked list. 
  Parameters: 
    - pid: the pid
    - cmd: the path 
*/
void add_node(pid_t pid, char **cmd)
{
  head = add_newNode(head, pid, cmd[1]);
}

/*
  Function: func_BGlist
  Description: 
    Calls remove_terminated_nodes
    Loops through the linked list to print the running processes to the terminal. 
    Keeps count of the amount of running processes and prints the total to the terminal. 
  Parameters: 
    - cmd: 
*/
void func_BGlist(char **cmd)
{
  remove_terminated_nodes();
  int count = 0;
  Node *temp = head;
  while (temp != NULL)
  {
    printList(temp);
    count++;
    temp = temp->next;
  }
  printf("Total running processes: %d\n", count);
}

/*
  Function: remove_terminated_nodes
  Description: 
    Iterates through the linked list. 
    If the pid associated with each node has terminated, prints a message to the terminal and removes the node. 
  Parameters: None
*/
void remove_terminated_nodes()
{
  Node *temp = head;
  while (temp != NULL)
  {
    int status;
    pid_t return_pid = waitpid(temp->pid, &status, WNOHANG);
    if (return_pid == -1)
    {
      Node *holder = temp;
      temp = temp->next;
      printf("%s with pid %d has been terminated.\n", holder->path, holder->pid);
      head = deleteNode(head, holder->pid);
    }
    else if (return_pid == 0)
    {
      temp = temp->next;
    }
    else if (return_pid == temp->pid)
    {
      Node *holder = temp;
      temp = temp->next;
      printf("%s with pid %d has been terminated.\n", holder->path, holder->pid);
      head = deleteNode(head, holder->pid);
    }
  }
}

/*
  Function: func_BGkill
  Description: 
    Checks to see if the given pid exists. 
    If it does, it terminates the program associated with the pid and deletes it from the linked list. 
    If it does not, it prints an error message to the terminal.
  Parameters:
    - cmd: the pid of the process we want to terminate
*/
void func_BGkill(char **cmd)
{
  pid_t pid = str_to_int(cmd[1]);
  bool exists = PidExist(head, pid);
  if (exists == true)
  {
    kill(pid, SIGTERM);
    printf("%d was terminated\n", pid);
    head = deleteNode(head, pid);
  }
  else
  {
    printf("pid %d does not exist\n", pid);
  }
}

/*
  Function: str_to_int
  Description: 
    Takes a string of numbers and turns it into a pid_t
  Parameters: 
    - str: a string of numbers
*/
pid_t str_to_int(char str[])
{
  pid_t x = atoi(str);
  return x;
}

/*
  Function: func_BGstop
  Description: 
    Checks to see if the given pid exists. 
    If it does, it stops the program associated with the pid. 
    If it does not, it prints an error message to the terminal.
  Parameters:
    - cmd: the pid of the process we want to terminate
*/
void func_BGstop(char **cmd)
{
  pid_t pid = str_to_int(cmd[1]);
  bool exists = PidExist(head, pid);
  if (exists == true)
  {
    kill(pid, SIGSTOP);
    printf("%d was stopped\n", pid);
  }
  else
  {
    printf("pid %d does not exist\n", pid);
  }
}

/*
  Function: func_BGstart
  Description: 
    Checks to see if the given pid exists. 
    If it does, it continues the program associated with the pid. 
    If it does not, it prints an error message to the terminal.
  Parameters:
    - cmd: the pid of the process we want to terminate
*/
void func_BGstart(char **cmd)
{
  pid_t pid = str_to_int(cmd[1]);
  bool exists = PidExist(head, pid);
  if (exists == true)
  {
    kill(pid, SIGCONT);
    printf("%d was started again\n", pid);
  }
  else
  {
    printf("pid %d does not exist\n", pid);
  }
}

/*
  Function: func_pstat
  Description: 
    Checks to see if the given pid exists. 
    If it does, it calls print_stats and print_status
  Parameters: 
    - str_pid: the pid in the form of a string 
*/
void func_pstat(char *str_pid)
{
  pid_t int_pid = str_to_int(str_pid);
  bool exists = PidExist(head, int_pid);
  if (exists == true)
  {
    print_stats(str_pid);
    print_status(str_pid);
  }
  else
  {
    printf("pid %d does not exist\n", int_pid);
  }
}

/*
  Function: print_stats
  Description: 
    Opens the /proc/pid/stat file. 
    Obtains and prints the comm, state, utime and stime from the file. 
  Parameters: 
    - str_pid: a pid in the form of a string
  References: 
    Used: https://stackoverflow.com/questions/33266678/how-to-extract-information-from-the-content-of-proc-files-on-linux-using-c
    Referenced responses to obtain values from the /proc files. 
*/
void print_stats(char *str_pid)
{
  char filename[100] = "/proc/";
  strcat(filename, str_pid);
  strcat(filename, "/stat");

  FILE *file = fopen(filename, "r");
  char comm[1000];
  char state;
  long double utime;
  long double stime;
  long int rss;

  fscanf(file, "%*d %s %c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %Lf %Lf %*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu %ld", comm, &state, &utime, &stime, &rss);
  printf("comm: %s\n", comm);
  printf("state: %c\n", state);
  printf("utime: %.1Lf ms\n", utime);
  printf("stime: %.1Lf ms\n", stime);
  printf("rss: %ld\n", rss);
  fclose(file);
}

/*
  Function: print_status
  Description: 
    Opens the /proc/pid/status file. 
    Gets and prints the voluntary ctxt switches and the nonvoluntary ctxt switches. 
  Parameters: 
    - str_pid: the pid in the form of a string
*/
void print_status(char *str_pid)
{
  char filename[100] = "/proc/";
  strcat(filename, str_pid);
  strcat(filename, "/status");

  FILE *file = fopen(filename, "r");
  char holder[1000];
  for (int i = 0; i <= 53; i++)
  {
    fgets(holder, 1000, file);
  }
  printf("%s", holder);
  fgets(holder, 100, file);
  printf("%s", holder);

  fclose(file);
}