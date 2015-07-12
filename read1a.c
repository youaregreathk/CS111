/**************************************/
/* UCLA CS 111 Lab 1 command reading  */
/*  Contributors:                     */
/*  Kwai Hung Shea UID :304497354     */
/*  FnuPramono     UIDC:604498984     */
/**************************************/

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>

#define STACK_SIZE 50

//Lab 1a
/****************************************************************************************
 In this lab1 lab 1a weneed to implement make_command_stream, which should create and
 initialize a command_stream_t instance. We will also have to implement the
 read_command_stream. We use a stack data structure to help us storing the command we read
 and then pop it and store it into the tree.
 We also use the linked-list data structure to link the different tree togeather.
 
 *****************************************************************************************/




//linked list for the command tree
//*********************************************

struct command_stream {
    int read;
    command_stream_t next;
    command_t root;
    command_stream_t prev;
};




//Stack Implementation
//*****************Stack Begin***********************************

struct Stack
{
    int size;
    command_t node[STACK_SIZE];
    
};

int stack_size(struct Stack *S)
{
    return S->size;
}

int isEmpty(struct Stack *S)
{
    if(S->size==0)
        return 1;
    else
        return 0;
}

void begin_st (struct Stack *S)
{
    S->size = 0;
}


command_t Stack_Top(struct Stack *S)
{
    if (S->size == 0) {
        fprintf(stderr, "Error: stack empty\n");
        exit(0);
    }
    
    return S->node[S->size-1];
}


void Stack_Push(struct Stack *S, command_t c)
{
    if (S->size < STACK_SIZE){
        S->node[S->size] = c;
        S->size++;
    }
    
    else
        fprintf(stderr, "Error: stack full\n");
}


void Stack_Pop(struct Stack *S)
{
    if (S->size == 0)
        fprintf(stderr, "Error: stack empty\n");
    else
        S->size--;
}
//*********************************************************
//Determine if the char is a special token
int Token_S(const char c)
{
    return (c == ';' || c == '>' ||
            c == '(' || c == ')' || c == '<' ||
            c == '|' || c == '&');
}

//Determine if the char is a odinary token
int Token_O(const char c)
{
    return (isalpha(c) || c == '_' || c == '!' || c == '%' ||
            c == '+' || c == ',' || c == '-' ||	\
            c == '.' || c == '/' || c == ':'
            || c == '@' || c == '^' || isdigit(c));
}

//Remove the white space between command
void DeleteSpace(char *c)
{
    int j;
    int initil;
    //char
    char *pt;
    char *tab, *fchar;
    
    for(j = 0, tab = c; tab[j] != '\0'; j++)
    {
        if(tab[j] == '\t')
            tab[j] = ' ';
    }
    
    fchar = c;
    while(fchar[0] == ' ' || fchar[0] == '\n')
    {
        for(j = 0; fchar[j] != '\0'; j++)
            fchar[j] = fchar[j+1];
    }
    //The following code remove the white spaces if occur more than once
    //and move the character after the white space
    for(j = 0; c[j] != '\0' ; j++)
    {
        while (c[j] == ' ' && c[j+1] == ' ')
        {
            for(initil = j, pt = c; pt[initil] != '\0'; initil++)
                pt[initil] = pt[initil+1];
        }
    }
}


