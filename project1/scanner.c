/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank()
{
  // TODO
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

void skipComment()
{
  int closed = 0;
  // TODO
  while (currentChar != EOF && closed < 2)
  {
    if (charCodes[currentChar] == CHAR_TIMES)
      closed = 1;
    else if (charCodes[currentChar] == CHAR_RPAR)
      closed = closed == 1 ? 2 : 0;
    else
      closed = 0;
    readChar();
  }
  if (closed != 2)
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  return;
}

Token *readIdentKeyword(void)
{
  // TODO
  char *word = (char *)malloc(sizeof(char) * (MAX_IDENT_LEN + 1));
  strcpy(word, "");

  int preColNo = colNo,
      preLineNo = lineNo;

  while (currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT))
  {
    word = strncat(word, &currentChar, 1);
    readChar();
  }
  if (strlen(word) > MAX_IDENT_LEN)
  {
    error(ERR_IDENTTOOLONG, preLineNo, preColNo);
    return makeToken(TK_NONE, preLineNo, preColNo);
  }

  TokenType tokenType = checkKeyword(word);

  if (tokenType == TK_NONE)
    tokenType = TK_IDENT;

  Token *token = makeToken(tokenType, preLineNo, preColNo);
  strcpy(token->string, word);
  return token;
}

Token *readNumber(void)
{
  // TODO
  char *number = (char *)malloc(sizeof(char) * 16);
  strcpy(number, "");

  int preColNo = colNo,
      preLineNo = lineNo;

  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT)
  {
    number = strncat(number, &currentChar, 1);
    readChar();
  }
  if (atoi(number) < 0)
  {
    error(ERR_NUMBERTOOLONG, preLineNo, preColNo);
    return makeToken(TK_NONE, preLineNo, preColNo);
  }

  Token *token = makeToken(TK_NUMBER, preLineNo, preColNo);
  strcpy(token->string, number);
  return token;
}

Token *readConstChar(void)
{
  // TODO
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar();
  if (currentChar == EOF)
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  token->string[0] = currentChar;
  token->string[1] = '\0';

  readChar();
  if (currentChar == EOF)
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
  {
    readChar();
    return token;
  }
  else
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  return token;
}

Token *getToken(void)
{
  Token *token;
  int ln, cn;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT:
    return readNumber();
  case CHAR_PLUS:
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;
    // ....
    // TODO
    // ....
  case CHAR_LPAR:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (charCodes[currentChar] == CHAR_TIMES)
    {
      readChar();
      skipComment();
      return getToken();
    }
    else if (charCodes[currentChar] == CHAR_PERIOD)
    {
      token = makeToken(SB_LSEL, ln, cn);
      readChar();
      return token;
    }
    else
    {
      token = makeToken(SB_LPAR, ln, cn);
      return token;
    }
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EXCLAIMATION:
    ln = lineNo;
    cn = colNo;
    if (charCodes[readChar()] != CHAR_EQ)
    {

      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      return token;
    }
    else
    {
      token = makeToken(SB_NEQ, ln, cn);
      readChar();
      return token;
    }
  case CHAR_COLON:
    ln = lineNo;
    cn = colNo;
    if (charCodes[readChar()] != CHAR_EQ)
    {
      token = makeToken(SB_COLON, ln, cn);
      return token;
    }
    else
    {
      token = makeToken(SB_ASSIGN, ln, cn);
      readChar();
      return token;
    }
  case CHAR_PERIOD:
    ln = lineNo;
    cn = colNo;
    if (charCodes[readChar()] != CHAR_RPAR)
    {
      token = makeToken(SB_PERIOD, ln, cn);
      return token;
    }
    else
    {
      token = makeToken(SB_RSEL, ln, cn);
      readChar();
      return token;
    }
  case CHAR_LT:
    ln = lineNo;
    cn = colNo;
    if (charCodes[readChar()] != CHAR_EQ)
      token = makeToken(SB_LT, ln, cn);
    else
    {
      token = makeToken(SB_LE, ln, cn);
      readChar();
    }
    return token;
  case CHAR_GT:
    ln = lineNo;
    cn = colNo;
    if (charCodes[readChar()] != CHAR_EQ)
      token = makeToken(SB_GT, ln, cn);
    else
    {
      token = makeToken(SB_GE, ln, cn);
      readChar();
    }
    return token;
  case CHAR_SINGLEQUOTE:
    return readConstChar();
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}

