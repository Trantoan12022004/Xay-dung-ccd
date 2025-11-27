/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // Bỏ qua các ký tự trắng(space, tab, newline)
  while (charCodes[currentChar] == CHAR_SPACE) {
    readChar();
  }
}

void skipComment() {
  // Comment bắt đầu bằng (* và kết thúc bằng *)
  int ln = lineNo;
  int cn = colNo;
  
  readChar(); // Đọc ký tự sau (
  readChar(); // Đọc ký tự sau *
  
  while (1) {
    if (currentChar == EOF) {
      error(ERR_ENDOFCOMMENT, ln, cn);
      break;
    }
    
    if (currentChar == '*') {
      readChar();
      if (currentChar == ')') {
        readChar(); // Đọc ký tự sau )
        break;
      }
    } else {
      readChar();
    }
  }
}

Token* readIdentKeyword(void) {
  // Đọc identifier hoặc keyword
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int count = 0;
  
  // Đọc các ký tự chữ cái, chữ số và gạch dưới
  while ((currentChar != EOF) && 
         ((charCodes[currentChar] == CHAR_LETTER) || 
          (charCodes[currentChar] == CHAR_DIGIT) ||
          (charCodes[currentChar] == CHAR_UNDERSCORE))) {  // THÊM UNDERSCORE
    
    if (count >= MAX_IDENT_LEN) {
      // Nếu vượt quá 10 ký tự, bỏ qua các ký tự thừa (KHÔNG báo lỗi)
      readChar();
      continue;  // Tiếp tục đọc nhưng không lưu
    }
    
    token->string[count++] = (char)currentChar;
    readChar();
  }
  
  token->string[count] = '\0';
  
  // Kiểm tra xem có phải keyword không
  TokenType kwType = checkKeyword(token->string);
  if (kwType != TK_NONE) {
    token->tokenType = kwType;
  }
  
  return token;
}

Token* readNumber(void) {
    // Đọc số nguyên
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int count = 0;
  
  // Đọc các chữ số
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT)) {
    token->string[count++] = (char)currentChar;
    readChar();
  }
  
  token->string[count] = '\0';
  token->value = atoi(token->string);
  
  return token;
}

Token* readConstChar(void) {
  // Đọc hằng ký tự 'x'
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  
  readChar(); // Đọc ký tự sau dấu '
  
  if (currentChar == EOF) {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  
  token->string[0] = (char)currentChar;
  token->string[1] = '\0';
  
  readChar(); // Đọc ký tự tiếp theo
  
  // Kiểm tra dấu ' đóng
  if (currentChar != '\'') {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  } else {
    readChar(); // Đọc ký tự sau dấu '
  }
  
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: 
  case CHAR_UNDERSCORE:  // THÊM DÒNG NÀY: underscore cũng bắt đầu identifier
  return readIdentKeyword();

  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=') {  // Kiểm tra +=
      token = makeToken(SB_PLUS_ASSIGN, ln, cn);
      readChar();
    } else {
      token = makeToken(SB_PLUS, ln, cn);
    }
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
    
  case CHAR_TIMES:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=') {  // Kiểm tra *=
      token = makeToken(SB_TIMES_ASSIGN, ln, cn);
      readChar();
    } else {
      token = makeToken(SB_TIMES, ln, cn);
    }
    return token;
    
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
    
  case CHAR_LT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_LE, ln, cn);
      readChar();
    } else {
      token = makeToken(SB_LT, ln, cn);
    }
    return token;
    
  case CHAR_GT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_GE, ln, cn);
      readChar();
    } 
    else if (currentChar == '>') {  // THÊM DÒNG NÀY: Xử lý <>
      token = makeToken(SB_NEQ, ln, cn);
      readChar();
    }
    else {
      token = makeToken(SB_GT, ln, cn);
    }
    return token;
    
  case CHAR_EXCLAIMATION:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_NEQ, ln, cn);
      readChar();
    } else {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
    }
    return token;
    
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
    
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
    
  case CHAR_PERIOD:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == ')') {
      token = makeToken(SB_RSEL, ln, cn);
      readChar();
    } else {
      token = makeToken(SB_PERIOD, ln, cn);
    }
    return token;
    
  case CHAR_COLON:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_ASSIGN, ln, cn);
      readChar();
    } else {
      token = makeToken(SB_COLON, ln, cn);
    }
    return token;
    
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
    
  case CHAR_SINGLEQUOTE:
    return readConstChar();
    
  case CHAR_LPAR:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == '*') {
      skipComment();
      return getToken();
    } else if (currentChar == '.') {
      token = makeToken(SB_LSEL, ln, cn);
      readChar();
      return token;
    } else {
      token = makeToken(SB_LPAR, ln, cn);
      return token;
    }
    
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;

  case CHAR_LSEL:
    token = makeToken(SB_LSEL, lineNo, colNo);
    readChar();
    return token;
    
  case CHAR_RSEL:
    token = makeToken(SB_RSEL, lineNo, colNo);
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  case KW_RETURN: printf("KW_RETURN\n"); break;
  case KW_SWITCH: printf("KW_SWITCH\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  case SB_PLUS_ASSIGN: printf("SB_PLUS_ASSIGN\n"); break;   // THÊM
  case SB_TIMES_ASSIGN: printf("SB_TIMES_ASSIGN\n"); break; // THÊM
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



