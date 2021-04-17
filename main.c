#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

struct data {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
};
typedef struct data Data;

struct lista_letra {
    struct lista_letra * prox;
    struct lista_letra * ant;
    char letra;
};
typedef struct lista_letra Letra;

struct conteudo_arquivo {
    struct conteudo_arquivo * prox;
    struct conteudo_arquivo * ant;
    int letras;
    Letra * letra;
};
typedef struct conteudo_arquivo Linha;

struct lista_arquivos {
    Data data;
    char nome[256];
    struct lista_arquivos * prox;
    Linha * linha;
};
typedef struct lista_arquivos Arquivo;

struct diretorio {
    struct diretorio * head;
    struct diretorio * tail;
    struct diretorio * pai;
    char nome[256];
    Arquivo * arquivos;
    Data data;
};
typedef struct diretorio Diretorio;

struct unidade {
    char nome;
    Diretorio * dir;
    struct unidade * prox;
    struct unidade * ant;
};
typedef struct unidade Unidade;

struct hist {
    char op[256];
    struct hist * prox;
};
typedef struct hist Historico;

Unidade * criaUnidade(char nome) {
    Unidade * U = (Unidade*)malloc(sizeof(Unidade));
    U->nome = nome;
    U->dir = NULL;
    U->prox = NULL;
    U->ant = NULL;

    return U;
}

void insereUnidades(Unidade ** Unid ,int n) {
    int i;
    Unidade * U = *Unid;
    for(i = 67; i < 67 + n; i++) {
        if(U == NULL) {
            *Unid = criaUnidade(i);
            U = *Unid;
        }
        else {
            while(U->prox != NULL)
                U = U->prox;
            U->prox = criaUnidade(i);
        }
    }
}

void insereHist(Historico ** H, char cmd[256]) {
    Historico * aux = *H;
    Historico * novo = (Historico*)malloc(sizeof(Historico));
    novo->prox = NULL;
    strcpy(novo->op, cmd);

    if(*H == NULL)
        *H = novo;
    else {
        while(aux->prox != NULL)
            aux = aux->prox;
        aux->prox = novo;
    }
}

int getBytes(Arquivo * arq) {
    Linha * l = arq->linha;
    int s = 0;
    while(l != NULL) {
        s += l->letras;
        l = l->prox;
    }
    return s;
}

void exibeHistorico(Historico * H) {
    while(H != NULL) {
        printf("%s\n", H->op);
        H = H->prox;
    }
}

void exibeArquivos(Arquivo * arq) {
    int soma = 0, s = 0, i = 0, flag = 0;
    while(arq != NULL) {
        flag = 1;
        s = getBytes(arq);
        printf("%02d/%02d/%02d  %02d:%02d          %d   %s\n", arq->data.dia, arq->data.mes, arq->data.ano, arq->data.hora, arq->data.minuto, s, arq->nome);
        soma += s;
        i++;
        arq = arq->prox;
    }
    printf("%d Arquivo(s)", i);
    if(flag == 1)
        printf("                    %d bytes\n", soma);
    else
        printf("\n");
}

void cmdDIR(Diretorio * dir) {
    while(dir != NULL) {
        printf("%02d/%02d/%02d  %02d:%02d  <DIR>        %s\n", dir->data.dia, dir->data.mes, dir->data.ano, dir->data.hora, dir->data.minuto, dir->nome);
        dir = dir->tail;
    }
}

Diretorio * criaDiretorio(char nome[256], Diretorio * P) {
    time_t t = time(NULL);
    struct tm tm;

    tm = *localtime(&t);
    Diretorio * D = (Diretorio*)malloc(sizeof(Diretorio));
    D->arquivos = NULL;
    strcpy(D->nome, nome);

    D->data.ano = tm.tm_year + 1900;
    D->data.mes = tm.tm_mon + 1;
    D->data.dia = tm.tm_mday;
    D->data.hora = tm.tm_hour;
    D->data.minuto = tm.tm_min;

    D->pai = P;
    D->head = NULL;
    D->tail = NULL;

    return D;
}

