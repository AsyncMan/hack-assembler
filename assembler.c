#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define i16 int16_t
#define u16 uint16_t

typedef struct {
  int value;
  char *symbol;

} symbol_value;

typedef struct {
  int size;
  symbol_value *symbol;
} symbol_table;

typedef struct {
  int binaryValue;
  char *instruction;
} instruction_table;

typedef enum { DEST, OPS, JUMP } c_part;

int skip_space(int letterAt, char *line) {
  while (line[letterAt] == ' ') {
    letterAt++;
  }
  return letterAt;
}

char *number_to_binary_char(u16 number) {
  char *binary = malloc(16 * sizeof(char));

  for (int i = 0; i <= 15; i++) {
    binary[i] = ((number >> (15 - i)) & 1) ? '1' : '0';
  }
  binary[16] = '\0';

  return binary;
}

bool strcontains(char *string, char character) {
  int length = strnlen(string, 10);

  for (int i = 0; i < length; i++) {
    if (string[i] == '=') {
      return (true);
    }
  }

  return (false);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return (-1);
  }
  FILE *sourceCode = fopen(argv[1], "r");
  FILE *objFile = fopen("./a.obj", "w");

  if (sourceCode == NULL) {
    return (-1);
  }

  if (objFile == NULL) {
    return (-1);
  }

  char line[255];
  int lineNumber = 0;


  symbol_table table = {.symbol = malloc(sizeof(symbol_value) * 23),
                        .size = 23};

  table.symbol[0].symbol = "R0";
  table.symbol[0].value = 0;

  table.symbol[1].symbol = "R1";
  table.symbol[1].value = 1;

  table.symbol[2].symbol = "R2";
  table.symbol[2].value = 2;

  table.symbol[3].symbol = "R3";
  table.symbol[3].value = 3;

  table.symbol[4].symbol = "R4";
  table.symbol[4].value = 4;

  table.symbol[5].symbol = "R5";
  table.symbol[5].value = 5;

  table.symbol[6].symbol = "R6";
  table.symbol[6].value = 6;

  table.symbol[7].symbol = "R7";
  table.symbol[7].value = 7;

  table.symbol[8].symbol = "R8";
  table.symbol[8].value = 8;

  table.symbol[9].symbol = "R9";
  table.symbol[9].value = 9;

  table.symbol[10].symbol = "R10";
  table.symbol[10].value = 10;

  table.symbol[11].symbol = "R11";
  table.symbol[11].value = 11;

  table.symbol[12].symbol = "R12";
  table.symbol[12].value = 12;

  table.symbol[13].symbol = "R13";
  table.symbol[13].value = 13;

  table.symbol[14].symbol = "R14";
  table.symbol[14].value = 14;

  table.symbol[15].symbol = "R15";
  table.symbol[15].value = 15;

  table.symbol[16].symbol = "SCREEN";
  table.symbol[16].value = 16384;

  table.symbol[17].symbol = "KBD";
  table.symbol[17].value = 24576;

  table.symbol[18].symbol = "SP";
  table.symbol[18].value = 0;

  table.symbol[19].symbol = "LCL";
  table.symbol[19].value = 1;

  table.symbol[20].symbol = "ARG";
  table.symbol[20].value = 2;

  table.symbol[21].symbol = "THIS";
  table.symbol[21].value = 3;

  table.symbol[22].symbol = "THAT";
  table.symbol[22].value = 4;

  fseek(sourceCode, 0, SEEK_SET);

  lineNumber = 0;
  while (fgets(line, 255, sourceCode)) {
    u16 letterAt = 0;

    u16 ops = 0;
    u16 dest = 0;
    u16 jump = 0;

    letterAt = skip_space(letterAt, line);

    if (line[letterAt] == '(') {
      table.size++;
      letterAt++;
      table.symbol = realloc(table.symbol, table.size * sizeof(symbol_table));
      line[strcspn(line, ")")] = '\0';
      table.symbol[table.size - 1].symbol = malloc(strlen(line) * sizeof(char));
      strcpy(table.symbol[table.size - 1].symbol, &line[letterAt]);
      table.symbol[table.size - 1].value = lineNumber;
      continue;
    } else if (line[letterAt] == '/') {
      continue;
    } else if (line[letterAt] == '\0' || line[letterAt] == '\n') {
      continue;
    }
    lineNumber++;
  }

  fseek(sourceCode, 0, SEEK_SET);

  lineNumber = 0;

  while (fgets(line, 255, sourceCode)) {
    int letterAt = 0;
    letterAt = skip_space(letterAt, line);
    u16 binaryLine = 0;

    // A instruction
    if (line[letterAt] == '@') {
      letterAt++;
      line[strcspn(line, "\n")] = '\0';
      if (line[letterAt] <= '9' && line[letterAt] >= '0') {
        binaryLine = atoi(&line[letterAt]);
      } else {
        bool found = false;
        for (int i = 0; i < table.size; i++) {
          if (strcmp(&line[letterAt], table.symbol[i].symbol) == 0) {
            binaryLine = table.symbol[i].value;
            found = true;
            break;
          }
        }
        if (found == false) {
          static int value = 16;
          table.size++;
          table.symbol =
              realloc(table.symbol, table.size * sizeof(symbol_table));
          line[strcspn(line, "\n")] = '\0';
          table.symbol[table.size - 1].symbol =
              malloc(strlen(line) * sizeof(char));
          strcpy(table.symbol[table.size - 1].symbol, &line[letterAt]);
          table.symbol[table.size - 1].value = value++;
          binaryLine = table.symbol[table.size - 1].value;
        }
      }
    } else if (line[letterAt] == '/' || line[letterAt] == '\n' ||
               line[letterAt] == '(') {
      continue;
    }
    // C-instruction
    else {
      u16 dest = 0;
      u16 ops = 0;
      u16 jump = 0;

      c_part part = DEST;

      while (line[letterAt] != '\n') {
        if (line[letterAt] == ';') {
          part = JUMP;
        } else if (line[letterAt] == '=') {
          part = OPS;
          letterAt++;
        } else if (!strcontains(line, '=')) {
          part = OPS;
        }

        if (part == DEST) {
          for (; line[letterAt] != '='; letterAt++) {
            if (line[letterAt] == 'M') {
              dest |= 0b1;
            } else if (line[letterAt] == 'D') {
              dest |= 0b10;
            } else if (line[letterAt] == 'A') {
              dest |= 0b100;
            }
          }
        } else if (part == JUMP) {
          letterAt++;
          if (strcmp(&line[letterAt], "JMP\n") == 0) {
            jump = 0b111;
          } else if (strcmp(&line[letterAt], "JLE\n") == 0) {
            jump = 0b110;
          } else if (strcmp(&line[letterAt], "JNE\n") == 0) {
            jump = 0b101;
          } else if (strcmp(&line[letterAt], "JLT\n") == 0) {
            jump = 0b100;
          } else if (strcmp(&line[letterAt], "JGE\n") == 0) {
            jump = 0b011;
          } else if (strcmp(&line[letterAt], "JEQ\n") == 0) {
            jump = 0b010;
          } else if (strcmp(&line[letterAt], "JGT\n") == 0) {
            jump = 0b001;
          }
          letterAt = strlen(line) - 1;
        } else if (part == OPS) {
          letterAt = skip_space(letterAt, line);
          if (line[letterAt] == '0') {
            ops = 0b0101010;
            letterAt++;
            letterAt = skip_space(letterAt, line);
          } else if (line[letterAt] == '1') {
            ops = 0b0111111;
            letterAt++;
            letterAt = skip_space(letterAt, line);
          } else if (line[letterAt] == 'D') {
            letterAt++;
            letterAt = skip_space(letterAt, line);
            if (line[letterAt] == '+') {
              letterAt++;
              letterAt = skip_space(letterAt, line);

              if (line[letterAt] == '1') {
                ops = 0b0011111;
              } else if (line[letterAt] == 'A') {
                ops = 0b0000010;
              } else if (line[letterAt] == 'M') {
                ops = 0b1000010;
              }
              letterAt++;
              letterAt = skip_space(letterAt, line);

            } else if (line[letterAt] == '-') {
              letterAt++;
              letterAt = skip_space(letterAt, line);

              if (line[letterAt] == '1') {
                ops = 0b0001110;
              } else if (line[letterAt] == 'A') {
                ops = 0b0010011;
              } else if (line[letterAt] == 'M') {
                ops = 0b1010011;
              }
              letterAt++;
              letterAt = skip_space(letterAt, line);
            } else if (line[letterAt] == '&') {
              letterAt++;
              letterAt = skip_space(letterAt, line);
              if (line[letterAt] == 'A') {
                ops = 0b0000000;
              } else if (line[letterAt] == 'M') {
                ops = 0b1000000;
              }
              letterAt++;
              letterAt = skip_space(letterAt, line);
            } else if (line[letterAt] == '|') {
              letterAt++;
              letterAt = skip_space(letterAt, line);
              if (line[letterAt] == 'A') {
                ops = 0b0010101;
              } else if (line[letterAt] == 'M') {
                ops = 0b1010101;
              }
              letterAt++;
              letterAt = skip_space(letterAt, line);

            } else {
              ops = 0b0001100;
            }
          } else if (line[letterAt] == 'A') {
            letterAt++;
            letterAt = skip_space(letterAt, line);
            if (line[letterAt] == '+') {
              letterAt++;
              letterAt = skip_space(letterAt, line);
              if (line[letterAt] == '1') {
                ops = 0b0110111;
              }
              letterAt++;
              letterAt = skip_space(letterAt, line);
            } else if (line[letterAt] == '-') {
              letterAt++;
              letterAt = skip_space(letterAt, line);

              if (line[letterAt] == '1') {
                ops = 0b0110010;
              } else if (line[letterAt] == 'D') {
                ops = 0b0000111;
              }
              letterAt++;
              letterAt = skip_space(letterAt, line);

            } else {
              ops = 0b0110000;
            }
          } else if (line[letterAt] == 'M') {
            letterAt++;
            letterAt = skip_space(letterAt, line);
            if (line[letterAt] == '+') {
              letterAt++;
              letterAt = skip_space(letterAt, line);
              if (line[letterAt] == '1') {
                ops = 0b1110111;
              }
              letterAt++;
              letterAt = skip_space(letterAt, line);

            } else if (line[letterAt] == '-') {
              letterAt++;
              letterAt = skip_space(letterAt, line);

              if (line[letterAt] == '1') {
                ops = 0b1110010;
              } else if (line[letterAt] == 'D') {
                ops = 0b1000111;
              }

              letterAt++;
              letterAt = skip_space(letterAt, line);
            } else {
              ops = 0b1110000;
            }
          } else if (line[letterAt] == '!') {
            letterAt++;
            letterAt = skip_space(letterAt, line);
            if (line[letterAt] == 'D') {
              ops = 0b0001101;
            } else if (line[letterAt] == 'A') {
              ops = 0b0110001;
            } else if (line[letterAt] == 'M') {
              ops = 0b1110001;
            }
            letterAt++;
            letterAt = skip_space(letterAt, line);
          } else if (line[letterAt] == '-') {
            letterAt++;
            letterAt = skip_space(letterAt, line);
            if (line[letterAt] == 'D') {
              ops = 0b0001111;
            } else if (line[letterAt] == 'A') {
              ops = 0b0110011;
            } else if (line[letterAt] == 'M') {
              ops = 0b1110011;
            }
            else if (line[letterAt] == '1'){
              ops = 0b0111010;
            }
            letterAt++;
            letterAt = skip_space(letterAt, line);
          }
        }
        binaryLine = (0b111 << 13) | (ops << 6) | (dest << 3) | jump;
      }
    }

    fprintf(objFile, "%s\n", number_to_binary_char(binaryLine));
    lineNumber++;
  }

  fclose(sourceCode);
  fclose(objFile);

  return EXIT_SUCCESS;
}
