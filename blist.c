#include <stdio.h>
#include <stdlib.h>



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


void translate(FILE *f)
{
    for(;;)
    {
        int x;
        //offset to next line
        x = fgetc(f) | (fgetc(f) << 8);
        if (feof(f))
        {
            fprintf(stderr, "Unexpected eof.\n");
            break;
        }
        
        // last line.
        if (x == 0) break;
    
        // line number.
        x = fgetc(f) | (fgetc(f) << 8);
        if (feof(f))
        {
            fprintf(stderr, "Unexpected eof.\n");
            break;        
        }
        
        fprintf(stdout, "% 5d: ", x); 
    
        for (;;)
        {
            x  = fgetc(f);
            if (x == 0) break;
            
            if (x < 0)
            {
                fprintf(stderr, "Unexpected eof.\n");
                break;
            }   
            if (x > 0x7f)
                fputs(tokens[x & 0x7f], stdout);
            else if (x > 0x1f) fputc(x, stdout);         
        
        }
    
        fprintf(stdout, "\n");
    }


}


int main(int argc, char **argv)
{
    translate(stdin);
    return 0;
}