void insereDiretorio(Diretorio * pai, Diretorio ** D, char nome[256]) {
    Diretorio * aux = *D;
    Diretorio * novo = criaDiretorio(nome, pai);
    if(*D == NULL)
        *D = novo;
    else {
        while(aux->tail != NULL)
            aux = aux->tail;
        aux->tail = novo;
    }
}

/*char isPath(char nome[256]) {
    int i = 0;
    char boo = 0;
    for(i = 0; i < strlen(nome); i++) {
        if(nome[i] == '\\')
            boo = 1;
    }
    return boo;
}*/

Diretorio * UchangeDir(Unidade * U, char nome[256]) {
    Diretorio * aux = U->dir;
    while(aux != NULL && strcasecmp(aux->nome, nome) != 0)
        aux = aux->tail;
    if(aux != NULL) {
        return aux;
    }
    else {
        printf("%s: Diretorio inexistente!\n", nome);
        return NULL;
    }
}

Diretorio * changeDir(Diretorio * Att, char nome[256]) {
    Diretorio * aux = Att;
    aux = aux->head;
    while(aux != NULL && strcasecmp(aux->nome, nome) != 0)
        aux = aux->tail;
    if(aux != NULL) {
        return aux;
    }
    else {
        printf("%s: Diretorio inexistente!\n", nome);
        return NULL;
    }
}

void Kill(Diretorio ** dir) {
    if(*dir != NULL) {
        Kill(&(*dir)->head);
        Kill(&(*dir)->tail);
        free(dir);
        *dir = NULL;
    }
}

void KillArquivo(Arquivo ** Arq) {
    Arquivo * a = *Arq;
    Linha * lixo, * l = a->linha;
    Letra * k, * let = NULL;
    while(l != NULL) {
        let = l->letra;
        while(let != NULL) {
            k = let;
            let = let->prox;
            free(k);
            k = NULL;
        }
        lixo = l;
        l = l->prox;
        free(lixo);
        lixo = NULL;
    }
}

void removeDir(Diretorio ** Dir, char nome[256]) {
    Diretorio * aux = *Dir;
    Diretorio * ant = NULL;
    aux = aux->head;
    if(strcasecmp(nome, "*") != 0) {
        while(aux != NULL && strcasecmp(aux->nome, nome) != 0) {
            ant = aux;
            aux = aux->tail;
        }
        if(aux != NULL) {
            if(ant != NULL) {
                ant->tail = aux->tail;
                Kill(&(aux)->head);
                free(aux);
            }
            else {
                ant = aux->pai;
                ant->head = aux->tail;
                Kill(&(aux)->head);
                free(aux);
            }
        }
        else
            printf("%s: Diretorio inexistente!\n", nome);
    }
    else {
        while(aux != NULL) {
            ant = aux;
            aux = aux->tail;
            Kill(&(ant)->head);
            free(ant);
            ant = NULL;
        }
        (*Dir)->head = NULL;
    }

}

void UremoveDir(Unidade ** U, Diretorio ** dir, char nome[256]) {
    Diretorio * aux = *dir;
    Diretorio * ant = NULL;
    if(strcasecmp(nome, "*") != 0) {
        while(aux != NULL && strcasecmp(aux->nome, nome) != 0) {
            ant = aux;
            aux = aux->tail;
        }
        if(aux != NULL) {
            if(ant != NULL) {
                ant->tail = aux->tail;
                Kill(&(aux)->head);
                free(aux);
            }
            else {
                (*U)->dir = aux->tail;
                Kill(&(aux)->head);
                free(aux);
            }
        }
        else
            printf("%s: Diretorio inexistente!\n", nome);
    }
    else {
        while(aux != NULL) {
            ant = aux;
            aux = aux->tail;
            Kill(&(ant)->head);
            free(ant);
            ant = NULL;
        }
        *dir = NULL;
    }
}

