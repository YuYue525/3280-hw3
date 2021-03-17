/*
* CSCI3280 Introduction to Multimedia Systems *
* --- Declaration --- *
* I declare that the assignment here submitted is original except for source
* material explicitly acknowledged. I also acknowledge that I am aware of
* University policy and regulations on honesty in academic work, and of the
* disciplinary guidelines and procedures applicable to breaches of such policy
* and regulations, as contained in the website
* http://www.cuhk.edu.hk/policy/academichonesty/ *
* Assignment 3
* Name : YU Yue
* Student ID : 1155124490
* Email Addr : 1155124490@link.cuhk.edu.hk
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define CODE_SIZE  12
#define TRUE 1
#define FALSE 0

#define MAX_TREE_NODES 4096
unsigned int current_code = 0;

/* function prototypes */
/*
struct TreeNode
{
    char c;
    int pre_node;
};

typedef struct TreeNode Tree[MAX_TREE_NODES];

void initTree(Tree tree)
{
    for(int i =0; i<MAX_TREE_NODES; i++)
    {
        if(i<256)
        {
            tree[i].c = i;
            tree[i].pre_node = MAX_TREE_NODES - 1;
        }
        else
        {
            tree[i].c = '\0';
            tree[i].pre_node = MAX_TREE_NODES - 1;
        }
    }
}
*/
struct TreeNode
{
    char c;
    int pre_node;
    int next_node;
    int son_node;
};

typedef struct TreeNode Tree[MAX_TREE_NODES];

void initTree(Tree tree)
{
    for(int i =0; i<MAX_TREE_NODES; i++)
    {
        if(i<256)
        {
            tree[i].c = i;
            tree[i].pre_node = MAX_TREE_NODES - 1;
            if(i!=255)
                tree[i].next_node = i+1;
            else
                tree[i].next_node = -1;
            tree[i].son_node = -1;
        }
        else
        {
            tree[i].c = '\0';
            tree[i].pre_node = MAX_TREE_NODES - 1;
            tree[i].next_node = -1;
            tree[i].next_node = -1;
        }
    }
    current_code = 256;
}

unsigned int read_code(FILE*, unsigned int); 
void write_code(FILE*, unsigned int, unsigned int); 
void writefileheader(FILE *,char**,int);
void readfileheader(FILE *,char**,int *);
void compress(FILE*, FILE*, Tree);
void decompress(FILE*, FILE*, Tree);
/*
struct TextChar
{
    char c;
    struct TextChar * pre_char;
};*/
struct TextChar
{
    char c;
    struct TextChar * pre_char;
    struct TextChar * next_char;
};
/*
struct TextChar * append(struct TextChar * P, char C)
{
    struct TextChar * newC = (struct TextChar*)malloc(sizeof(struct TextChar));
    newC -> c = C;
    newC -> pre_char = P;
    return newC;
}
*/
struct TextChar * append(struct TextChar * P, char C)
{
    struct TextChar * newC = (struct TextChar*)malloc(sizeof(struct TextChar));
    newC -> c = C;
    newC -> pre_char = P;
    newC -> next_char = NULL;
    if(P != NULL)
        P -> next_char = newC;
    return newC;
}

int isEmptyNode( unsigned int i, Tree tree)
{
    if(i != 0 && tree[i].c == '\0' && tree[i].pre_node == MAX_TREE_NODES - 1)
        return 1;
    return 0;
}

int isFound(Tree tree, unsigned int code, struct TextChar * T)
{
    struct TreeNode node = tree[code];
    
    while(1)
    {
        if(node.c != T->c)
            return 0;
        if(node.pre_node == MAX_TREE_NODES - 1 && T->pre_char == NULL)
            return 1;
        else
            if(node.pre_node == MAX_TREE_NODES - 1 || T->pre_char == NULL)
                return 0;
        node = tree[node.pre_node];
        T = T -> pre_char;
    }
    return 0;
}

struct TextChar * firstChar(struct TextChar * P)
{
    if(P == NULL)
        return NULL;
    if(P -> pre_char == NULL)
        return P;
    return firstChar(P -> pre_char);
}

/*
int findCode(struct TextChar * T, Tree tree)
{
    for(int i = 0; i<MAX_TREE_NODES-1; i++)
    {
        if(isEmptyNode(i, tree))
            return -1;
        if(isFound(tree, i, T))
            return i;
    }
    return -1;
}
*/
int findCode(struct TextChar * T, Tree tree)
{
    int pos = 0;
    struct TextChar * first = firstChar(T);
    while(1)
    {
        if(tree[pos].c != first->c)
        {
            if(tree[pos].next_node != -1)
                pos = tree[pos].next_node;
            else
                return -1;
        }
        else if(first == T)
            return pos;
        else if(first->next_char !=NULL && tree[pos].son_node != -1)
        {
            first = first->next_char;
            pos = tree[pos].son_node;
        }
        else if(first->next_char == NULL)
            return pos;
        else
            return -1;
       
    }
    return -1;
}

