/*
Nomes:
Giovane Limas Salvi;
Guilherme Eduardo Corrêa Raposo - RA: ;
Thaís Say de Carvalho
*/
#include "trabalho3.h"
#include <stdio.h>
#include <stdlib.h>

void normalizaImagem(Imagem1C *m){
    //normaliza a imagem em uma faxa de 0 a 255
    int i = 0, j = 0, menor = 255, maior = 0;

    for (i = 0; i < m->largura; i++){
        menor = 255, maior = 0;
        for (j = 0; j < m->altura; j++){//acha o menor e maior valor de cada coluna
            if (m->dados[j][i] > maior)
                maior = m->dados[j][i];
            if (m->dados[j][i] < menor)
                menor = m->dados[j][i];
        }
        for (j = 0; j < m->altura; j++)//normaliza cada uma das colunas
            m->dados[j][i] = (m->dados[j][i]-menor)*(255/(maior-menor));
    }

}
int calculaInicio(Imagem1C* img){
    //encontra o comeco
    int i, menor = 255, comeco = 0;

    for(i = 0; i < img->altura; i ++)
        if (img->dados[i][0] < menor){
                menor = img->dados[i][0];
                comeco = i;
        }
    return comeco;
}
void preencheMatrizCusto(int** matriz, Imagem1C* img){
    //funcao que preenche todos os pontos pretos com a distancia ate o comeco
    int i, j, distancia = 0, achou = 1;
    for(i = 0; i < img->altura; i++){//acha todos os pontos pretos da ultima coluna
        if(matriz[i][(img->largura) -1] == -1){
            matriz[i][(img->largura)-1] = distancia;
        }
    }

    while(achou){//enquanto achar linha
        achou = 0;
        //percorre toda a matriz e  acha os ultimos pontos preenchidos
        for(i = 0; i < img->altura; i++){
            for(j = 0; j < img->largura; j++){
                if(matriz[i][j] == distancia){
                    //completa os vizinhos com  a distancia ja calculada mais 1
                    if (i != img->altura - 1)
                        if(matriz[i+1][j] == -1){
                            matriz[i+1][j] = distancia + 1;
                            achou = 1;
                        }
                    if(i != 0)
                        if(matriz[i-1][j] == -1){
                            matriz[i-1][j] = distancia + 1;
                            achou = 1;
                        }
                    if(j != img->largura)
                        if(matriz[i][j+1] == -1){
                            matriz[i][j+1] = distancia + 1;
                            achou = 1;
                        }
                    if(j != 0)
                        if(matriz[i][j-1] == -1){
                            matriz[i][j-1] = distancia + 1;
                            achou = 1;
                        }
                }
            }
        }
        if(achou)
            distancia++;
    }
}
void preencheMatriz(Imagem1C* img, int corte, int** matriz){
    int i, j;
    for (i = 0; i < img->altura; i++){
        for (j = 0; j < img->largura; j ++){
            if (img->dados[i][j] <= corte){// se o ponto for preto preenche com - 1
                matriz[i][j] = -1;
                //se um ponto e menor que o corte , todos os seus vizinhos sao pretos
                if (i!= 0)
                    matriz[i-1][j] = -1;
                if (i!= img->altura -1)
                    matriz[i+1][j] = -1;
                if (j != 0)
                    matriz[i][j-1] = -1;
                if (j != img->largura -1)
                    matriz[i][j+1] = -1;
            }
            else
                matriz[i][j] = -2;//preenche com -2 para claro
        }
    }
    preencheMatrizCusto(matriz, img);
}
int preencheMenorCaminho(Imagem1C* img, int distancia, int pos_y, Coordenada** caminho, int** matriz){
    int n_passos, pos_x = 0, i;
    n_passos = distancia + 1;

    *caminho = (Coordenada*) malloc(n_passos * sizeof(Coordenada));//aloca vetor cordenada

    (*caminho)[0].x = 0;
    (*caminho)[0].y = pos_y;//primeira cordenada

    //procura o proximo passo e aloca a cordenada no caminho:
    for(i = 1; i < n_passos && pos_x < img->largura -1; i++){
        if((pos_x != img->largura) && matriz[pos_y][pos_x + 1] == (distancia - 1)){
            (*caminho)[i].x = pos_x + 1;
            (*caminho)[i].y = pos_y;
        }
        else if((pos_y != img->altura) && matriz[pos_y + 1][pos_x] == distancia - 1){
            (*caminho)[i].x =  pos_x;
            (*caminho)[i].y = pos_y +1;
        }
        else if((pos_x != 0) && matriz[pos_y][pos_x - 1] == distancia - 1){
            (*caminho)[i].x = pos_x - 1;
            (*caminho)[i].y = pos_y;
        }
        else if(matriz[pos_y - 1][pos_x] == distancia - 1){
            (*caminho)[i].x = pos_x;
            (*caminho)[i].y = pos_y -1;
        }
        else{// se tudo der ruim, so siga em frente
            (*caminho)[i].x = pos_x + 1;
            (*caminho)[i].y = pos_y;
        }
        distancia = distancia - 1;
        //posicao x e y atual:
        pos_y = (*caminho)[i].y;
        pos_x = (*caminho)[i].x;
    }
    return n_passos;
}

int** alocaMatrizInt(Imagem1C* img){
    int i;
    int **matriz;
    matriz = (int**)malloc(img->altura * sizeof(int*));
    for(i = 0; i < img->altura; i++)
        matriz[i] = (int*) malloc(img->largura * sizeof(int));

    return matriz;
}
void destroiMatriz(int **matriz, int linhas){
    //funcao para desalocar a matriz de int criada
    int i;
    for(i = 0; i < linhas; i ++)
            free(matriz[i]);
        free(matriz);

}
int encontraCaminho (Imagem1C* img, Coordenada** caminho){
    int n_passos = 0, distancia, corte = 85;
    int inicio;
    int **matriz;
    normalizaImagem(img);//normaliza a imagem em escala de cinza
    inicio = calculaInicio(img);
    matriz = alocaMatrizInt(img);//cria uma matriz de int para que receber a matriz custo
    preencheMatriz(img, corte, matriz);// binariza a imagem e preenche a matriz custo
    distancia = matriz[inicio][0];
    //caso o valor de corte nao tenha sido sufuciente para chegar ao inicio da imagem:
    while (distancia < 0){
        corte +=10;
        preencheMatriz(img, corte, matriz);
        distancia = matriz[inicio][0];
    }
    n_passos = preencheMenorCaminho(img,distancia, inicio,caminho, matriz);//preenche o vetor com as cordenadas do menor caminho
    destroiMatriz(matriz, img->altura);//destroi a matriz alocada

    return n_passos;
}

