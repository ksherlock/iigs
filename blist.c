#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

const char *tokens[]={
/*80*/	" END ",
	" FOR ",
	" NEXT ",
	" DATA ",
	" INPUT ",
	" DEL ",
	" DIM ",
	" READ ",
	" GR ",
	" TEXT ",
	" PR# ",
	" IN# ",
	" CALL ",
	" PLOT ",
	" HLIN ",
	" VLIN ",
	" HGR2 ",
	" HGR ",
	" HCOLOR= ",
	" HPLOT ",
	" DRAW ",
	" XDRAW ",
	" HTAB ",
	" HOME ",
	" ROT= ",
	" SCALE= ",
	" SHLOAD ",
	" TRACE ",
	" NOTRACE ",
	" NORMAL ",
	" INVERSE ",
	" FLASH ",
	" COLOR= ",
	" POP ",
	" VTAB ",
	" HIMEM: ",
	" LOMEM: ",
	" ONERR ",
	" RESUME ",
	" RECALL ",
	" STORE ",
	" SPEED= ",
	" LET ",
	" GOTO ",
	" RUN ",
	" IF ",
	" RESTORE ",
	" & ",
/*B0*/	" GOSUB ",
	" RETURN ",
	" REM ",
	" STOP ",
	" ON ",
	" WAIT ",
	" LIST ",
	" SAVE ",
	" DEF ",
	" POKE ",	
	" PRINT ",
	" CONT ",
	" LIST ",
	" CLEAR ",
	" GET ",
	" NEW ",
	" TAB( ",
	" TO ",
	" FN ",
	" SPC( ",
	" THEN ",
	" AT ",
	" NOT ",
	" STEP ",
	" + ",
	" - ",
	" * ",
	" / ",
	" ^ ",
	" AND ",
	" OR ",
	" > ",
	" = ",
	" < ",
	" SGN ",
	" INT ",
	" ABS ",
	" USR ",
	" FRE ",
	" SCRN( ",
	" PDL ",
	" POS ",
	" SQR ",
	" RND ",
	" LOG ",
	" EXP ",
	" COS ",
	" SIN ",
	" TAN ",
	" ATN ",
	" PEEK ",
	" LEN ",
	" STR$ ",
	" VAL ",
	" ASC ",
	" CHR$ ",
	" LEFT$ ",
	" RIGHT$ ",
	" MID$ ",
/*EB*/	" ?? ",	/* I'm pretty sure these aren't valid ... */
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
	" ?? ",
};       

/*

  This is where the actual work is done.
  An Applesoft file is laid out like so:

  word word [variable # of bytes] <0x00> <-eol marker
  |	|	|
  |	|	|if >0x7F, it is a token, otherwise, it's a printed
  |	|
  |	|The line # (65535 max)
  |
  | This is an offset to the next line (?) if 0, the file is done.

  repeating over and over...

*/


int translate(FILE *f, FILE *out)
{
    for(;;)
    {
        int x;
        //offset to next line
        x = fgetc(f) | (fgetc(f) << 8);
        if (feof(f))
        {
            fprintf(stderr, "Unexpected eof.\n");
            return EX_DATAERR;
        }
        
        // last line.
        if (x == 0) break;
    
        // line number.
        x = fgetc(f) | (fgetc(f) << 8);
        if (feof(f))
        {
            fprintf(stderr, "Unexpected eof.\n");
            return EX_DATAERR;
        }
        
        fprintf(out, "% 5d: ", x); 
    
        for (;;)
        {
            x = fgetc(f);
            if (x == 0) break;
            
            if (x < 0)
            {
                fprintf(stderr, "Unexpected eof.\n");
                return EX_DATAERR;
            }   
            if (x > 0x7f)
                fputs(tokens[x & 0x7f], out);
            else if (x > 0x1f) fputc(x, out);         
        
        }
    
        fprintf(out, "\n");
    }

    return 0;
}


void usage(void)
{
   fputs("Usage: blist [-o output file] input file\n",stdout);
}

int main(int argc, char **argv)
{
    int c, rv;
    const char *outfile = NULL;
    FILE *input = stdin;
    FILE *output = stdout;

    while ((c = getopt(argc, argv, "ho:")) != -1)
    {
        switch(c)
        {
        case 'o': 
            outfile = optarg;
            break;
        case 'h':
            usage();
            return 0;
        default:
            usage();
            return EX_USAGE;
        }
    }

    argc -= optind;
    argv += optind;


    switch (argc)
    {
    case 0:
        break;
    case 1:
        input = fopen(argv[0], "r");
        if (!input) 
        {
            perror(argv[0]);
            return EX_NOINPUT;
        }
        break;
    default:
        usage();
        return EX_USAGE;
    }

    if (outfile && strcmp(outfile, "-")) {
        output = fopen(outfile, "w");
        if (!output)
        {
            perror(outfile);
            if (input != stdin) fclose(input);
            return EX_CANTCREAT;
        }
    }

    rv = translate(input, output);

    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    return rv;
}