struct TextChar * findStr(unsigned int code, Tree tree)
{
    if(isEmptyNode(code, tree))
        return NULL;
    struct TextChar * previous = NULL;
    if(tree[code].pre_node != MAX_TREE_NODES - 1)
        previous = findStr(tree[code].pre_node, tree);
    return append(previous, tree[code].c);
}
/*
int addNode(struct TextChar * P, char C, Tree tree)
{
    for (unsigned int i = 0; i < MAX_TREE_NODES-1; i++)
    {
        if (isEmptyNode(i, tree)==1)
        {
            tree[i].c = C;
            int pre_code = findCode(P, tree);
            if (pre_code == -1)
                pre_code = addNode(P->pre_char, P->c, tree);
            tree[i].pre_node = pre_code;
            return i;
        }
    }
    
    initTree(tree);
    return addNode(P, C, tree);
}
*/
int addNode(struct TextChar * P, char C, Tree tree)
{
    
    if(current_code <MAX_TREE_NODES -1)
    {
        tree[current_code].c = C;
        struct TextChar * f = firstChar(P);
        int pre_code = findCode(f, tree);
        if (pre_code == -1)
            pre_code = addNode(P->pre_char, P->c, tree);
        tree[current_code].pre_node = pre_code;
        if(tree[pre_code].son_node == -1)
            tree[pre_code].son_node = current_code;
        else
        {
            int x = tree[tree[pre_code].son_node].next_node;
            while( x!= -1)
                x = tree[x].next_node;
            tree[x].next_node = current_code;
        }
        tree[current_code].next_node = -1;
        tree[current_code].son_node = -1;
        current_code ++;
        return current_code;
        
    }
    else
    {
        initTree(tree);
        return addNode(P, C, tree);
    }
}


void outputStr(struct TextChar * P, FILE * output)
{
    if(P == NULL)
        return;
    if(P -> pre_char != NULL)
        outputStr(P -> pre_char, output);
    fprintf(output, "%c", P -> c);
}

void freeText(struct TextChar * P)
{
    if(P->pre_char != NULL)
        freeText(P->pre_char);
    free(P);
}

struct TextChar * newChar(char c)
{
    struct TextChar * new = (struct TextChar*)malloc(sizeof(struct TextChar));
    new->c = c;
    new->pre_char = NULL;
    return new;
}

int main(int argc, char **argv)
{

    int printusage = 0;
    int	no_of_file;
    char **input_file_names;    
	char *output_file_names;
    FILE *lzw_file;

    Tree tree;
    initTree(tree);
    
    if (argc >= 3)
    {
		if ( strcmp(argv[1],"-c") == 0)
		{		
			/* compression */
			lzw_file = fopen(argv[2] ,"wb");
        
			/* write the file header */
			input_file_names = argv + 3;
			no_of_file = argc - 3;
			writefileheader(lzw_file,input_file_names,no_of_file);
        	        	
			/* ADD CODES HERE */
            for(int i = 0; i<no_of_file; i++)
            {
                FILE * input = fopen(input_file_names[i], "r");
                compress(input, lzw_file, tree);
                fclose(input);
            }
            write_code(lzw_file, MAX_TREE_NODES-1, CODE_SIZE);
        	
			fclose(lzw_file);        	
		}else
		if ( strcmp(argv[1],"-d") == 0)
		{

			/* decompress */
			lzw_file = fopen(argv[2] ,"rb");
			
			/* read the file header */
			no_of_file = 0;			
			readfileheader(lzw_file,&output_file_names,&no_of_file);
			
			/* ADD CODES HERE */
            
            int file_num = 0;
            char * name;
            name = strtok(output_file_names,"\n");
            
            while (name!=NULL)
            {
                if(strlen(name)!=0)
                {
                    FILE * output = fopen(name, "w");
                    decompress(lzw_file, output, tree);
                    fclose(output);
                }
                file_num++;
                
                if(file_num>=no_of_file)
                    break;
                name = strtok(NULL, "\n");
            }
			
			fclose(lzw_file);		
			free(output_file_names);
		}else
			printusage = 1;
    }else
		printusage = 1;

	if (printusage)
		printf("Usage: %s -<c/d> <lzw filename> <list of files>\n",argv[0]);
 	
	return 0;
}

/*****************************************************************
 *
 * writefileheader() -  write the lzw file header to support multiple files
 *
 ****************************************************************/
void writefileheader(FILE *lzw_file,char** input_file_names,int no_of_files)
{
	int i;
	/* write the file header */
	for ( i = 0 ; i < no_of_files; i++) 
	{
		fprintf(lzw_file,"%s\n",input_file_names[i]);	
			
	}
	fputc('\n',lzw_file);

}