/******************************************************************/

void printToken(Token *token)
{

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
  case TK_NONE:
    printf("TK_NONE\n");
    break;
  case TK_IDENT:
    printf("TK_IDENT(%s)\n", token->string);
    break;
  case TK_NUMBER:
    printf("TK_NUMBER(%s)\n", token->string);
    break;
  case TK_CHAR:
    printf("TK_CHAR(\'%s\')\n", token->string);
    break;
  case TK_EOF:
    printf("TK_EOF\n");
    break;

  case KW_PROGRAM:
    printf("KW_PROGRAM\n");
    break;
  case KW_CONST:
    printf("KW_CONST\n");
    break;
  case KW_TYPE:
    printf("KW_TYPE\n");
    break;
  case KW_VAR:
    printf("KW_VAR\n");
    break;
  case KW_INTEGER:
    printf("KW_INTEGER\n");
    break;
  case KW_CHAR:
    printf("KW_CHAR\n");
    break;
  case KW_ARRAY:
    printf("KW_ARRAY\n");
    break;
  case KW_OF:
    printf("KW_OF\n");
    break;
  case KW_FUNCTION:
    printf("KW_FUNCTION\n");
    break;
  case KW_PROCEDURE:
    printf("KW_PROCEDURE\n");
    break;
  case KW_BEGIN:
    printf("KW_BEGIN\n");
    break;
  case KW_END:
    printf("KW_END\n");
    break;
  case KW_CALL:
    printf("KW_CALL\n");
    break;
  case KW_IF:
    printf("KW_IF\n");
    break;
  case KW_THEN:
    printf("KW_THEN\n");
    break;
  case KW_ELSE:
    printf("KW_ELSE\n");
    break;
  case KW_WHILE:
    printf("KW_WHILE\n");
    break;
  case KW_DO:
    printf("KW_DO\n");
    break;
  case KW_FOR:
    printf("KW_FOR\n");
    break;
  case KW_TO:
    printf("KW_TO\n");
    break;

  case SB_SEMICOLON:
    printf("SB_SEMICOLON\n");
    break;
  case SB_COLON:
    printf("SB_COLON\n");
    break;
  case SB_PERIOD:
    printf("SB_PERIOD\n");
    break;
  case SB_COMMA:
    printf("SB_COMMA\n");
    break;
  case SB_ASSIGN:
    printf("SB_ASSIGN\n");
    break;
  case SB_EQ:
    printf("SB_EQ\n");
    break;
  case SB_NEQ:
    printf("SB_NEQ\n");
    break;
  case SB_LT:
    printf("SB_LT\n");
    break;
  case SB_LE:
    printf("SB_LE\n");
    break;
  case SB_GT:
    printf("SB_GT\n");
    break;
  case SB_GE:
    printf("SB_GE\n");
    break;
  case SB_PLUS:
    printf("SB_PLUS\n");
    break;
  case SB_MINUS:
    printf("SB_MINUS\n");
    break;
  case SB_TIMES:
    printf("SB_TIMES\n");
    break;
  case SB_SLASH:
    printf("SB_SLASH\n");
    break;
  case SB_LPAR:
    printf("SB_LPAR\n");
    break;
  case SB_RPAR:
    printf("SB_RPAR\n");
    break;
  case SB_LSEL:
    printf("SB_LSEL\n");
    break;
  case SB_RSEL:
    printf("SB_RSEL\n");
    break;
  }
}

int scan(char *fileName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

main()
{

  if (scan("./test/example1.kpl") == IO_ERROR)
    printf("IO_ERROR\n");

  // return -1;
  getch();
}

// return 0;
