
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define MAX_NUM_LAYOUTS 16
#define INVALID_CHAR    '_'

/* index/position of non-space characters must be identical for same keys!
 * spaces are ignored. Use '_' (= INVALID_CHAR) for non-ascii characters!
 * see https://docs.microsoft.com/en-us/globalization/windows-keyboard-layouts
 */
/*                       -              -             -              -             SHIFT+          SHIFT+        SHIFT+         SHIFT+  */
/*                       1st row        2nd row       3rd row        4th row       1st row         2nd row       3rd row        4th row */
const char Lus_int[] = " _1234567890-=  qwertyuiop[]  asdfghjkl;_\\  \\zxcvbnm,./  _! @#$%_&*()_+  QWERTYUIOP{}  ASDFGHJKL: _|  |ZXCVBNM<>? "; /* kbdusx.html US-INTL */
const char Lus_en[]  = " _1234567890-=  qwertyuiop[]  asdfghjkl;'\\  \\zxcvbnm,./  ~! @#$%^&*()_+  QWERTYUIOP{}  ASDFGHJKL:\"|  |ZXCVBNM<>/ "; /* kbdus.html US-EN */
const char Luk[]     = " _1234567890-=  qwertyuiop[]  asdfghjkl;' #  \\zxcvbnm,./  ~!\"_$%^&*()_+  QWERTYUIOP{}  ASDFGHJKL: @|  |ZXCVBNM<>/ "; /* kbduk.html EN */
const char Lde[]     = " _1234567890__  qwertzuiop_+  asdfghjkl__ #   <yxcvbnm,.-  _!\"_$%&/()=?_  QWERTZUIOP_*  ASDFGHJKL_ _'  >YXCVBNM;:_ "; /* kbdgr.html */
const char Ltr[]     = " _1234567890__  qwertyuiop__  asdfghjkl_i ,   <zxcvbnm__.  _! '_+%&/()=?_  QWERTYUIOP__  ASDFGHJKL_ _;  >ZXCVBNM__: "; /* kbdtuq.html */
const char Lit[]     = "\\1234567890'_  qwertyuiop_+  asdfghjkl__ _   <zxcvbnm,.-  |!\"_$%&/()=?^  QWERTYUIOP_*  ASDFGHJKL_ __  >ZXCVBNM;:_ "; /* kbdit.html */
const char Les[]     = " _1234567890'_  qwertyuiop_+  asdfghjkl__ _   <zxcvbnm,.-  _!\"_$%&/()=?_  QWERTYUIOP_*  ASDFGHJKL_ __  >ZXCVBNM;:_ "; /* kbdsp.html */

const char * layouts[][2] =
{
  { "us-intl", Lus_int }, { "us-int", Lus_int }, { "us", Lus_int }
, { "us-en", Lus_en }
, { "uk", Luk }, { "en", Luk }
, { "de", Lde }
, { "tr", Ltr }, { "tr-q", Ltr }, { "tr-Q", Ltr }
, { "it", Lit }
, { "es", Les }, { "sp", Les }
};

const char * default_layouts[][2] =
{
  { "us-intl", Lus_int }
, { "us-en", Lus_en }
, { "de", Lde }
};


#define ARRAY_SIZE( A )   ( sizeof(A) / sizeof(A[0]) )

static void print_layouts()
{
  int L;
  for ( L = 0; L < ARRAY_SIZE(layouts); ++L )
  {
    fputs(layouts[L][0], stdout);
    if ( L == ARRAY_SIZE(layouts) -1 )
      fputc('\n', stdout);
    else
      fputc(' ', stdout);
  }
}


static void usage()
{
  fputs("Usage: ckbdpwd [-v][-h][-l][-f] <max_num> [ <layout1> [<layout2> [..]] ]\n", stderr);
  fputs("  common keyboard password:\n", stderr);
  fputs("  create a password - usable on different keyboard layouts.\n", stderr);
  fputs("  -n       do not append Line Feed '\\n'\n", stderr);
  fputs("  -v       verbose output\n", stderr);
  fputs("  -h       print usage\n", stderr);
  fputs("  -l       list available keyboard layouts\n", stderr);
  fputs("  -f       test/filter mode: expect password on stdin and test filter charachters\n", stderr);
  fputs("  max_num  maximum number of hex bytes to use\n", stderr);
  fputs("  hexcode is read from standard input (stdin). Use md5sum or shasum\n", stderr);
}