/*****************************************************************
 *
 * readfileheader() - read the fileheader from the lzw file
 *
 ****************************************************************/
void readfileheader(FILE *lzw_file,char** output_filenames,int * no_of_files)
{
	int noofchar;
	char c,lastc;

	noofchar = 0;
	lastc = 0;
	*no_of_files=0;
	/* find where is the end of double newline */
	while((c = fgetc(lzw_file)) != EOF)
	{
		noofchar++;
		if (c =='\n')
		{
			if (lastc == c )
				/* found double newline */
				break;
			(*no_of_files)++;
		}
		lastc = c;
	}

	if (c == EOF)
	{
		/* problem .... file may have corrupted*/
		*no_of_files = 0;
		return;
	
	}
	/* allocate memeory for the filenames */
	*output_filenames = (char *) malloc(sizeof(char)*noofchar);
	/* roll back to start */
	fseek(lzw_file,0,SEEK_SET);

	fread((*output_filenames),1,(size_t)noofchar,lzw_file);
	
	return;
}

/*****************************************************************
 *
 * read_code() - reads a specific-size code from the code file
 *
 ****************************************************************/
unsigned int read_code(FILE *input, unsigned int code_size)
{
    unsigned int return_value;
    static int input_bit_count = 0;
    static unsigned int input_bit_buffer = 0L;

    /* The code file is treated as an input bit-stream. Each     */
    /*   character read is stored in input_bit_buffer, which     */
    /*   is 32-bit wide.                                         */

    /* input_bit_count stores the no. of bits left in the buffer */

    while (input_bit_count <= 24) {
        input_bit_buffer |= (unsigned long) getc(input) << (24-input_bit_count);
        input_bit_count += 8;
    }
    
    return_value = input_bit_buffer >> (32 - code_size);
    input_bit_buffer <<= code_size;
    input_bit_count -= code_size;
    
    return(return_value);
}


/*****************************************************************
 *
 * write_code() - write a code (of specific length) to the file 
 *
 ****************************************************************/
void write_code(FILE *output, unsigned int code, unsigned int code_size)
{
    static int output_bit_count = 0;
    static unsigned long output_bit_buffer = 0L;

    /* Each output code is first stored in output_bit_buffer,    */
    /*   which is 32-bit wide. Content in output_bit_buffer is   */
    /*   written to the output file in bytes.                    */

    /* output_bit_count stores the no. of bits left              */    

    output_bit_buffer |= (unsigned long) code << (32-code_size-output_bit_count);
    output_bit_count += code_size;

    while (output_bit_count >= 8) {
        putc(output_bit_buffer >> 24, output);
        output_bit_buffer <<= 8;
        output_bit_count -= 8;
    }


    /* only < 8 bits left in the buffer                          */    

}

/*****************************************************************
 *
 * compress() - compress the source file and output the coded text
 *
 ****************************************************************/
void compress(FILE *input, FILE *output, Tree tree)
{

	/* ADD CODES HERE */
    char C;
    struct TextChar * P = NULL;
    unsigned int X = 0;
    
    while((C = fgetc(input))!=EOF)
    {
        struct TextChar * PC = append(P, C);
        unsigned int code = findCode(PC, tree);
        free(PC);
        
        if(code == -1)
        {
            unsigned int P_code = findCode(P, tree);
            write_code(output, P_code, CODE_SIZE);
            addNode(P, C, tree);
            X = (unsigned int)C;
            freeText(P);
            P = newChar(C);
        }
        else
        {
            X = code;
            P = append(P, C);
        }
    }
    freeText(P);
    if(X!=0)
        write_code(output, X, CODE_SIZE);
    write_code(output, MAX_TREE_NODES-1, CODE_SIZE);
}


/*****************************************************************
 *
 * decompress() - decompress a compressed file to the orig. file
 *
 ****************************************************************/
void decompress(FILE *input, FILE *output, Tree tree)
{	

	 /*ADD CODES HERE */

    unsigned int PW =0;
    PW = read_code(input, CODE_SIZE);
    struct TextChar * S = NULL;
    S = findStr(PW, tree);
    outputStr(S, output);
    
    unsigned int CW = 0;
    char C;

    while((CW = read_code(input, CODE_SIZE))!= MAX_TREE_NODES -1)
    {
        freeText(S);
        
        S = findStr(CW, tree);
        
        
        if(S != NULL)
        {
            outputStr(S, output);
            
            struct TextChar * first_char = firstChar(S);
            C = first_char -> c;

            freeText(S);
            S = findStr(PW, tree);
            addNode(S, C, tree);
        }
        else
        {
            S = findStr(PW, tree);
            struct TextChar * first_char = firstChar(S);
            C = first_char -> c;
            outputStr(S, output);
            fprintf(output, "%c", C);
            addNode(S, C, tree);
        }

        PW = CW;
    }
}