int check_buf(char *c)          //Determine the entire buffer
{
    int j = 0;
    //Set viraible for iteration
    int iter1;
    int iter2 = 0;
    DeleteSpace(c);
    int flag_andOr = 0;
    //A counter for counting the left (
    int count_lpar = 0;
    //A counter for counting the right )
    int count_rpar = 0;
    //the number of the line
    int lineNo = 0;
    
    if(!Token_O(c[0]) && c[0] != '(' && c[0] != '#')
    {
        fprintf(stderr, "Error: First character must be valid.\n");
        return 0;
    }
    
    if(c[j] == '\n')
        lineNo++;
    while(c[j] != '\0')  //Check the operator except the ()
    {
        flag_andOr = 0;
        iter1 = j;   //check token invalid input
        if(!Token_O(c[j]) && !Token_S(c[j]) && c[j] != ' ' && c[j] != '\n' && c[j] != '#')
        {
            fprintf(stderr, "Parse Error in Line %d: Token must be valid.\n", lineNo);
            return 0;
        }
        
        //Check no s token and o token after the comment
        else if(j != 0 && c[j] == '#' && (c[j-1] != ' ' && c[j-1] != '\n'))
        {
            fprintf(stderr, "Parse Error in Line %d: There cannot be an ordinary token before a comment.\n", lineNo);
            return 0;
        }

        else if(c[j] == '#')
        {                      //Once encounter a comment ,skip until a newline
            while(c[j] != '\n' && c[j] != '\0')
                j++;
            continue;
        }
        
        
        else if(c[j] == '<')
        {                        //check < have valid token in both sides
            while(c[iter1-1] == ' ')
                iter1--;
            if(c[iter1-1] == '\n' || Token_S(c[iter1-1]))
            {
                fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or newline before IO redirect.\n", lineNo);
                return 0;
            }
            
            iter1 = j;
            while(c[iter1+1] == '\n' || c[iter1+1] == ' ')
                iter1++;
            
            if(!Token_O(c[iter1+1]))
            {
                if(c[iter1+1] != '>' && c[iter1+1] != '&'){
                    fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or blank after IO redirect.\n", lineNo);
                    return 0;
                }
            }
        }
        
        
        else if(c[j] == '>')
        {                  //Check > have valid tokens on both sides
            while(c[iter1-1] == ' ')
                iter1--;
            if(c[iter1-1] == '\n' || Token_S(c[iter1-1]))
            {
                if(c[iter1-1] != '>' && c[iter1-1] != '<'){
                    fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or newline before IO redirect.\n", lineNo);
                    return 0;
                }
                
                if(c[iter1-1] == '>' && c[iter1+1] == '>'){
                    fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or newline before IO redirect.\n", lineNo);
                    return 0;
                }
            }
            
            iter1 = j;
            while(c[iter1+1] == '\n' || c[iter1+1] == ' ')
                iter1++;
            
           
            if(!Token_O(c[iter1+1]))
            {                                     //If next token is special or blank
                if(c[iter1+1] != '>' && c[iter1+1] != '&' && c[iter1+1] != '|'){
                    fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or blank after IO redirect.\n", lineNo);
                    return 0;
                }
                
                if(c[iter1-1] == '>' && c[iter1+1] == '>'){
                    fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or newline before IO redirect.\n", lineNo);
                    return 0;
                }
            }
        }
        
        
        else if(c[j] == ';')
        {                      //check if ; have valid token on left side
            while(c[iter1-1] == ' ')
                iter1--;

            if(c[iter1-1] == '\n' || Token_S(c[iter1-1]))
            {                 //print erro
                fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or newline before semicolon.\n", lineNo);
                return 0;
            }
            
            iter1 = j;
            while(c[iter1+1] == '\n' || c[iter1+1] == ' ')
                iter1++;
            if(Token_S(c[iter1+1]))
            {
                fprintf(stderr, "Parse Error in Line %d: Cannot have a special token after semicolon.\n", lineNo);
                return 0;
            }
        }
        
        
        else if(c[j] == '|')
        {                     //Check that | has valid tokens on the left side
            while(c[iter1-1] == ' ')
                iter1--;
            
            
            if(c[iter1-1] == '\n' || Token_S(c[iter1-1]))
            {//If previous token is special or newline
                
                if(c[iter1-1] != '>'){  //If previous token is not >,then print error
                    fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or newline before pipe.\n", lineNo);
                    return 0;
                }
            }
            
            iter1 = j;
            while(c[iter1+1] == '\n' || c[iter1+1] == ' ')
                iter1++;
            if(!Token_O(c[iter1+1]) && c[iter1+1] != '|')
            {
                fprintf(stderr, "Parse Error in Line %d: Cannot have a special token that is not '|' after pipe.\n", lineNo);
                return 0;
            }
        }
        
        
        
        if(c[j]=='|' && c[j+1]!='|')
        {                           //Regular if statement because we would have entered the conditional that checks for pipes
            iter1 = j;
            
          
            while(c[iter1+1] == '\n' || c[iter1+1] == ' ')
                iter1++;
            
            if(!Token_O(c[iter1+1]))
            
            {       //If next non-space or non-newline is an operator or blank, error
                fprintf(stderr, "Parse Error in Line %d: Cannot have a special token after OR.\n", lineNo);
                return 0;
            }
        }
        
        else if(c[j] == '&' && c[j+1] != '&')
        {
            if(c[j-1] != '<' && c[j-1] != '>'){
                fprintf(stderr, "Parse Error in Line %d: Must have two consecutive '&' for a valid AND command.", lineNo);
                return 0;
            }
        }
        
        if((c[j] == '&' && c[j+1] == '&') || (c[j] == '|' && c[j+1] == '|'))
        {                               //Check that and-ors have an appropriate lhs and rhs
            flag_andOr = 1;
            
            while(c[iter1-1] == ' ')
                iter1--;
            
            
            if(c[iter1-1] == '\n' || Token_S(c[iter1-1]))
            {
                fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or newline before and-or.\n", lineNo); //If the previous token is a newline or a special token, error
                return 0;
            }
            
            iter1 = j;
            while(c[iter1+2] == '\n' || c[iter1+2] == ' ')
                iter1++;
            
            //If the next token is a special token or blank, error
            if(!Token_O(c[iter1+2]))
            {
                fprintf(stderr, "Parse Error in Line %d: Cannot have a special token or blank after and-or.\n", lineNo);
                return 0;
            }
        }
        
        if(flag_andOr)
            j += 2;
        
        else
            j++;
    }
    
    while(c[iter2] != '\0')
    {
        if(c[iter2] == '(')
            count_lpar++;
        
        else if(c[iter2] == ')')
            count_rpar++;
        
        iter2++;
    }
    
    
    if(count_lpar != count_rpar)
    {
        fprintf(stderr, "Number of left and right parentheses have to match.\n");
        return 0;
    }   //If different number of left and right parentheses, error
    
    return 1;
}


