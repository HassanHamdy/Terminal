/* ****************************************************************   */
/*								                                      */
/*		        ReadLine Library (Dont remove CopyRight)	          */
/*								                                      */
/* ****************************************************************   */

/* Copyright (C) 1987-2009 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library for
   reading lines of text with interactive input and history editing.

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

//create history list as external for readline library
extern HIST_ENTRY **history_list ();
FILE *fp,*fr;
//for concatenation Strings
void concat(char *s1,char *s2);
//for Run a Command
void *Run_Command_in_background(void *);
//to build Terminal Default path
void Construct_dir(char *def);

int main ()
{

    /*****  Controlling Shell  ******/
    char *Command, *prompt;
    printf("--------------------------------- UNIX Shell -----------------------------------\n \t \t Copyright (C) 2016-2017 OS2 Team Foundation\n \t \t MultiThreading Based | 'exit' to terminate\n--------------------------------------------------------------------------------\n");
    prompt = "osh> ";

    /******  Program Predefined  ******/
    char *Left,*Right,*subcommand;
    char Redirect_inp[] = "< input.txt";
    char Redirect_out[] = "> Result.txt";
    char Redirect[]= " Result.txt";
    fp = fopen("CommandList.txt","a+");   //a+ for append and create file if its not found
    int i,c,j,indexOfChar,ConvertedChar,exist = 0,ret,check =0;
    pthread_t thread1;
    char* home = getenv("HOME");  //getenv => getenvirnoment get the default path for Home
    concat(home,"/");                  //prepare to build default path
    //create pointers
    Left = (char *)malloc(20*sizeof(char));
    Right = (char *)malloc(20*sizeof(char));
    subcommand = (char *)malloc(20*sizeof(char));

    Construct_dir(home);

    while (1)
    {
        /*******  Point to The Entered Command  ************/
        if (check == 0)
        Command = readline(prompt);    //pointer to the input Line  in the Shell , readline => read line from console
        /***************** Controlling Shell *************/
        // if user input exit or zero => terminate  (exit can be changed)
        // if user type list => show all history (Optional)
        // NOTE : if user enter up button => it will give him the previous command he entered
        if (*Command == '0' || strcmp(Command, "exit") == 0)
        {
            break;   //traced
        }
        if (*Command)
        {
            add_history(Command);/* If there is an command ,remember it. 'readline Library' */
        }
        if(strstr(Command,"|"))      //strstr return 1 if it found | in the command
        {
            //seperate each command
            for(i = 0; Command[i] != '|'; i++)
            {
                if(Command[i+1] == '|')  // Command[i+1] == '|'   ->  until reach the | in command
                    break;
                concat(Left,&Command[i]);  // it will concatenate left with first command before '|'
            }

            i = i+3;  // ^_^ to skip three index which are ->  'SPACE'|'SPACE'

            for(; Command[i] != '\0'; i++)  //Command[i] != '\0'  3shan a2ra le a5r el second command
            {
                concat(Right,&Command[i]);
            }

            for(i = 0; i <= strlen(Redirect_out); i++)  // "> Result.txt"
            {
                concat(Left,&Redirect_out[i]);   // firstcommand > Result.txt    
            }

            printf("[SYS_MSG] : In the Main Thread !");

            //run thread w htnade le function w tb3tla left(1st command)
            if(pthread_create(&thread1,NULL,Run_Command_in_background,(void *)Left) != 0)
                perror("pthread_create"),exit(1);

            // we will wait until 1st command thread to finish
            if(pthread_join(thread1,NULL) != 0)
                perror("pthread_join"),exit(1);

            printf("[SYS_MSG] : Return back to main Thread !\n");


            for(i = 0; i <= strlen(Redirect); i++)  // " Result.txt"
            {
                concat(Right,&Redirect[i]);  // 1st output will be input to 2nd command
            }

             printf("[SYS_MSG] : In the Main Thread !");

            // thread with 2nd command (right)
            if(pthread_create(&thread1,NULL,Run_Command_in_background,(void *)Right) != 0)
                perror("pthread_create"),exit(1);

            // wait to execute
            if(pthread_join(thread1,NULL) != 0)
                perror("pthread_join"),exit(1);

            printf("[SYS_MSG] : Return back to main Thread !\n");

        }
        //in case Command is !!
        else if(strstr(Command,"!"))
        {

            if(Command[0] == '!')
            {
                 int count = 0;
                 char Lcommand[50] , blCommand[50];
                 indexOfChar = strchr(Command,'!') - Command ;  //kda ana bgeb el index bta3 !
                 i = 1;
                exist = 0; //for check for existence of N.of Line User Entered

                if(isdigit(Command[indexOfChar+1]))  //lw 2le b3d el ! kan rkm  as : !5
                {
                    ConvertedChar = atoi(&Command[indexOfChar+1]);  //hna el 5 deh char  hna hwa be convert le int

                    while((c = getc(fp)) != EOF)
                    {
                        if( ConvertedChar == i  ) //bmshe 3la el Lines w lma 2wsl msl le Line 5 2tl3 el command
                        {
                            //putchar(c);
                             Lcommand[count] = c;
                             count++;
                            exist = 1;
                        }
                        if(c == '\n')
                            i++;
                    }
                    if(exist == 0 )
                        printf("[SYS_MSG] : Sorry number you have entered is wrong\n");
                    else{
                        i=0;
                        while (Lcommand[i] != '\n'){
                        blCommand[i] = Lcommand[i];
                        //  putchar(blCommand[i]);
                        i++;
                        }
                        blCommand[i] ='\0';
                      //  printf("%s\n", blCommand);
                        Command = blCommand;
                        check = 1;
                        rewind(fp);
                        continue;
                    }
                    rewind(fp);

                }



               else if(Command[indexOfChar+1] == '!')
               {  // !! -> the most recent command in the history
                 char test;
                 if (( test = getc(fp)) != EOF)     //ftell(fp) == 0  file is empty
                 {
                        j = 0 ;
                        int count = 0;
                        char Lcommand[50] , blCommand[50];
                        char slash [] = "\n";
                        while((c = getc(fp)) != EOF)   //read last thing in file
                        {
                            if(c == '\n')
                                j++;
                        }
                        rewind(fp);  //return first index
                        i = 0;

                        while((c = getc(fp)) != EOF)
                        {
                            if(i == j-1) //3shan el j bymshe 3la \n  j-1 hygeb a5r command not \n
                            {
                             Lcommand[count] = c;
                             count++;
                            }
                            if(c == '\n')
                                i++;
                        }
                        i=0;
                        while (Lcommand[i] != '\n'){  //Lcommand m3ah a5r command with \n
                           blCommand[i] = Lcommand[i];   //blCommand => command only
                           i++;
                        }
                        blCommand[i] ='\0';
                        printf("%s\n", blCommand);
                        Command = blCommand;
                        check = 1;
                        rewind(fp);
                        continue;   //to work with the target command
                 }else
                   printf("[ERROR_MSG] : Not Satisfied Commands In File ! \n");
               }

            }else{
                printf("[ERROR_MSG] : You have entered Wrong Command ^_^\n");
                continue;
            }
        }
        //in case Command contain & (to run in background)
        else if(strstr(Command,"&"))
        {
            int s = strlen(Command);
            if(Command[s-1] == '&') //check for last index in command
            {
            for(i = 0; Command[i] != '&'; i++)     //ls &    .:. ls -> subcommand
            {
                concat(subcommand,&Command[i]);
            }

            for(i = 0; i <= strlen(Redirect_inp); i++)   // "< input.txt"
            {
                concat(subcommand,&Redirect_inp[i]);   // ls < input.txt   => y3ne b5leh ya5od el input mn 7aga tanya
            }

            printf("[SYS_MSG] : In the Main Thread !");

            if(pthread_create(&thread1,NULL,Run_Command_in_background,(void *)subcommand) != 0)
                perror("pthread_create"),exit(1);

            if(pthread_join(thread1,NULL) != 0)
                perror("pthread_join"),exit(1);
                printf("[SYS_MSG] : Return back to Main Thread !\n");
            }else{
                printf("[ERROR_MSG] : You have entered Wrong Command ^_^\n");
                continue;
            }
        }
        else if(strstr(Command,"cd")) 
        {
            
            if(Command[0] == 'c' && Command[1] == 'd' && Command[2] == ' ')
            {

            for(i = 3; i <= strlen(Command); i++)  //i=3  after cd'SPACE' => kda m3aya el Path 2le 3wz yero7o
            {
                concat(subcommand,&Command[i]);
            }
            if(strcmp(subcommand,"~") == 0)     // to return user to home directory
            {
                // work 100% (try to print anything)
                int k = chdir(home);
                if(k < 0)
                    perror("CD");
                else
                {
                    if(check == 0){
                    system("pwd");
                    concat(Command,"\n");
                    fprintf(fp,Command);
                    rewind(fp);
                    }
                }
            }
            else if(subcommand[0] == '~' && strlen(subcommand) > 1)
            {
                char path[] = "/home/Name_Of_your_Device/"; 

                //subcommand : ~/Desktop/OS_Version5
                for(i = 2; i <= strlen(subcommand); i++)
                {
                   concat(path,&subcommand[i]);  //concat. home with Desktop/OS_Version5
                }

                int k = chdir(path);
                if(k < 0)
                    perror("CD");
                else
                {
                    if(check == 0)  //make sure [of using !!] so not repeated command
                    {
                        system("pwd");
                        concat(Command,"\n");
                        //printf("in file %s \n",Command);
                        fprintf(fp,Command);
                        rewind(fp);
                    }
                }
            }
            else   //cd anydir  [no need now to define a function for ../]
            {
            //the path is the default now so subcommand will change the directory directly without concat. path
                if(chdir(subcommand) == -1)
                    perror("CD");
                else
                {   //edited by tawfik
                   if(check == 0)
                   {
                    concat(Command,"\n");
                    fprintf(fp,Command);
                    rewind(fp);
                    system("pwd");
                    }
                }
            }

            /* Check for Cd Parameters ..*/
            }else{
                printf("[ERROR_MSG] : You have entered Wrong Command ^_^\n");
                continue;
         }

        }
        // ordinary CommandLine [handle Enter without input command]
		else if(Command[0] == '\0' || Command[0] == '\n' || (Command[0] == ' ' && strlen(Command) == 1))
		{
			continue;
		}
        else
        {
            printf("[SYS_MSG] : In the Main Thread !");


            if(pthread_create(&thread1,NULL,Run_Command_in_background,(void *)Command) != 0)
                perror("pthread_create"),exit(1);

            if(pthread_join(thread1,NULL) != 0)
                perror("pthread_join"),exit(1);

            printf("[SYS_MSG] : Return back to Main Thread !\n");

        }

        /*** End of Iteration ***/
        memset(Left,0,strlen(Left));
        memset(Right,0,strlen(Right));
        memset(subcommand,0,strlen(subcommand));
        memset(Command,0,strlen(Command));
        check = 0;
    }

    /**************  Finally ********************/

    fclose(fp);
    free(Command);
    free(Left);
    free(Right);
    free(subcommand);


    return 0;
}

void concat(char *s1,char *s2)
{
    int i = 0;       //s1 = /home/   s2 = Desktop

    i = strlen(s1);  // i = 6

    s1[i] = *s2;    //as s1[6] is empty now  as last inserted index was s1[5]

    i = i+1;
    s1[i] = '\0';
}

void *Run_Command_in_background(void *cmd)
{
    printf("\n\n [MSG] : In the Background Thread !! \n\n");
    if(!system(cmd))    //system(cmd) => return 0 if executed    !0 = 1  if true
    {
        //concatenate the command string and save it in command file
        concat(cmd,"\n");
        fprintf(fp,cmd);
        rewind(fp);   // return the cursor to index 0 in file

    }
    else
    {
        printf("You have entered Wrong Command");
    }
    printf("\n\n");
    return 0;
}

void Construct_dir(char *def)
{
   chdir(def);  //mb3thash le Run_Command_in_background 3shan kan hynfz 2le feha
}