int main(int argc, char **argv)
{
  const char * sel_layouts[MAX_NUM_LAYOUTS];
  int layout_idx[MAX_NUM_LAYOUTS];
  char common_keys[256];
  int num_common = 0;
  int max_num = -1;
  int num_layouts = 0;
  int verbosity = 0;
  int appendLF = 1;
  int filterMode = 0;
  int argno, L, i, xlen = 0, v = 0, vinc = 0;

  for ( L = 0; L < MAX_NUM_LAYOUTS; ++L )
  {
    sel_layouts[L] = NULL;
    layout_idx[L] = -1;
  }

  for ( argno = 1; argno < argc; ++argno )
  {
    if ( !strcmp(argv[argno], "-v") )
      ++verbosity;
    else if ( !strcmp(argv[argno], "-vv") )
      verbosity += 2;
    else if ( !strcmp(argv[argno], "-vvv") )
      verbosity += 3;
    else if ( !strcmp(argv[argno], "-n") )
      appendLF = 0;
    else if ( !strcmp(argv[argno], "-f") )
      filterMode = 1;
    else if ( !strcmp(argv[argno], "-h") )
    {
      usage();
      return 0;
    }
    else if ( !strcmp(argv[argno], "-L") || !strcmp(argv[argno], "-l") )
    {
      print_layouts();
      return 0;
    }
    else
    {
      int is_layout = 0;
      /* test for layout */
      for ( L = 0; L < ARRAY_SIZE(layouts); ++L )
      {
        if ( !strcmp( argv[argno], layouts[L][0] ) )
        {
          sel_layouts[num_layouts++] = layouts[L][1];
          if (verbosity)
            fprintf(stderr, "adding layout for %s for idx %d\n", layouts[L][0], num_layouts-1);
          is_layout = 1;
          break;
        }
      }
      /* test for <maxnum> */
      if ( !is_layout )
      {
        int m = atoi(argv[argno]);
        if ( m )
          max_num = m;
        else
          fprintf(stderr, "warning: ignoring unknown command line option '%s'\n", argv[argno]);
      }

    }
  }

  if ( !num_layouts )
  {
    fprintf(stderr, "using layouts: ");
    for ( L = 0; L < ARRAY_SIZE(default_layouts); ++L )
      fprintf(stderr, "'%s'%s", default_layouts[L][0], ( (L == ARRAY_SIZE(default_layouts) -1) ? "\n" : ", " ) );

    for ( L = 0; L < ARRAY_SIZE(default_layouts); ++L )
    {
      sel_layouts[num_layouts++] = default_layouts[L][1];
      if (verbosity)
        fprintf(stderr, "adding layout for %s for idx %d\n", default_layouts[L][0], num_layouts-1);
    }
  }
  else if ( 1 == num_layouts )
    fputs("warning: using only one single layout!\n", stderr);

  if ( argc <= 1 )
    fputs("expecting hexcode on stdin (Ctrl-D to finish, use option '-h' for usage) ..\n", stderr);

  /* parse keyboard layout strings - for same characters */
  while (1)
  {
    int num_same = 1;
    int at_end = 0;

    /* next real key per layout */
    for ( L = 0; L < num_layouts; ++L )
    {
      /* to next position */
      ++layout_idx[L];
      /* skip spaces */
      while ( ' ' == sel_layouts[L][ layout_idx[L] ] )
        ++layout_idx[L];
      /* check end of string */
      if ( 0 == sel_layouts[L][ layout_idx[L] ] )
      {
        at_end = 1;
        break;
      }
    }
    if ( at_end )
      break;

    /* count identical characters over the layouts */
    for ( L = 1; L < num_layouts; ++L )
    {
      if ( sel_layouts[0][ layout_idx[0] ] == sel_layouts[L][ layout_idx[L] ] )
        ++num_same;
    }

    if ( sel_layouts[0][ layout_idx[0] ] != INVALID_CHAR )
    {
      if ( num_same == num_layouts )
      {
        /* common character in all layouts */
        for ( i = 0; i < num_common; ++i )
        {
          if ( common_keys[i] == sel_layouts[0][ layout_idx[0] ] )
            break;
        }
        if ( i >= num_common )
        {
          /* if not already in common_keys[] */
          common_keys[num_common++] = sel_layouts[0][ layout_idx[0] ];
          if ( verbosity >= 3 )
          {
            fputs("matching key: ", stderr);
            fputc( sel_layouts[0][ layout_idx[0] ], stderr);
            fputc( '\n', stderr);
          }
        }
      }
      else if ( verbosity >= 2 )
      {
        fputs("mismatching keys: ", stderr);
        for ( L = 0; L < num_layouts; ++L )
          fputc( sel_layouts[L][ layout_idx[L] ], stderr);
        fputc('\n', stderr);
      }
    }
  }

  if ( verbosity >= 1 || num_common <= 10 )
  {
    fprintf(stderr, "%d common keys: ", num_common);
    for ( i = 0; i < num_common; ++i )
      fputc(common_keys[i], stderr);
    fputc('\n', stderr);
  }

  if ( num_common <= 10 )
  {
    fputs("Error: too few common characters/keys over layouts!\n", stderr);
    return 10;
  }

  if ( !filterMode )
  {
    /* common keys in layouts determined */
    while ( max_num < 0 || (xlen / 2) < max_num )
    {
      int c = getc(stdin);
      if ( c == EOF )
        break;
      if ( isspace(c) || isblank(c) )
        continue;
      if ( !isxdigit(c) )
        break;

      v = v << 4;
      if ( c >= '0' && c <= '9' )
        v = v | ( c - '0' );
      else if ( c >= 'A' && c <= 'F' )
        v = v | ( 10 + c - 'A' );
      else if ( c >= 'a' && c <= 'f' )
        v = v | ( 10 + c - 'a' );
      ++xlen;
      if ( !(xlen & 1) )
      {
        /* new hex value v */
        if (verbosity)
          fprintf(stderr, "read hex: 0x%02X = dec %d\n", v, v);

        v += vinc;
        vinc = v / num_common;  /* do not just throw away big numbers */
        v = v % num_common;
        fputc( common_keys[v], stdout );

        /* re-initialize v for next value */
        v = 0;
      }
    }
  }
  else
  {
    while ( max_num < 0 || (xlen / 2) < max_num )
    {
      int c = getc(stdin);
      if ( c == EOF )
        break;

      for ( i = 0; i < num_common; ++i )
      {
        if ( c == common_keys[i] )
        {
          fputc(c, stdout);
          ++xlen;
          break;
        }
      }
      if ( i >= num_common )
        fprintf(stderr, "character '%c' not in common keys!\n", c);
    }
  }

  if (xlen && appendLF )
    fputc('\n', stdout);

  return 0;
}