Arquivo * CriaArquivo(char nome[256]) {
    time_t t = time(NULL);
    struct tm tm;

    tm = *localtime(&t);
    Arquivo * arq = (Arquivo*)malloc(sizeof(Arquivo));
    arq->data.ano = tm.tm_year + 1900;
    arq->data.mes = tm.tm_mon + 1;
    arq->data.dia = tm.tm_mday;
    arq->data.hora = tm.tm_hour;
    arq->data.minuto = tm.tm_min;

    strcpy(arq->nome, nome);
    arq->prox = NULL;
    arq->linha = NULL;

    return arq;
}

Letra * CriaLetra(char t) {
    Letra * let = (Letra*)malloc(sizeof(Letra));
    let->prox = NULL;
    let->ant = NULL;
    let->letra = t;

    return let;
}

void insereLetra(Letra ** Let, char c) {
    Letra * novo = CriaLetra(c);
    Letra * aux = *Let;
    if(*Let == NULL)
        *Let = novo;
    else {
        while(aux->prox != NULL)
            aux = aux->prox;
        aux->prox = novo;
        novo->ant = aux;
    }
}

void insereLinha(Linha ** linha, char st[10000]) {
    int i;
    Linha * l = *linha;
    Linha * newline = (Linha*)malloc(sizeof(Linha));
    newline->ant = NULL;
    newline->prox = NULL;
    newline->letra = NULL;
    newline->letras = strlen(st) - 1;

    if(*linha == NULL) {
        *linha = newline;
    }
    else {
        while(l->prox != NULL)
            l = l->prox;
        l->prox = newline;
        newline->ant = l;
    }
    for(i = 0; i < strlen(st); i++) {
        insereLetra(&newline->letra, st[i]);
    }
}

char openFile(Arquivo ** Arq) {
    char str[10000];
    printf("- ");
    fgets(str, sizeof(str), stdin);
    fgets(str, sizeof(str), stdin);
    while(strcasecmp(str, "S") != 10 && strcasecmp(str, "X") != 10) {
        insereLinha(&(*Arq)->linha, str);
        printf("- ");
        fgets(str, sizeof(str), stdin);
    }
    if(strcasecmp(str, "X") == 10) {
        return 0;
    }
    else
        return 1;
}

void removeArquivo(Arquivo ** Arq, char nome[256]) {
    Arquivo * aux = *Arq;
    Arquivo * ant = NULL;
    while(aux != NULL && strcmp(aux->nome, nome) != 0) {
        ant = aux;
        aux = aux->prox;
    }
    if(aux != NULL) {
        KillArquivo(&aux);
        if(ant == NULL)
            *Arq = aux->prox;
        else
            ant->prox = aux->prox;

        free(aux);
        aux = NULL;
    }
}

void removeArquivoAll(Arquivo ** Arq) {
	Arquivo * aux = *Arq;
	while(*Arq != NULL) {
		while(aux->prox != NULL)
			aux = aux->prox;
		removeArquivo(&(*Arq), aux->nome);
		aux = *Arq;
	}
}

void insereArquivo(Arquivo ** ARQ, char nome[256]) {
    Arquivo * arq = CriaArquivo(nome);
    Arquivo * aux = *ARQ;
    if(*ARQ == NULL) {
        *ARQ = arq;
    }
    else {
        while(aux->prox != NULL)
            aux = aux->prox;
        aux->prox = arq;
    }
    if(!openFile(&arq)) {
        removeArquivo(&(*ARQ), nome);
    }
}

void exibeConteudo(Arquivo * Arq, char nome[256]) {
    Linha * l = NULL;
    Letra * let = NULL;
    while(Arq != NULL && strcmp(Arq->nome, nome) != 0)
        Arq = Arq->prox;
    if(Arq != NULL) {
        l = Arq->linha;
        while(l != NULL) {
            let = l->letra;
            printf("# ");
            while(let != NULL) {
                printf("%c", let->letra);
                let = let->prox;
            }
            l = l->prox;
        }
    }
}

void exibeLinha(Linha * l) {
    Letra * let = l->letra;
    while(let != NULL) {
        printf("%c", let->letra);
        let = let->prox;
    }
}