// The command tree creator
command_t make_command_tree (char *c, int noClobber) {
    struct Stack cmdStack;
    struct Stack oprStack;
    
    begin_st(&cmdStack);
    begin_st(&oprStack);
    
    
    int t = 0;
    while(c[t] != '\0'){     //iterate the entire buffer
        command_t x;
        x = (command_t)malloc(sizeof(struct command));
        
        
        if(Token_O(c[t])){
            x->type = SIMPLE_COMMAND;
            int count = 0;      // For ordinary token and creation of SIMPLE commands
            x->u.word = (char**)malloc(sizeof(char*) * 100);
            
           
            int j = 0;
            while(j < 100){
                x->u.word[j] = (char*)malloc(sizeof(char) * 100);
                j++;                  //allocate space
            }
            
            while(Token_O(c[t]) || c[t] == ' '){
                
                int len = 0;
                while(Token_O(c[t])){     //For ordinary tokens
                    x->u.word[count][len] = c[t];
                    len++;
                    t++;
                }
                if(len > 0){
                    x->u.word[count][len] = '\0';
                    count++;
                }
                
                if(c[t] == ' '){
                    t++;
                }
            }
            
            x->u.word[count] = '\0';
            Stack_Push(&cmdStack, x);
        }
        
        else if(c[t] == '|' && c[t+1] != '|' && c[t-1] != '>'){
                 // For PIPELINE commands
            x->type = PIPE_COMMAND;
               //pop off from stack
            while(oprStack.size != 0 && Stack_Top(&oprStack)->type == PIPE_COMMAND){
                command_t oprTemp = Stack_Top(&oprStack);
                Stack_Pop(&oprStack);
                
                command_t cmdTemp1 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                command_t cmdTemp2 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                
                oprTemp->u.command[0] = cmdTemp2;
                oprTemp->u.command[1] = cmdTemp1;
                
                Stack_Push(&cmdStack, oprTemp);
            }
            
            Stack_Push(&oprStack, x);
            t++;
        }
        
        else if(c[t] == '|' && c[t+1] == '|'){
            x->type = OR_COMMAND;         // For OR Commands
            
            while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND)){
                command_t oprTemp = Stack_Top(&oprStack);
                Stack_Pop(&oprStack);
                
                command_t cmdTemp1 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                command_t cmdTemp2 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                
                oprTemp->u.command[0] = cmdTemp2;
                oprTemp->u.command[1] = cmdTemp1;
                
                Stack_Push(&cmdStack, oprTemp);
            }
            
            Stack_Push(&oprStack, x);
            
            t+=2;   //skip 2 digits
         }
        
       
        else if(c[t] == '&' && c[t-1] != '<' && c[t-1] != '>'){
            x->type = AND_COMMAND;    // For AND Commands
            
            while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND)){
                command_t oprTemp = Stack_Top(&oprStack);
                Stack_Pop(&oprStack);
                
                command_t cmdTemp1 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                command_t cmdTemp2 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                
                oprTemp->u.command[0] = cmdTemp2;
                oprTemp->u.command[1] = cmdTemp1;
                
                Stack_Push(&cmdStack, oprTemp);
            }
            
            
            Stack_Push(&oprStack, x);
            t+=2;   //Skip 2 elements since AND has two digits
        }
      
        else if(c[t] == ';'){
                                       // For SEQUENCE commands
            if(c[t+1] != '\0'){
                
                if(c[t+1] == ' '){
                    if(c[t+2] != '\0'){
                        
                        x->type = SEQUENCE_COMMAND;
                        
                        while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND || Stack_Top(&oprStack)->type == SEQUENCE_COMMAND)){
                            command_t oprTemp = Stack_Top(&oprStack);
                            Stack_Pop(&oprStack);
                            
                            command_t cmdTemp1 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            command_t cmdTemp2 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            
                            oprTemp->u.command[0] = cmdTemp2;
                            oprTemp->u.command[1] = cmdTemp1;
                            
                            Stack_Push(&cmdStack, oprTemp);
                        }
                        
                        Stack_Push(&oprStack, x);
                    }
                }
                
                else{
                                  //No space immediately after
                    x->type = SEQUENCE_COMMAND;
                    
                    while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND || Stack_Top(&oprStack)->type == SEQUENCE_COMMAND)){
                        command_t oprTemp = Stack_Top(&oprStack);
                        Stack_Pop(&oprStack);
                        
                        command_t cmdTemp1 = Stack_Top(&cmdStack);
                        Stack_Pop(&cmdStack);
                        command_t cmdTemp2 = Stack_Top(&cmdStack);
                        Stack_Pop(&cmdStack);
                        
                        oprTemp->u.command[0] = cmdTemp2;
                        oprTemp->u.command[1] = cmdTemp1;
                        
                        Stack_Push(&cmdStack, oprTemp);
                    }
                    
                    Stack_Push(&oprStack, x);
                }
                
            }
            
            t++;
            
        }
        
        
        //For NEWLINE commands
        else if(c[t] == '\n'){
            
            if(c[t+1] != '\0' && !Token_S(c[t-1])){
                
                if(c[t+1] == ' ' && c[t-1] == ' '){
                    
                    if(c[t+2] != '\0' && !Token_S(c[t-2])){
                        
                        x->type = SEQUENCE_COMMAND;
                        
                        while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND || Stack_Top(&oprStack)->type == SEQUENCE_COMMAND)){
                            command_t oprTemp = Stack_Top(&oprStack);
                            Stack_Pop(&oprStack);
                            
                            command_t cmdTemp1 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            command_t cmdTemp2 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            
                            oprTemp->u.command[0] = cmdTemp2;
                            oprTemp->u.command[1] = cmdTemp1;
                            
                            Stack_Push(&cmdStack, oprTemp);
                        }
                        
                        Stack_Push(&oprStack, x);
                    }
                }
                
                else if(c[t+1] == ' ' && c[t-1] != ' '){
                    
                    if(c[t+2] != '\0'){
                        
                        x->type = SEQUENCE_COMMAND;
                        
                        while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND || Stack_Top(&oprStack)->type == SEQUENCE_COMMAND)){
                            command_t oprTemp = Stack_Top(&oprStack);
                            Stack_Pop(&oprStack);
                            
                            command_t cmdTemp1 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            command_t cmdTemp2 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            
                            oprTemp->u.command[0] = cmdTemp2;
                            oprTemp->u.command[1] = cmdTemp1;
                            
                            Stack_Push(&cmdStack, oprTemp);
                        }
                        
                        Stack_Push(&oprStack, x);
                    }
                    
                }
                
                
                else if(c[t+1] != ' ' && c[t-1] == ' '){
                    
                    if(!Token_S(c[t-2])){
                        
                        x->type = SEQUENCE_COMMAND;
                        
                        while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND || Stack_Top(&oprStack)->type == SEQUENCE_COMMAND)){
                            command_t oprTemp = Stack_Top(&oprStack);
                            Stack_Pop(&oprStack);
                            
                            command_t cmdTemp1 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            command_t cmdTemp2 = Stack_Top(&cmdStack);
                            Stack_Pop(&cmdStack);
                            
                            oprTemp->u.command[0] = cmdTemp2;
                            oprTemp->u.command[1] = cmdTemp1;
                            
                            Stack_Push(&cmdStack, oprTemp);
                        }
                        
                        Stack_Push(&oprStack, x);
                    }
                    
                }
                
                
                else{
                    
                    x->type = SEQUENCE_COMMAND;
                    
                    while(oprStack.size != 0 && (Stack_Top(&oprStack)->type == PIPE_COMMAND || Stack_Top(&oprStack)->type == OR_COMMAND || Stack_Top(&oprStack)->type == AND_COMMAND || Stack_Top(&oprStack)->type == SEQUENCE_COMMAND)){
                        command_t oprTemp = Stack_Top(&oprStack);
                        Stack_Pop(&oprStack);
                        
                        command_t cmdTemp1 = Stack_Top(&cmdStack);
                        Stack_Pop(&cmdStack);
                        command_t cmdTemp2 = Stack_Top(&cmdStack);
                        Stack_Pop(&cmdStack);
                        
                        oprTemp->u.command[0] = cmdTemp2;
                        oprTemp->u.command[1] = cmdTemp1;
                        
                        Stack_Push(&cmdStack, oprTemp);
                    }
                    
                    Stack_Push(&oprStack, x);
                }
                
                
            }
            
            t++;
            
        }
        
        
        
        else if(c[t] == '('){          // For beginning parenthesis leading to SUBSHELL commands
            x->type = SUBSHELL_COMMAND;
            Stack_Push(&oprStack, x);
            t++;
        }
        
        
        else if(c[t] == ')'){
            while(Stack_Top(&oprStack)->type != SUBSHELL_COMMAND){
                command_t oprTemp = Stack_Top(&oprStack);
                Stack_Pop(&oprStack);
                // For ending parenthesis taking care of the creation of SUBSHELL commands
                command_t cmdTemp1 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                command_t cmdTemp2 = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                
                oprTemp->u.command[0] = cmdTemp2;
                oprTemp->u.command[1] = cmdTemp1;
                
                Stack_Push(&cmdStack, oprTemp);
            }
            
            command_t oprTemp = Stack_Top(&oprStack);
            Stack_Pop(&oprStack);
            
            command_t cmdTemp = Stack_Top(&cmdStack);
            Stack_Pop(&cmdStack);
            
            oprTemp->u.subshell_command = cmdTemp;
            Stack_Push(&cmdStack, oprTemp);
            t++;
        }
        
        
        else if(c[t] == '>' && c[t+1] != '>' && c[t+1] != '&' && c[t+1] != '|'){
                                       //output
            if(noClobber == 0){
                t++;
                while(!Token_O(c[t])){
                    t++;
                }
                
                command_t cmdTemp = Stack_Top(&cmdStack);
                Stack_Pop(&cmdStack);
                
                cmdTemp->output = (char*)malloc(sizeof(char) * 100);
                
                int len = 0;
                while(Token_O(c[t])){
                    cmdTemp->output[len] = c[t];
                    len++;
                    t++;
                }
                
                cmdTemp->output[len] = '\0';
                Stack_Push(&cmdStack, cmdTemp);
            }
            
            else{
                t++;
                while(!Token_O(c[t])){
                    t++;
                }
                
                while(Token_O(c[t])){
                    t++;
                }
            }
            
        }
        
        

        else if(c[t] == '<' && c[t+1] != '>' && c[t+1] != '&'){
                                       //the input
            t++;
            while(!Token_O(c[t])){
                t++;
            }
            
            command_t cmdTemp = Stack_Top(&cmdStack);
            Stack_Pop(&cmdStack);
            
            cmdTemp->input = (char*)malloc(sizeof(char) * 100);
            
            int len = 0;
            while(Token_O(c[t])){
                cmdTemp->input[len] = c[t];
                len++;
                t++;
            }
            
            cmdTemp->input[len] = '\0';
            Stack_Push(&cmdStack, cmdTemp);
        }
        
        else if(c[t] == '>' && c[t+1] == '>'){
            while(!Token_O(c[t])){
                t++;
            }
            
            command_t cmdTemp = Stack_Top(&cmdStack);
            Stack_Pop(&cmdStack);
            
            cmdTemp->append = (char*)malloc(sizeof(char) * 100);
            
            int len = 0;
            while(Token_O(c[t])){
                cmdTemp->append[len] = c[t];
                len++;
                t++;
            }
            
            cmdTemp->append[len] = '\0';
            Stack_Push(&cmdStack, cmdTemp);
        }
        
        else if(c[t] == '>' && c[t+1] == '|'){
            t++;                   // The NOCLOBBER OUTPUTS
            while(!Token_O(c[t])){
                t++;
            }
            
            command_t cmdTemp = Stack_Top(&cmdStack);
            Stack_Pop(&cmdStack);
            
            cmdTemp->clobber_output = (char*)malloc(sizeof(char) * 100);
            
            int len = 0;
            while(Token_O(c[t])){
                cmdTemp->clobber_output[len] = c[t];
                len++;
                t++;
            }
            
            cmdTemp->clobber_output[len] = '\0';
            Stack_Push(&cmdStack, cmdTemp);
        }
        
        
        else if(c[t] == '<' && c[t+1] == '>'){
            t++;
            while(!Token_O(c[t])){
                t++;                      // The input and output
            }
            
            command_t cmdTemp = Stack_Top(&cmdStack);
            Stack_Pop(&cmdStack);
            
            cmdTemp->input = (char*)malloc(sizeof(char) * 100);
            cmdTemp->output = (char*)malloc(sizeof(char) * 100);
            
            int len = 0;
            while(Token_O(c[t])){
                cmdTemp->input[len] = c[t];
                cmdTemp->output[len] = c[t];
                len++;
                t++;
            }
            
            cmdTemp->input[len] = '\0';
            cmdTemp->output[len] = '\0';
            Stack_Push(&cmdStack, cmdTemp);
        }
        
       
        else if(c[t] == '<' && c[t+1] == '&'){
            t++;
            while(!Token_O(c[t])){
                t++;
            }                               // INPUT DUPLICATE
            
            command_t cmdTemp = Stack_Top(&cmdStack);
            Stack_Pop(&cmdStack);
            
            cmdTemp->input_dup = (char*)malloc(sizeof(char) * 100);
            
            int len = 0;
            while(Token_O(c[t])){
                cmdTemp->input_dup[len] = c[t];
                len++;
                t++;
            }
            
            cmdTemp->input_dup[len] = '\0';
            Stack_Push(&cmdStack, cmdTemp);
        }
        
        
        // OUTPUT DUPLICATE
        else if(c[t] == '>' && c[t+1] == '&'){
            t++;
            while(!Token_O(c[t])){
                t++;
            }
            
            command_t cmdTemp = Stack_Top(&cmdStack);
            Stack_Pop(&cmdStack);
            
            cmdTemp->output_dup = (char*)malloc(sizeof(char) * 100);
            
            int len = 0;
            while(Token_O(c[t])){
                cmdTemp->output_dup[len] = c[t];
                len++;
                t++;
            }
            
            cmdTemp->output_dup[len] = '\0';
            Stack_Push(&cmdStack, cmdTemp);
        }
        
        // For empty spaces
        else{
            t++;
        }
    }
    
    
    while(oprStack.size != 0){
        command_t oprTemp = Stack_Top(&oprStack);
        Stack_Pop(&oprStack);
        // Creating the tree from whatever is left
        command_t cmdTemp1 = Stack_Top(&cmdStack);
        Stack_Pop(&cmdStack);
        command_t cmdTemp2 = Stack_Top(&cmdStack);
        Stack_Pop(&cmdStack);
        
        oprTemp->u.command[0] = cmdTemp2;
        oprTemp->u.command[1] = cmdTemp1;
        Stack_Push(&cmdStack, oprTemp);
    }
    
    command_t tree = (command_t)malloc(sizeof(struct command));
    tree = Stack_Top(&cmdStack);
    
    Stack_Pop(&cmdStack);
    
    return tree;
}


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
                     void *get_next_byte_argument, int noClobber)
{
    int size = 2000;
    char *buffer = (char*) malloc(sizeof(char) * size); //Dynamically allocated array
    char c;
    int count = 0;//Counter for indexing in my dynamically allocated array
    
    if (buffer == NULL) //Returns an error if buffer is NULL
    {
        fprintf(stderr, "Error when using 'buffer' malloc.");
        exit(1);
    }
    
    c = get_next_byte(get_next_byte_argument);
    
    while(c != EOF)
    {
        buffer[count] = c;
        count++;
        
     
        if(count >= size)
        {               //Reallocate the size if needed
            buffer = (char*)realloc(buffer, size*2);
            
            if(buffer == NULL)
            {
                fprintf(stderr, "Error when using 'buffer' malloc.");
                exit(1);
            }
            
            size *= 2;//Adjust size for future reallocations
        }
        
        c = get_next_byte(get_next_byte_argument);
    }
    
    buffer[count] = '\0';
    
    DeleteSpace(buffer);
    
    if(!check_buf(buffer)){
        fprintf(stderr, "Line 784: Buffer is invalid");
        exit(1);
    }
    
    command_stream_t stream = NULL;
    command_stream_t follower = NULL;
    
    count = 0;
    
    while(buffer[count] != '\0'){
        
        char *temp_buffer = (char*) malloc(sizeof(char) * size);
        int len = 0;
        
        while(buffer[count] != '\0'){
            
            char temp_immediate_prev = buffer[count-1];
            char temp_preceding = buffer[count-2];
            
            
            if(buffer[count] == '#'){
                while(buffer[count] != '\n'){
                    count++;
                }
                
                count++;
            }
            
            
            if((buffer[count] == '\n' && buffer[count + 1] == '\n') || (buffer[count] == '\n' && buffer[count+2] == '\n') || (buffer[count] == ' ' && buffer[count+1] == '\n' && buffer[count+2] == '\n') || (buffer[count] == ' ' && buffer[count+1] == '\n' && buffer[count+3] == '\n')){
                
                while(buffer[count] == '\n' || buffer[count] == ' '){
                    count++;
                }
                
                if(!Token_S(temp_immediate_prev)){
                    if(temp_immediate_prev == ' '){
                        if(!Token_S(temp_preceding)){
                            break;
                        }
                    }
                    
                    else{
                        break;
                    }
                }
                
            }
            
            temp_buffer[len] = buffer[count];
            len++;
            count++;
        }
        
        //Command trees are separated by >= 2 commands
        temp_buffer[len] = '\0';
        
        if(stream == NULL && follower == NULL){
            stream = (command_stream_t)malloc(sizeof(struct command_stream));
            stream->read = 0;
            stream->root = (command_t)malloc(sizeof(struct command));
            stream->root = make_command_tree(temp_buffer, noClobber);
            stream->prev = NULL;
            stream->next = NULL;
        }
        
        else if(stream != NULL && follower == NULL){
            stream->next = (command_stream_t)malloc(sizeof(struct command_stream));
            stream->next->read = 0;
            stream->next->root = (command_t)malloc(sizeof(struct command));
            stream->next->root = make_command_tree(temp_buffer, noClobber);
            follower = stream->next;
            
            follower->prev = stream;
            follower->next = NULL;
        }
        
        else{
            command_stream_t temp = (command_stream_t)malloc(sizeof(struct command_stream));
            temp->read = 0;
            temp->root = (command_t)malloc(sizeof(struct command));
            temp->root = make_command_tree(temp_buffer, noClobber);
            
            follower->next = temp;
            temp->prev = follower;
            temp->next = NULL;
            follower = follower->next;
        }
        
    }
    
    return stream;
}

//The read part
command_t
read_command_stream (command_stream_t s)
{
    if(s == NULL)
        return NULL;
    else if(s != NULL && s->read == 0){
        command_t tree;
        tree = s->root;
        s->read++;
        return tree;
    }
    
    else if(s != NULL && s->next != NULL){
        return read_command_stream(s->next);
    }
    
    else{
        return NULL;
    }
}
