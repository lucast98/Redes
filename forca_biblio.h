char* secret_word;  //String que contém a palavra exibida durante o jogo (letras ocultas = #)
char* wrong_letter; //String que contém os erros dos jogadores
int tries;            //Contador de erros

#define MAX_WORD 30 //Tamanho máximo de uma palavra (evita palavra inexistente muito grande)

//Verifica se o char é válido
int verify_char(char);

//return -> letra na forma minúscula
char standard_char(char);

//Cria a string com letras ocultas
int create_secret(int);

//Retorna o ponteiro da string com letras ocultas
char* get_secret();

//Retorna o número de erros
int get_wrongtry();

//Verifica se a string é válida
int verify_word(char*);

//Verifica se a jogada foi correta
int check_letter(char*, char);

//Verifica o status do jogo
int end_game();