void finder(Arquivo * arq, char str[1000]) {
    char text[744];
    char filename[256];
    //int flag = -1;
    Linha * l = NULL;
    Letra * let = NULL;
    int i = 1, j, cont = 0;
    if(str[1] == '"') {
        for(i = 2, j = 0; str[i] != '"'; i++, j++)
            text[j] = str[i];
        text[j] = '\0';
        for(i = i + 2, j = 0; i < strlen(str); i++, j++)
            filename[j] = str[i];
        filename[j] = '\0';

        while(arq != NULL && strcmp(arq->nome, filename) == 0)
            arq = arq->prox;
        //printf("arquivo: %s\n", filename);
        //printf("texto: %s\n", text);
        if(arq != NULL) {
            i = 0;
            j = strlen(text);
            l = arq->linha;
            while(l != NULL) {
                let = l->letra;
                while(let != NULL) {
                    if(let->letra == text[0]) {
                        while(let->letra == text[i]) {
                            i++;
                            let = let->prox;
                        }
                        if(i == j) {
                            printf("[%d]: ", cont);
                            exibeLinha(l);
                            //printf("\n");
                        }
                        i = 0;
                    }
                    else
                        let = let->prox;
                }
                cont++;
                l = l->prox;
            }
        }
    }
}

void compArquivo(Arquivo * arq, char nome1[256], char nome2[256]) {
    Arquivo * file1 = NULL;
    Arquivo * file2 = NULL;
    Arquivo * aux = arq;
    int flag = 0, cont = 0;

    while(aux != NULL && strcmp(nome1, aux->nome) != 0)
        aux = aux->prox;
    if(aux != NULL)
        file1 = aux;
    aux = arq;
    while(aux != NULL && strcmp(nome2, aux->nome) != 0)
        aux = aux->prox;
    if(aux != NULL)
        file2 = aux;

    while(file1->linha != NULL && file2->linha != NULL) {
        if(file1->linha->letras == file2->linha->letras) {
            while(file1->linha->letra != NULL && file2->linha->letra != NULL) {
                if(file1->linha->letra->letra != file2->linha->letra->letra)
                    flag = 1;
                file1->linha->letra = file1->linha->letra->prox;
                file2->linha->letra = file2->linha->letra->prox;
            }
            if(flag == 1) {
                printf("%s[%d]: ", nome1, cont);
                exibeLinha(file1->linha);
                printf("\n");
                printf("%s[%d]: ", nome2, cont);
                exibeLinha(file2->linha);
                printf("\n");
            }
            flag = 0;
        }
        else {
            printf("%s[%d]: ", nome1, cont);
            exibeLinha(file1->linha);
            printf("\n");
            printf("%s[%d]: ", nome2, cont);
            exibeLinha(file2->linha);
            printf("\n");
        }
        file1->linha = file1->linha->prox;
        file2->linha = file2->linha->prox;
        cont++;
    }

}

Arquivo * copyArquivo(Arquivo * arq, char nome[256]) {
    Arquivo * temp = NULL;
    while(arq != NULL && strcmp(arq->nome, nome) != 0)
        arq = arq->prox;
    if(arq != NULL) {
        temp = CriaArquivo(arq->nome);
        temp->linha = arq->linha;
        temp->data = arq->data;
        //temp->prox = NULL;
        return temp;
    }
    else {
        printf("%s: Arquivo nao encontrado!\n", nome);
        return NULL;
    }

}

void pasteArquivo(Arquivo ** arq, Arquivo * copied) {
    Arquivo * aux = *arq;
    char resp;
    while(aux != NULL && strcmp(aux->nome, copied->nome) != 0)
        aux = aux->prox;
    if(aux == NULL) {
        //copied->prox = NULL;
        if(*arq == NULL)
            *arq = copied;
        else {
            aux = *arq;
            while(aux->prox != NULL)
                aux = aux->prox;
            aux->prox = copied;
        }
    }
    else {
        printf("%s: Arquivo ja existente!\nDeseja substiui-lo (y/n)? ", copied->nome);
        scanf("%c", &resp);
        removeArquivo(&(*arq), copied->nome);
        aux = *arq;
        while(aux->prox != NULL)
            aux = aux->prox;
        aux->prox = copied;
    }
}

