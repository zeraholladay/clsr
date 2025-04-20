extern int yyparse();

int main(/*int argc, char *argv[]*/) {
  while (1) {
    yyparse();
  }
  return 0;
}