/*
void delay(int n) {
    int mil = 1000 * n;
    clock_t start = clock();
    while (clock() < start + mil);
}
*/

int main() {
    Unidade * U = NULL;
    Historico * H = NULL;
    Diretorio * Atual = NULL;
    Diretorio * att = NULL;
    Arquivo * cop = NULL;
    char dir_atual[256], op[100], unid_atual, nf[256], nome[256], aux[256], str[1000];
    int unid = 1, i;
    system("clear");
    if(unid > 0) {
        insereUnidades(&U, unid);
        unid_atual = U->nome;
        strcpy(dir_atual, "");
        Atual = U->dir;
        printf("%c:\\", unid_atual);
        printf("%s", dir_atual);
        printf("> ");
        scanf("%s", &op);
        while(strcasecmp(op, "EXIT") != 0) {
            if(strcasecmp(op, "DIR") == 0) {                                                ///COMANDO DIR
                insereHist(&H, "DIR");
                if(Atual == NULL)
                    cmdDIR(U->dir);
                else {
                    cmdDIR(Atual->head);
                    exibeArquivos(Atual->arquivos);
                }
            }
            else if(strcasecmp(op, "CLS") == 0) {                                           ///COMANDO CLS
                insereHist(&H, "CLS");
                system("clear");
            }
            else if(strcasecmp(op, "CD") == 0) {                                            ///COMANDO CD (NÃO TOCA NESSA MERDA, É SÉRIO!)
                scanf("%s", &nome);
                strcat(aux, "CD ");
                strcat(aux, nome);
                insereHist(&H, aux);
                if(strcasecmp(nome, ".") == 0) {
                    ///MANTER VAZIO!
                }
                else {
                    if(strcasecmp(nome, "..") == 0) {
                        if(Atual->pai != NULL) {
                            Atual = Atual->pai;
                            for(i = strlen(dir_atual); dir_atual[i] != '\\'; i--);
                            dir_atual[i] = '\0';
                        }
                        else{
                            Atual = NULL;
                            strcpy(dir_atual, "");
                        }
                    }
                    else {
                        if(Atual == NULL) {
                            att = UchangeDir(U, nome);
                            if(att != NULL) {
                                Atual = att;
                                strcpy(dir_atual, Atual->nome);
                            }

                        }
                        else {
                            att = changeDir(Atual, nome);
                            if(att != NULL) {
                                Atual = att;
                                strcat(dir_atual, "\\");
                                strcat(dir_atual, Atual->nome);
                            }
                        }
                    }
                }


            }
            else if(strcasecmp(op, "MD") == 0) {                                            ///COMANDO MD
                scanf("%s", &nome);
                strcat(aux, "MD ");
                strcat(aux, nome);
                insereHist(&H, aux);
                if(Atual == NULL)
                    insereDiretorio(NULL, &U->dir, nome);
                else
                    insereDiretorio(Atual, &Atual->head, nome);
            }
            else if(strcasecmp(op, "RD") == 0) {                                            ///COMANDO RD
                scanf("%s", &nome);
                strcat(aux, "RD ");
                strcat(aux, nome);
                insereHist(&H, aux);
                if(Atual == NULL)
                    UremoveDir(&U, &U->dir, nome);
                else
                    removeDir(&Atual, nome);
            }
            else if(strcasecmp(op, "COPY") == 0) {
                scanf("%s", &nome);
                if(strcasecmp(nome, "CON") == 0) {                                          ///COMANDO COPY CON
                    scanf("%s", &nome);
                    strcat(aux, "COPY CON ");
                    strcat(aux, nome);
                    insereHist(&H, aux);
                    insereArquivo(&Atual->arquivos, nome);
                }
                else {
                    strcat(aux, "COPY ");						    						///COMANDO COPY
                    strcat(aux, nome);
                    insereHist(&H, aux);
                    cop = copyArquivo(Atual->arquivos, nome);
                }
            }
            else if(strcasecmp(op, "PASTE") == 0) {                                         ///COMANDO PASTE
                strcat(aux, "PASTE ");
                strcat(aux, nome);
                insereHist(&H, aux);
                if(cop == NULL)
                    printf("Nenhum arquivo copiado!\n");
                else {
                    pasteArquivo(&Atual->arquivos, cop);
                }
            }
            else if(strcasecmp(op, "DEL") == 0) {                                           ///COMANDO DEL
                scanf("%s", &nome);
                strcat(aux, "DEL ");
                strcat(aux, nome);
                insereHist(&H, aux);
                if(strcmp(nome, "*") == 0)
                	removeArquivoAll(&Atual->arquivos);
               	else
                	removeArquivo(&Atual->arquivos, nome);
            }
            else if(strcasecmp(op, "TYPE") == 0) {                                          ///COMANDO TYPE
                scanf("%s", &nome);
                strcat(aux, "TYPE ");
                strcat(aux, nome);
                insereHist(&H, aux);
                exibeConteudo(Atual->arquivos, nome);
            }
            else if(strcasecmp(op, "FIND") == 0) {                                          ///COMANDO FIND
                fgets(str, sizeof(str), stdin);
                //fgets(str, sizeof(str), stdin);
                //printf("str:%s\n", str);
                strcat(aux, "DEL ");
                strcat(aux, str);
                insereHist(&H, aux);
                finder(Atual->arquivos, str);
            }
            else if(strcasecmp(op, "FC") == 0) {                                            ///COMANDO FC
                scanf("%s", &nome);
                scanf("%s", &nf);
                strcat(aux, "FC ");
                strcat(aux, nome);
                strcat(aux, " ");
                strcat(aux, nf);
                insereHist(&H, aux);
                compArquivo(Atual->arquivos, nome, nf);
            }
            else if(strcasecmp(op, "DOSKEY") == 0) {                                        ///COMANDO DOSKEY
                insereHist(&H, "DOSKEY");
                exibeHistorico(H);
            }
            else if(strcasecmp(op, "HELP") == 0) {                                          ///COMANDO HELP
                insereHist(&H, "HELP");
                printf("DIR: exibe todos os diretorios/arquivos dentro do diretorio atual\n");
                printf("CD [NOME DO DIRETORIO]: entra no diretorio com o [NOME DO DIRETORIO] informado\n");
                printf("MD [NOME DO DIRETORIO]: cria um diretorio com o [NOME DO DIRETORIO] informado\n");
                printf("RD [NOME DO DIRETORIO/*]: remove o diretorio com [NOME DO DIRETORIO] informado. [*] remove todos os diretorios\n");
                printf("COPY [NOME DO ARQUIVO]: copia o arquivo com o nome [ARQUIVO ORIGEM]\n");
                printf("PASTE: cola o arquivo copiado com o comando COPY no diretorio atual\n");
                printf("COPY CON [NOME DO ARQUIVO]: cria um arquivo com o [NOME DO ARQUIVO] informado e edita-o. [S] para salvar e [X] para cancelar a criacao do arquivo\n");
                printf("DEL [NOME DO ARQUIVO/*]: remove o arquivo com [NOME DO ARQUIVO] informado. [*] remove todos os arquivos\n");
                printf("TYPE [NOME DO ARQUIVO]: exibe o conteudo do arquivo com [NOME DO ARQUIVO] informado\n");
                printf("FIND [TEXTO] [NOME DO ARQUIVO]: procura o [TEXTO] informado dentro do conteudo do arquivo com [NOME DO ARQUIVO] informado\n");
                printf("FC [NOME DO ARQUIVO 1] [NOME DO ARQUIVO 2]: compara arquivos com o [NOME DO ARQUIVO 1] [NOME DO ARQUIVO 2] respectivamente, caso haja diferença, retorna as linhas diferentes\n");
                printf("DOSKEY: exibe o histório de todos os comandos ja usados\n");
            }
            else
            {
                printf("%s: comando nao encontrado!\n", op);
                insereHist(&H, op);
            }
            strcpy(aux, "");
            printf("%c:\\", unid_atual);
            printf("%s", dir_atual);
            printf("> ");
            scanf("%s", &op);
        }
    }
    else
        printf("Nenhuma unidade encontrada!");
}
