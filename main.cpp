// ************************************************************
//
//      Construção de um Ar condicionado para disciplina de
//                 Computação Gráfica
//
//        Prof:    Evandro L. Viapiana
//
//      Alunos:    Anderson A. Fontana
//                 Felipe Batistella
//                 Gabriel Paludo Licks
//                 Regis Sganzerla
//
//
// ************************************************************

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sstream>

#if __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif linux
    #include <GL/glut.h>
    #include <unistd.h>
    #define Sleep(x) usleep((x)*1000)
#else
    #include <gl/glut.h>
#endif

using namespace std;

// ************************************************************
// DEFINES
// ************************************************************

#define PI 3.14

#define JANELAS   3
#define LUZES     1

#define LAR_MAIN  600
#define ALT_MAIN  600

#define LAR_INFO  155
#define ALT_INFO  60

#define LAR_HELP  600
#define ALT_HELP  240


#define NUM_TEX   4
#define TEXTURA1  1000
#define TEXTURA2  1001
#define TEXTURA3  1002
#define TEXTURA4  1003

#define NUM_OBJETOS 5

#define CENA 0
#define CAIXA 1
#define VENTILADOR 2
#define GRADEF 3
#define GRADEL 4

// ************************************************************
// VARIÁVEIS GERAIS
// ************************************************************

struct tipo_camera {
    GLfloat posx;               // posicao x da camera
    GLfloat posy;               // posicao y da camera
    GLfloat posz;               // posicao z da camera
    GLfloat alvox;              // alvo x da visualizacao
    GLfloat alvoy;              // alvo y da visualizacao
    GLfloat alvoz;              // alvo z da visualizacao
    GLfloat ang;                // abertura da 'lente' - efeito de zoom
    GLfloat inicio;             // inicio da area de visualizacao em profundidade
    GLfloat fim;                // fim da area de visualizacao em profundidade
};

struct tipo_transformacao{
    GLfloat dx, dy, dz;         // paramatros de translacao
    GLfloat sx, sy, sz;         // parametros de escala
    GLfloat angx , angy , angz; // parametros de rotacao
};

struct tipo_janela{
    GLfloat largura;            // largura da janela
    GLfloat altura;             // altura da jaanela
    GLfloat aspecto;            // aspecto da janela (relacao entre largura e altura)
};

struct tipo_luz{
    GLfloat difusa[ 4 ];
    GLfloat especular[ 4 ];
    GLfloat posicao[ 4 ];
    GLfloat direcao[ 3 ];
    GLfloat cutoff;
    GLfloat exponente;
    bool    ligada;
};

GLfloat angle, fAspect;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX_ini, obsY_ini, obsZ_ini;
int x_ini,y_ini,bot;

// define a perspectiva da camera
tipo_camera camera;

// definicao dos valores de transformacao
tipo_transformacao transf[ NUM_OBJETOS ];

// definicao dos janela principal
tipo_janela janela;

// definicao de uma fonte de luz
tipo_luz luz[ LUZES ];

bool animar, ligado, dirTrem;
int passoAux;

// especularidade e brilho do material
GLfloat especularidade[ 4 ];
GLfloat ambiente[ 4 ];
GLint   espec_material;

GLuint  texture_id[ NUM_TEX ];

// transformacao atual, eixo atual
char  transformacao, eixo, tonalizacao;

// controle das janelas, passo de atualizacao dos parametros das transformacoes e qual objeto mostrar
GLint jan[ JANELAS ], passo, objeto;

// ************************************************************
// VARIÁVEIS CAIXA
// ************************************************************

struct point
{
    float x, y, z;
};

int arLargura = 86;
int arAltura = 60;
int arProfundidade = 30;
int diametroHelice = 40;

int RH = diametroHelice/2;      // Raio da hélice
point FA = {-(arLargura/2), arAltura/2, arProfundidade/2};  // Canto Superior Esquerdo
point FB = {FA.x+arLargura, FA.y, FA.z};                    // Canto Superior Direito
point FC = {FA.x+arLargura, FA.y-arAltura, FA.z};           // Canto Inferior Direito
point FD = {FA.x, FA.y-arAltura, FA.z};                     // Canto Inferior Esquerdo

point CH = {FA.x+(arLargura/3), FA.y-(arAltura/2), FA.z};   // Centro da Hélice
point HA = {CH.x-RH, CH.y+RH, CH.z};                        // Canto Superior Esquerdo da Hélice
point HB = {CH.x+RH, CH.y+RH, CH.z};                        // Canto Superior Direito da Hélice
point HC = {CH.x+RH, CH.y-RH, CH.z};                        // Canto Inferior Direito da Hélice
point HD = {CH.x-RH, CH.y-RH, CH.z};                        // Canto Inferior Esquerdo da Hélice

float RC = 2;       // Raio dos Cantos
// Pontos de Cima
point CA = {FA.x, FA.y+RC, FA.z-arProfundidade};
point CB = {FB.x, FB.y+RC, FB.z-arProfundidade};
point CD = {FA.x, FA.y+RC, FA.z-RC};
point CC = {FB.x, FB.y+RC, FB.z-RC};

// Pontos do Lado Direito
point LDA = {FB.x+RC, FB.y, FB.z-RC};
point LDB = {FB.x+RC, FB.y, FB.z-arProfundidade};
point LDC = {FC.x+RC, FC.y, FC.z-arProfundidade};
point LDD = {FC.x+RC, FC.y, FC.z-RC};

// Pontos de Baixo
point BA = {FD.x, FD.y-RC, FD.z-RC};
point BB = {FC.x, FC.y-RC, FC.z-RC};
point BC = {FC.x, FC.y-RC, FC.z-arProfundidade};
point BD = {FD.x, FD.y-RC, FD.z-arProfundidade};

// Pontos do Lado Esquerdo
point LEA = {FA.x-RC, FA.y, FA.z-arProfundidade};
point LEB = {FA.x-RC, FA.y, FA.z-RC};
point LEC = {FD.x-RC, FD.y, FD.z-RC};
point LED = {FD.x-RC, FD.y, FD.z-arProfundidade};

// Pontos de Tras
point TA = {FB.x, FB.y, FB.z-arProfundidade-RC};
point TB = {FA.x, FA.y, FA.z-arProfundidade-RC};
point TC = {FD.x, FD.y, FD.z-arProfundidade-RC};
point TD = {FC.x, FC.y, FC.z-arProfundidade-RC};

// ************************************************************
// VARIÁVEIS HÉLICE
// ************************************************************

// Vértices que forma a curva bezier
GLfloat vertices_helice[ 8 ][ 8 ][ 3 ] = {
    {{  90 , -30 , 110 },{ 70 , -27 , 110 },{ 50 , -25 , 117 },{ 30 , -20 , 113 },{  0 , -15 , 120 },{ -10 , -10 , 117 },{ -30 , -5 , 113 },{ -50 , 0 , 110 }} ,
    {{ 100 , -30 , 100 },{ 80 , -27 ,  98 },{ 60 , -25 ,  97 },{ 40 , -20 ,  96 },{ 20 , -15 ,  95 },{   0 , -10 ,  94 },{ -20 , -5 ,  92 },{ -45 , 0 ,  90 }} ,
    {{  70 , -30 ,  90 },{ 50 , -27 ,  88 },{ 35 , -25 ,  87 },{ 20 , -20 ,  86 },{  5 , -15 ,  85 },{ -10 , -10 ,  84 },{ -25 , -5 ,  82 },{ -40 , 0 ,  80 }} ,
    {{  55 , -30 ,  70 },{ 40 , -27 ,  70 },{ 30 , -25 ,  70 },{ 20 , -20 ,  70 },{  5 , -15 ,  70 },{  -5 , -10 ,  70 },{ -20 , -5 ,  70 },{ -35 , 0 ,  70 }} ,
    {{  40 , -30 ,  60 },{ 30 , -27 ,  58 },{ 20 , -25 ,  57 },{ 10 , -20 ,  56 },{  0 , -15 ,  55 },{ -10 , -10 ,  54 },{ -20 , -5 ,  52 },{ -30 , 0 ,  50 }} ,
    {{  30 , -30 ,  40 },{ 25 , -27 ,  42 },{ 20 , -25 ,  43 },{ 10 , -20 ,  44 },{  0 , -15 ,  45 },{ -10 , -10 ,  46 },{ -20 , -5 ,  48 },{ -30 , 0 ,  50 }} ,
    {{  25 , -30 ,  20 },{ 20 , -27 ,  20 },{ 15 , -25 ,  20 },{ 10 , -20 ,  20 },{  0 , -15 ,  20 },{ -10 , -10 ,  20 },{ -20 , -5 ,  20 },{ -25 , 0 ,  20 }} ,
    {{  20 , -30 ,   0 },{ 15 , -27 , -10 },{ 10 , -25 , -15 },{  5 , -20 ,   0 },{  0 , -15 ,   0 },{  -5 , -10 ,   0 },{ -15 , -5 ,   0 },{ -20 , 0 ,   0 }}
};

// Número de vértices para a curva bezier
GLint numero_vertices_helice = 8;
// ************************************************************
// VARIÁVEIS SUPORTE
// ************************************************************
GLfloat v[ 18 ][ 3 ] =
{
    {  50 ,  400 ,   0 },   //0
    { -50 ,  400 ,   0 },   //1
    { -50 , -300 ,   0 },   //2
    {  50 , -300 ,   0 },   //3

    { -50 ,  400 ,  40 },   //4
    { -50 , -300 ,  40 },   //5

    {  50 ,  400 ,  40 },   //6
    {  50 , -300 ,  40 },   //7

    { -70 ,  400 ,  40 },   //8
    { -70 , -300 ,  40 },   //9

    { -70 ,  400 , -70 },   //10
    { -70 , -300 , -70 },   //11

    {  70 ,  400 , -70 },   //12
    {  70 , -300 , -70 },   //13

    {  70 ,  400 ,  40 },   //14
    {  70 , -300 ,  40 },   //15

    {  50 ,  400 ,  40 },   //16
    {  50 , -300 ,  40 },   //17



};

GLfloat b[ 18 ][ 3 ] ={

    {  18 , 10 ,  30 },     //0
    { -18 , 10 ,  30 },     //1
    { -18 ,  0 ,  30 },     //2
    {  18 ,  0 ,  30 },     //3

    { -18 , 20 , -90 },     //4
    { -18 ,  0 , -90 },     //5

    {  18 , 20 , -90 },     //6
    {  18 ,  0 , -90 },     //7


};
//declaracao da funcao para utilizar durante animacao
void teclado( unsigned char key , GLint x , GLint y );

// ************************************************************
// FUNÇÕES GERAIS
// ************************************************************

int LoadBMP(char* filename)
{
    #define SAIR        {fclose(fp_arquivo); return -1;}
    #define CTOI(C)     (*(int*)&C)

    GLubyte     *image;
    GLubyte     Header[0x54];
    GLuint      DataPos, imageSize;
    GLsizei     Width,Height;

    // Abre o arquivo e efetua a leitura do Header do arquivo BMP
    FILE * fp_arquivo = fopen(filename,"rb");
    if (!fp_arquivo)
        return -1;
    if (fread(Header,1,0x36,fp_arquivo)!=0x36)
        SAIR;
    if (Header[0]!='B' || Header[1]!='M')
        SAIR;
    if (CTOI(Header[0x1E])!=0)
        SAIR;
    if (CTOI(Header[0x1C])!=24)
        SAIR;

    // Recupera a informação dos atributos de
    // altura e largura da imagem
    Width   = CTOI(Header[0x12]);
    Height  = CTOI(Header[0x16]);
    ( CTOI(Header[0x0A]) == 0 ) ? ( DataPos=0x36 ) : ( DataPos = CTOI(Header[0x0A]) );

    imageSize=Width*Height*3;

    // Efetura a Carga da Imagem
    // image = (GLubyte *) malloc ( imageSize );
    image = new GLubyte[imageSize];
    int retorno;
    retorno = fread(image,1,imageSize,fp_arquivo);

    if (retorno != imageSize) {
        delete (image);
        SAIR;
    }

    // Inverte os valores de R e B
    int t, i;

    for ( i = 0; i < imageSize; i += 3 )
    {
        t = image[i];
        image[i] = image[i+2];
        image[i+2] = t;
    }

    // Tratamento da textura para o OpenGL

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S    ,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T    ,GL_REPEAT);

    glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // Faz a geraçao da textura na memória
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    fclose (fp_arquivo);
    delete (image);
    return 1;
}

void Texturizacao() // faz o carregamento
{
    glEnable(GL_TEXTURE_2D);
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );//Como armazena o pixel
    glGenTextures ( NUM_TEX , texture_id );//armazena q qtidade de textura

    texture_id[ 0 ] = TEXTURA1; // define um numero (identificacao) para a textura
    glBindTexture ( GL_TEXTURE_2D, texture_id[0] );//armazena na posição 0 do vetor
    LoadBMP ( "Texturas/logo.bmp" ); // lê a textura

    texture_id[ 1 ] = TEXTURA2;
    glBindTexture ( GL_TEXTURE_2D, texture_id[1] );
    LoadBMP ( "Texturas/textura1.bmp" );

    texture_id[ 2 ] = TEXTURA3;
    glBindTexture ( GL_TEXTURE_2D, texture_id[2] );
    LoadBMP ( "Texturas/inside.bmp" );

    texture_id[ 3 ] = TEXTURA4;
    glBindTexture ( GL_TEXTURE_2D, texture_id[3] );
    LoadBMP ( "Texturas/parede.bmp" );

    glTexGeni( GL_S , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
    glTexGeni( GL_T , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
}

// Funcao para mostrar texto
void mostra_texto_bitmap( float x , float y , string texto )
{
    glRasterPos2f ( x , y );

    for( int i = 0 ; i < texto.length() ; i++ )
        glutBitmapCharacter( GLUT_BITMAP_8_BY_13 , texto[ i ] );
}

// ************************************************************
// FUNÇÕES JANELA INFO
// ************************************************************

// Funcao responsavel por mostrar o info
void desenha_info(void)
{
    ostringstream msg1, msg2, msg3;

    // Limpa a janela de visualizao com a cor de fundo especificada
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // seleciona o tipo de matriz para a visualizacao dos objetos (modelos)
    glMatrixMode(GL_MODELVIEW);

    glColor4f( 1.0 , 1.0 , 0.0 , 1.0 );

    msg1 << "TRANSFORMACAO : " << transformacao;
    msg2 << "         EIXO : " << eixo;
    msg3 << "        PASSO : " << passo;

    mostra_texto_bitmap( 0 ,  3 , msg3.str() );
    mostra_texto_bitmap( 0 , 23 , msg2.str() );
    mostra_texto_bitmap( 0 , 43 , msg1.str() );

    // Executa os comandos OpenGL
    glutSwapBuffers();
}

// Funcao usada para especificar a projecao ortogonal do info
void especifica_parametros_visualizacao_info( void )
{
    // seleciona o tipo de matriz para a projecao
    glMatrixMode( GL_PROJECTION );

    // limpa (zera) as matrizes
    glLoadIdentity();

    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    gluOrtho2D(  0 , LAR_INFO , 0 , ALT_INFO );

}

// =======================================================================
// Funcao callback chamada quando o tamanho da janela do info eh alterado
void altera_tamanho_janela_info( GLsizei largura , GLsizei altura )
{
    glutReshapeWindow( LAR_INFO , ALT_INFO );

    especifica_parametros_visualizacao_info();
}

// ************************************************************
// FUNÇÕES JANELA HELP
// ************************************************************

// Funcao responsavel por mostrar o help
void desenha_help(void)
{
    // Limpa a janela de visualizao com a cor de fundo especificada
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // seleciona o tipo de matriz para a visualizacao dos objetos (modelos)
    glMatrixMode(GL_MODELVIEW);

    glClear( GL_COLOR_BUFFER_BIT );
    glColor4f( 1.0 , 1.0 , 0.0 , 1.0 );

    mostra_texto_bitmap( 0 , 195 , "ESC           : finaliza o programa" );
    mostra_texto_bitmap( 0 , 180 , "F             : alterna tonalizacao entre flat e smooth" );
    mostra_texto_bitmap( 0 , 165 , "0 a 4         : seleciona um objeto" );
    mostra_texto_bitmap( 0 , 150 , "A             : ligar/desligar" );
    mostra_texto_bitmap( 0 , 135 , "I             : reinicializa" );
    mostra_texto_bitmap( 0 , 120 , "L             : liga/desliga luz" );
    mostra_texto_bitmap( 0 , 105 , "X Y Z         : seleciona o eixo" );
    mostra_texto_bitmap( 0 ,  90 , "S R T         : seleciona a transformacao" );
    mostra_texto_bitmap( 0 ,  75 , "C             : aumenta o angulo de abertura da perspectiva (zoom-out)" );
    mostra_texto_bitmap( 0 ,  60 , "c             : diminui o angulo de abertura da perspectiva (zoom-in)" );
    mostra_texto_bitmap( 0 ,  45 , "+ -           : executa a transformacao corrente sobre o eixo corrente" );
    mostra_texto_bitmap( 0 ,  30 , "ALT NAVEGACAO : movimenta a fonte de luz" );
    mostra_texto_bitmap( 0 ,  15 , "MOUSE         : movimenta a camera" );

    // Executa os comandos OpenGL
    glutSwapBuffers();
}

void SRT(int obj, GLfloat dx =0, GLfloat dy =0, GLfloat dz=0, GLfloat angx=0, GLfloat angy=0, GLfloat angz=0, GLfloat sx=1, GLfloat sy=1, GLfloat sz=1){
    glTranslatef(transf[ obj ].dx + dx, transf[ obj ].dy + dy,transf[ obj ].dz + dz);

    glRotatef( transf[obj].angx + angx, 1 , 0 , 0 );
    glRotatef( transf[obj].angy + angy, 0 , 1 , 0 );
    glRotatef( transf[obj].angz + angz, 0 , 0 , 1 );

    glScalef( transf[obj].sx * sx, transf[obj].sy * sy, transf[obj].sz * sz );
}

// Funcao usada para especificar a projecao ortogonal do help
void especifica_parametros_visualizacao_help( void )
{
    // seleciona o tipo de matriz para a projecao
    glMatrixMode( GL_PROJECTION );

    // limpa (zera) as matrizes
    glLoadIdentity();

    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    gluOrtho2D(  0 , LAR_HELP , 0 , ALT_HELP );
}

// Funcao callback chamada quando o tamanho da janela do help eh alterado
void altera_tamanho_janela_help( GLsizei largura , GLsizei altura )
{
    glutReshapeWindow( LAR_HELP , ALT_HELP );

    especifica_parametros_visualizacao_help();
}

// ************************************************************
// FUNÇÕES ILUMINAÇÃO E VISUALIZAÇÃO
// ************************************************************

// Funcao que cunfigura o uso da iluminacao
void define_iluminacao( void )
{
    glMaterialfv( GL_FRONT , GL_SPECULAR , especularidade );
    glMateriali( GL_FRONT , GL_SHININESS , espec_material );

    // define os parametros da fonte de luz numero 0
    for( int i = 0 ; i < LUZES ; i++ )
    {
        glLightfv( GL_LIGHT0 + i , GL_AMBIENT  , ambiente );
        glLightfv( GL_LIGHT0 + i , GL_DIFFUSE  , luz[ i ].difusa );
        glLightfv( GL_LIGHT0 + i , GL_SPECULAR , luz[ i ].especular );
        glLightfv( GL_LIGHT0 + i , GL_POSITION , luz[ i ].posicao );
    }

    // habilita a criacao automatica da normal
    glEnable( GL_AUTO_NORMAL );
    // habilita a normalizacao do vetor normal
    glEnable( GL_NORMALIZE );
    // ativa o uso da luz ambiente - caso a luz 0 seja desligada
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , ambiente );
    // habilita a definicao da cor do material a partir da cor corrente
    glEnable( GL_COLOR_MATERIAL );
    // habilita o uso da iluminacao
    glEnable( GL_LIGHTING );

    // habilita a fonte de luz numero 0
    if ( luz[ 0 ].ligada )
        glEnable( GL_LIGHT0 );
    else
        glDisable( GL_LIGHT0 );

    // habilita o modelo de colorizacao de Gouraud
    if ( tonalizacao == 'F' )
        glShadeModel( GL_FLAT );
    else
        glShadeModel( GL_SMOOTH );
}

// Função usada para especificar a posição do observador virtual
void PosicionaObservador(void)
{
    // Especifica sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);
    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();
    define_iluminacao();
    // Posiciona e orienta o observador
    glTranslatef(-camera.posx,-camera.posy,-camera.posz);
    glRotatef(rotX,1,0,0);
    glRotatef(rotY,0,1,0);
}

// Funcao usada para especificar o volume de visualizacao
void especifica_parametros_visualizacao( void )
{
    // seleciona o tipo de matriz para a projecao
    glMatrixMode( GL_PROJECTION );

    // limpa (zera) as matrizes
    glLoadIdentity();

    // Especifica e configura a projecao perspectiva
    gluPerspective( camera.ang , janela.aspecto , camera.inicio , camera.fim );

    // Especifica sistema de coordenadas do modelo
    glMatrixMode( GL_MODELVIEW );

    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // Especifica posicao da camera (o observador) e do alvo
    // gluLookAt( camera.posx , camera.posy , camera.posz , camera.alvox , camera.alvoy , camera.alvoz ,0,1,0);
    PosicionaObservador();
}

// Funcao callback chamada quando o tamanho da janela eh alterado
void altera_tamanho_janela( GLsizei largura , GLsizei altura )
{
    janela.largura = largura;
    janela.altura  = altura;

    // Para previnir uma divisao por zero
    if ( janela.altura == 0 ) janela.altura = 1;

    // Especifica as dimensıes da viewport
    glViewport( 0 , 0 , janela.largura , janela.altura );

    // Calcula o aspecto
    janela.aspecto = janela.largura / janela.altura;

    especifica_parametros_visualizacao();
}

// ************************************************************
// FUNÇÕES DESENHO
// ************************************************************
//Suporte
void desenha_suporte(int trans_v, int trans_b ){
GLUquadricObj *sup; // um objeto é criado
sup = gluNewQuadric();


glPushMatrix();
glTranslatef(trans_v , -10 , -25);
        glScalef( 0.1 , 0.1 , 0.1 );
        glColor3ub( 155 , 155 , 155);

        //FRENTE
            glBegin( GL_POLYGON );
            glNormal3f(   0.0 ,   0.0 ,  1.0 );	// Normal da face
                 glTexCoord2f( 1.0 ,  1.0 ); glVertex3fv( v[0] );
                 glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[1] );
                 glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[2] );
                 glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[3] );
            glEnd();

        //ESQUERDA
            glBegin( GL_POLYGON );
             glNormal3f(   1.0 ,   0.0 ,  0.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[1] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[4] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[5] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[2] );
            glEnd();

        //DIREITA
            glBegin( GL_POLYGON );
            glNormal3f(   -1.0 ,   0.0 ,  0.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[6] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[0] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[3] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[7] );
            glEnd();

         //FRENTE_E
         glBegin( GL_POLYGON );
                glNormal3f(   0.0 ,   0.0 ,  1.0 );	// Normal da face
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[4] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[8] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[9] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[5] );
            glEnd();

        //LADO_ESQUERDO
         glBegin( GL_POLYGON );
                 glNormal3f(   -1.0 ,   0.0 ,  0.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[8] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[10] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[11] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[9] );
            glEnd();

        //TRASEIRA
         glBegin( GL_POLYGON );
                glNormal3f(   0.0 ,   0.0 ,  -1.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[10] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[12] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[13] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[11] );
            glEnd();

        //LADO_DIREITO
         glBegin( GL_POLYGON );
                 glNormal3f(   1.0 ,   0.0 ,  0.0 );
                 glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[12] );
                 glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[14] );
                 glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[15] );
                 glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[13] );
            glEnd();

        //FRENTE_D
         glBegin( GL_POLYGON );
                glNormal3f(   0.0 ,   0.0 ,  1.0  );	// Normal da face
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[14] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[16] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[17] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[15] );
            glEnd();

        //LADO_DIREITO
         glBegin( GL_POLYGON );
                glNormal3f(   -1.0 ,   0.0 ,  0.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( v[16] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( v[0] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( v[3] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( v[17] );
            glEnd();
     glPopMatrix();

    glPushMatrix();
     glScalef( 0.4 , 0.4 , 0.4 );
    glTranslatef(trans_b , -100 , 10);

    glBegin( GL_POLYGON );//FRENTE
                glNormal3f(   0.0 ,   0.0 ,  1.0  );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( b[0] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( b[1] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( b[2] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( b[3] );
            glEnd();

     glBegin( GL_POLYGON );//ESQUERDA
                glNormal3f(   -1.0 ,   0.0 ,  0.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( b[1] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( b[4] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( b[5] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( b[2] );
            glEnd();

     glBegin( GL_POLYGON );//TRASEIRA
                glNormal3f(   0.0 ,   0.0 ,  -1.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( b[6] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( b[7] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( b[5] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( b[4] );
            glEnd();

      glBegin( GL_POLYGON );//DIREITA
                glNormal3f(   1.0 ,   0.0 ,  0.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( b[3] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( b[7] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( b[6] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( b[0] );
            glEnd();

    glBegin( GL_POLYGON );//CIMA
                glNormal3f(   0.0 ,   0.8 ,  1  );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( b[6] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( b[4] );
                 glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( b[1] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( b[0] );
            glEnd();

    glBegin( GL_POLYGON );//BAIXO
                glNormal3f(   0.0 ,   -1.0 ,  0.0 );
                glTexCoord2f( 1.0 ,  1.0 );glVertex3fv( b[3] );
                glTexCoord2f( 0.0 ,  1.0 );glVertex3fv( b[2] );
                glTexCoord2f( 0.0 ,  0.0 );glVertex3fv( b[5] );
                glTexCoord2f( 1.0 ,  0.0 );glVertex3fv( b[7] );
            glEnd();

    glPopMatrix();




}

// Hélice
void desenha_helice(void)
{
    GLUquadricObj *quadObj = gluNewQuadric(); // Quádrico p/ cilindro
    gluQuadricTexture(quadObj, GL_TRUE);
    gluQuadricDrawStyle(quadObj, GLU_FILL);

    glColor3ub( 120 , 120 , 120);

    glScalef(0.14, 0.14, 0.14);

    glPushMatrix(); // CENA

        glPushMatrix(); // CILINDRO

            gluCylinder(quadObj, 20, 20, 40, 20, 20);

            glTranslatef(0, 0, 40);
            glRotatef(180, 1, 0, 0);
            glBegin(GL_POLYGON);
                int radius = 20;
                for(double i = 0; i < 2 * PI; i += PI / 50)
                    glVertex3f(cos(i) * radius, sin(i) * radius, 0.0);
            glEnd();

            glPushMatrix(); // HÉLICES

                glTranslatef(0, 0, 40);
                glRotatef(90, 1, 0, 0);

                glMap2f( GL_MAP2_VERTEX_3 , 0.0 , 1.0 , 3 , 8 , 0 , 1 , 3 * numero_vertices_helice , numero_vertices_helice , &vertices_helice[ 0 ][ 0 ][ 0 ] );
                glMapGrid2f( 20 , 0 , 1.0 , 20 , 0 , 1.0 );

                glRotatef( 0 , 0 , 1 , 0 );
                glEvalMesh2( GL_FILL , 0 , 20 , 0 , 20 );
                glRotatef( 90 , 0 , 1 , 0 );
                glEvalMesh2( GL_FILL , 0 , 20 , 0 , 20 );
                glRotatef( 180 , 0 , 1 , 0 );
                glEvalMesh2( GL_FILL , 0 , 20 , 0 , 20 );
                glRotatef( -90 , 0 , 1 , 0 );
                glEvalMesh2( GL_FILL , 0 , 20 , 0 , 20 );

            glPopMatrix();

        glPopMatrix();

    glPopMatrix();
    gluDeleteQuadric(quadObj);
}
/*Cria uma grade. (0,0) é o ponto inferior esquerdo da grade, (x, y) é o ponto superior direito
quantidade total de linhas que a grade tem e quantidade total de colunas(ambos incluindo bordas),
profundidade é o tamanho das "pernas" da grade*/
void desenha_grade(int x, int y, float linhas, float colunas, float raio, int profundidade){
    float tamLin = y/linhas;
    float tamCol = x/colunas;
    GLUquadricObj *qobj;
    qobj = gluNewQuadric();
    long double i;
    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
    glPushMatrix();
    glRotatef( 90, 0 , 1 , 0 );
    for(i=0;i<=y;i+=tamLin){
        glTranslatef( 0, tamLin, 0 );
        gluCylinder(qobj, raio, raio, y, 10, 10);
    }
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0, 0, -profundidade );
    for(i=0;i<=y;i+=tamLin){
        glTranslatef( 0, tamLin, 0 );
        gluCylinder(qobj, raio, raio, profundidade, 10, 10);
    }
    glPopMatrix();
    glPushMatrix();
    glTranslatef( x, 0, -profundidade );
    for(i=0;i<=y;i+=tamLin){
        glTranslatef( 0, tamLin, 0 );
        gluCylinder(qobj, raio, raio, profundidade, 10, 10);
    }
    glPopMatrix();
    glPushMatrix();
    glTranslatef( -tamCol, y+tamLin, raio*2 );
    glRotatef( 90, 1 , 0 , 0 );
    for(i=0;i<=x;i+=tamCol){
        glTranslatef( tamCol, 0, 0 );
        gluCylinder(qobj, raio, raio, x, 10, 10);
    }
    glPopMatrix();
    glPushMatrix();
    glTranslatef( -tamCol, y+tamLin, -profundidade );
    for(i=0;i<=x;i+=tamCol){
        glTranslatef( tamCol, 0, 0 );
        gluCylinder(qobj, raio, raio, profundidade, 10, 10);
    }
    glPopMatrix();
    glPushMatrix();
    glTranslatef( -tamCol, tamLin, -profundidade );
    for(i=0;i<=x;i+=tamCol){
        glTranslatef( tamCol, 0, 0 );
        gluCylinder(qobj, raio, raio, profundidade, 10, 10);
    }
    glPopMatrix();
    gluDeleteQuadric(qobj);
}

void gradeLado(){
    int x = 50;
    int y = 50;
    float linhas = 5.0;
    float colunas = 4.0;
    int profundidade = 5;
    float raio = 0.1;
    glPushMatrix();
        SRT(GRADEL,-48,-33,-14,0,-90,0,0.5);
    desenha_grade(x,y,linhas,colunas,raio, profundidade);
    glPopMatrix();
}

void gradeFrente(){
    int x = 50;
    int y = 50;
    float linhas = 30.0;
    float colunas = 5.0;
    int profundidade = 2;
    float raio = 0.1;
    glPushMatrix();
        SRT(GRADEF,-38,-25,16);
        desenha_grade(x,y,linhas,colunas,raio, profundidade);
    glPopMatrix();
}

void desenhaParede(){
    glPushMatrix();
        glColor3ub( 166 , 56 , 31);
        glTranslatef(0,0,-35);
        glScalef(1, 1, 0.1);
        glutSolidCube(100);

    glPopMatrix();
}

void drawInside(point pnt, int raio, float prof, int segmentos)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture ( GL_TEXTURE_2D, TEXTURA3 );
    glBegin( GL_QUADS );
        glNormal3f(   0.0 ,   0.0 ,  1.0 ); // Normal da face
        glTexCoord2f( 0 , 1 ); glVertex3f( HA.x, HA.y, HA.z-prof); //( -w ,  h , d );
        glTexCoord2f( 1 , 1 ); glVertex3f( HB.x, HB.y, HB.z-prof); //(  w ,  h , d );
        glTexCoord2f( 1 , 0 ); glVertex3f( HC.x, HC.y, HC.z-prof); //(  w , -h , d );
        glTexCoord2f( 0 , 0 ); glVertex3f( HD.x, HD.y, HD.z-prof); //( -w , -h , d );
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawMainCircle( float prof, int segmentos)
{
    float ang;
    point at, ant, hp;  // Atual, Anterior, helice point

    glNormal3f( 0.0 , 0.0 , 1.0 );
    glBegin( GL_TRIANGLE_STRIP );
    glColor4f(0.7, 0.7, 0.7, 1.0);
    for ( GLint i = 0 ; i <= segmentos ; i ++ ){
        ang =  2 * PI * i / segmentos;
        at.x = RH * cos( ang );
        at.y = RH * sin( ang );
        glVertex3f( CH.x + at.x , CH.y + at.y , CH.z - prof);
        glVertex3f( CH.x + at.x , CH.y + at.y , CH.z);
    }
    glEnd();

    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
    for ( GLint i = 0 ; i <= segmentos ; i ++ ){
        ang =  2 * PI * i / segmentos;
        at.x = RH * cos( ang );
        at.y = RH * sin( ang );

        if (i==0) {ant = at; continue;}

        if (CH.y + at.y >= CH.y)        // Superior
            if (CH.x + at.x <= CH.x)            // Esquerdo
                hp = HA;
            else                                // Direito
                hp = HB;
        else                            // Inferior
            if (CH.x + at.x <= CH.x)            // Esquerdo
                hp = HD;
            else                                // Direito
                hp = HC;
        glNormal3f( 0.0 , 0.0 , 1.0 );
        glBegin( GL_TRIANGLES );
            glVertex3f( CH.x + ant.x , CH.y + ant.y , CH.z);
            glVertex3f( hp.x , hp.y , hp.z);
            glVertex3f( CH.x + at.x , CH.y + at.y , CH.z);
        glEnd();

        ant = at;
    }

    glPushMatrix();
        glColor4f(0.8, 0.8, 0.8, 1.0);
        drawInside(CH, RH, prof, segmentos);
    glPopMatrix();
}

void myVertex3f(point pnt)
{
    glVertex3f(pnt.x,   pnt.y,  pnt.z);
}

void desenhaFrente()
{
    drawMainCircle( 20, 200 );

    glNormal3f( 0.0 , 0.0 , 1.0 );
    glBegin( GL_TRIANGLE_STRIP );
        glColor3f(1.0f, 1.0f, 1.0f);
        myVertex3f(HB);
        myVertex3f(FA);
        myVertex3f(HA);
        myVertex3f(FD);
        myVertex3f(HD);
        myVertex3f(FC);
        myVertex3f(HC);
        myVertex3f(FB);
        myVertex3f(HB);
        myVertex3f(FA);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture ( GL_TEXTURE_2D, TEXTURA1 );
    glColor3f(1.0, 1.0, 1.0);
    glBegin( GL_QUADS );

        // Face frontal
        int padding = 5;
        glNormal3f(   0.0 ,   0.0 ,  1.0 ); // Normal da face
        glTexCoord2f( 0 , 0 ); glVertex3f( HB.x+padding, FC.y+padding, FC.z+0.1); //( -w , -h , d );
        glTexCoord2f( 1 , 0 ); glVertex3f( FB.x-padding, FC.y+padding, FC.z+0.1); //(  w , -h , d );
        glTexCoord2f( 1 , 1 ); glVertex3f( FB.x-padding, FB.y-padding, FC.z+0.1); //(  w ,  h , d );
        glTexCoord2f( 0 , 1 ); glVertex3f( HB.x+padding, FB.y-padding, FC.z+0.1); //( -w ,  h , d );
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenhaAtras()
{
    glNormal3f( 0.0 , 0.0 , -1.0 );
    glBegin( GL_QUADS );
    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
        myVertex3f(TD);
        myVertex3f(TC);
        myVertex3f(TB);
        myVertex3f(TA);
    glEnd();
}

void desenhaCima()
{
    glNormal3f( 0.0 , 1.0 , 0.0 );
    glBegin( GL_QUADS );
    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
        myVertex3f(CD);
        myVertex3f(CC);
        myVertex3f(CB);
        myVertex3f(CA);
    glEnd();
}

void desenhaBaixo()
{
    glNormal3f( 0.0 , -1.0 , 0.0 );
    glBegin( GL_QUADS );
    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
        myVertex3f(BD);
        myVertex3f(BC);
        myVertex3f(BB);
        myVertex3f(BA);
    glEnd();
}

void desenhaLadoEsquerdo()
{
    glNormal3f( -1.0 , 0.0 , 0.0 );
    glBegin( GL_QUADS );
    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
        myVertex3f(LED);
        myVertex3f(LEC);
        myVertex3f(LEB);
        myVertex3f(LEA);
    glEnd();
}

void desenhaLadoDireito()
{
    glNormal3f( 1.0 , 0.0 , 0.0 );
    glBegin( GL_QUADS );
    glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
        myVertex3f(LDD);
        myVertex3f(LDC);
        myVertex3f(LDB);
        myVertex3f(LDA);
    glEnd();
}

void desenhaEsferaCanto(point pnt, float offsetZ, int cortes) {
	glPushMatrix();
		glTranslatef( pnt.x , pnt.y , pnt.z+offsetZ );
		glutSolidSphere(RC, cortes, cortes );
	glPopMatrix();
}

void desenhaCilindroLado(float x, float y, float z, float h, char rot, int cortes) {
	GLUquadricObj *qobj;
	qobj = gluNewQuadric();

	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	// gluQuadricDrawStyle( qobj,  GLU_FILL );

	glPushMatrix();
		glTranslatef( x , y , z );
		if (rot == '|')
			glRotatef( 90, 1 , 0 , 0 );
		else if (rot == '-')
			glRotatef( 90 , 0 , 1 , 0 );
		else if (rot == '.')
			glRotatef( 180 , 1 , 0 , 0 );
		gluCylinder( qobj , RC , RC , h , cortes , cortes );
	glPopMatrix();

	gluDeleteQuadric( qobj );
}

void desenhaIndicador(){
    if(animar)
        glColor3ub(0,255,0);
    else
        glColor3ub(255,0,0);
    glTranslated(-35,-27,15);
    glutSolidSphere(1,10,10);
}

void desenhaFundoVentilador(){
    GLUquadricObj *qobj;
	qobj = gluNewQuadric();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	gluQuadricDrawStyle( qobj,  GLU_FILL );

	glPushMatrix();
        glColor3ub(0,0,0);
        glTranslatef(-15,0,0);
        gluDisk(qobj, 0,20,30,30);
    glPopMatrix();
    gluDeleteQuadric(qobj);
}

void tremeCaixa(){
    if(dirTrem)
        transf[ 1 ].dx -= 0.5;
    else
        transf[ 1 ].dx += 0.5;
    dirTrem = !dirTrem;
    teclado(1, 0,0);
}

void ligaVent(){
    if(passoAux<200){
        passoAux++;
    }
    transf[ 2 ].angz -= passoAux;
    teclado(1, 0,0);
    Sleep(15);
}

void desligaVent(){
    transf[ 2 ].angz -= passoAux;
    teclado(1, 0,0);
    Sleep(20);
    if(passoAux>1){
        passoAux--;
    }else{
        ligado=false;
    }
}

void animacao(){
    if(animar){
        tremeCaixa();
        ligaVent();
    }else if(ligado){
        desligaVent();
    }
}
void desenhaCaixa(){
        glBindTexture ( GL_TEXTURE_2D, TEXTURA1 ); // textura corrente
		desenhaFrente();
		desenhaAtras();
		desenhaCima();
		desenhaBaixo();
		desenhaLadoEsquerdo();
		desenhaLadoDireito();


		desenhaEsferaCanto(FA, -RC, 20);
		desenhaEsferaCanto(FB, -RC, 20);
		desenhaEsferaCanto(FC, -RC, 20);
		desenhaEsferaCanto(FD, -RC, 20);

		desenhaEsferaCanto(TA, RC, 20);
		desenhaEsferaCanto(TB, RC, 20);
		desenhaEsferaCanto(TC, RC, 20);
		desenhaEsferaCanto(TD, RC, 20);

		desenhaCilindroLado(FA.x, FA.y, FA.z-RC, arAltura, '|', 20);
		desenhaCilindroLado(FB.x, FB.y, FB.z-RC, arAltura, '|', 20);
		desenhaCilindroLado(FA.x, FA.y, FA.z-RC, arLargura, '-', 20);
		desenhaCilindroLado(FD.x, FD.y, FD.z-RC, arLargura, '-', 20);

		desenhaCilindroLado(TA.x, TA.y, TA.z+RC, arAltura, '|', 20);
		desenhaCilindroLado(TB.x, TB.y, TB.z+RC, arAltura, '|', 20);
		desenhaCilindroLado(TB.x, TB.y, TB.z+RC, arLargura, '-', 20);
		desenhaCilindroLado(TC.x, TC.y, TC.z+RC, arLargura, '-', 20);

		desenhaCilindroLado(FA.x, FA.y, FA.z-RC, arProfundidade-RC, '.', 20);
		desenhaCilindroLado(FB.x, FB.y, FB.z-RC, arProfundidade-RC, '.', 20);
		desenhaCilindroLado(FC.x, FC.y, FC.z-RC, arProfundidade-RC, '.', 20);
		desenhaCilindroLado(FD.x, FD.y, FD.z-RC, arProfundidade-RC, '.', 20);

}

void desenha_arCondicionado(void)
{
	glPushMatrix();

        SRT(CAIXA,0,-2);
        desenhaCaixa();

        glPushMatrix();
            desenhaIndicador();
        glPopMatrix();

        glPushMatrix();
            desenhaFundoVentilador();
        glPopMatrix();

        glPushMatrix();
            SRT(VENTILADOR, -15);
            desenha_helice();
        glPopMatrix();

		glPushMatrix();
            SRT(GRADEF);
            gradeFrente();
        glPopMatrix();

        glPushMatrix();
            SRT(GRADEL);
            gradeLado();
        glPopMatrix();

	glPopMatrix();

}

// Funcao responsavel por desenhar os objetos
void desenha(void)
{
    // Limpa a janela de visualizao com a cor de fundo especificada
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Seleciona o tipo de matriz para a visualizacao dos objetos (modelos)
    glMatrixMode(GL_MODELVIEW);

    SRT(CENA);
    desenhaParede();
    glPushMatrix();
        glBindTexture ( GL_TEXTURE_2D, TEXTURA2 ); // textura corrente
        desenha_suporte(-30, -75);
    glPopMatrix();

    glPushMatrix();
        glBindTexture ( GL_TEXTURE_2D, TEXTURA2 ); // textura corrente
        desenha_suporte(30, 75);
    glPopMatrix();
    glPushMatrix();
        desenha_arCondicionado();
    glPopMatrix();

    animacao();

    // Executa os comandos OpenGL
    glutSwapBuffers();
}

// ************************************************************
// INICIALIZA
// ************************************************************

void inicializa( void )
{
    // Define a cor de fundo da janela de visualizacao como preta
    //             R     G     B    alfa
    glClearColor( 0.1 , 0.1 , 0.1 , 1.0 );

    for( int i = 0 ; i < NUM_OBJETOS ; i++ )
    {
        transf[i].dx   = 0.0;
        transf[i].dy   = 0.0;
        transf[i].dz   = 0.0;
        transf[i].sx   = 1.0;
        transf[i].sy   = 1.0;
        transf[i].sz   = 1.0;
        transf[i].angx = 0.0;
        transf[i].angy = 0.0;
        transf[i].angz = 0.0;
    }

    angle=45;

    // Inicializa as variáveis usadas para alterar a posição do
    // observador virtual
    rotX = 0;
    rotY = 0;

    camera.posx   = 0;
    camera.posy   = 0;
    camera.posz   = 200;
    camera.alvox  = 0;
    camera.alvoy  = 0;
    camera.alvoz  = 0;
    camera.inicio = 0.1;
    camera.fim    = 5000.0;
    camera.ang    = 45;

    transformacao = 'T';
    eixo          = 'X';
    tonalizacao   = 'S';
    passo         = 1;
    passoAux      = 1;

    ambiente[ 0 ] = 0.2;
    ambiente[ 1 ] = 0.2;
    ambiente[ 2 ] = 0.2;
    ambiente[ 3 ] = 1.0;

    // cor
    luz[ 0 ].difusa[ 0 ] = 0.7;
    luz[ 0 ].difusa[ 1 ] = 0.7;
    luz[ 0 ].difusa[ 2 ] = 0.7;
    luz[ 0 ].difusa[ 3 ] = 1.0;

    // brilho
    luz[ 0 ].especular[ 0 ] = 0.7;
    luz[ 0 ].especular[ 1 ] = 0.7;
    luz[ 0 ].especular[ 2 ] = 0.7;
    luz[ 0 ].especular[ 3 ] = 1.0;

    // posicao
    luz[ 0 ].posicao[ 0 ] =    0.0;
    luz[ 0 ].posicao[ 1 ] =  300.0;
    luz[ 0 ].posicao[ 2 ] =  100.0;
    luz[ 0 ].posicao[ 3 ] =    1.0;

    luz[ 0 ].ligada = true;

    // capacidade de brilho do material
    especularidade[ 0 ] = 0.6;
    especularidade[ 1 ] = 0.6;
    especularidade[ 2 ] = 0.6;
    especularidade[ 3 ] = 1.0;
    espec_material = 50;

    animar = false;
    ligado = false;
    dirTrem = true;



    // habilita a transparenica
    glEnable( GL_BLEND );
    // define a forma de calculo da transparencia
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
    // Habilita antialiasing
    glEnable( GL_LINE_SMOOTH );
    // Solicita melhor qualidade
    glHint( GL_LINE_SMOOTH_HINT , GL_NICEST );
    // sentido de criacao da face - frente da face - default: GL_CCW
    glFrontFace( GL_CW );
    // Habilita a remocao de faces
    // glEnable( GL_CULL_FACE );
    // remove faces traseiras - default: traseiras
    glCullFace( GL_BACK );
    // habilita o z-buffer
    glEnable( GL_DEPTH_TEST );
    // habilita map para curvas bezier
    glEnable( GL_MAP2_VERTEX_3 );

    define_iluminacao();

    glutSetWindow( jan[ 0 ] );
    glutReshapeWindow( LAR_HELP , ALT_HELP );
    glutPositionWindow( 10 , 10 );

    glutSetWindow( jan[ 1 ] );
    glutReshapeWindow( LAR_INFO , ALT_INFO );
    glutPositionWindow( 10 , 400 );

    glutSetWindow( jan[ 2 ] );
    glutReshapeWindow( LAR_MAIN , ALT_MAIN );
    glutPositionWindow( ( glutGet( GLUT_SCREEN_WIDTH  ) - LAR_MAIN ) / 2 ,
                        ( glutGet( GLUT_SCREEN_HEIGHT ) - ALT_MAIN ) / 2 );
    Texturizacao();

    PosicionaObservador();
    glutPostRedisplay();
}

// ************************************************************
// FUNÇÕES TECLADO
// ************************************************************

// Funcao de tratamento do teclado
void teclado( unsigned char key , GLint x , GLint y )
{

    if ( key == 27 && !animar )
        exit( 0 );

    if ( key >= '0' && key <= '4' )
        objeto = key - 48;

    if ( toupper(key) == 'A'){
        animar=!animar;
        if(animar){
            ligado = true;
            passoAux = 1;
        }
    }

    if ( toupper( key ) == 'L' )
    {
        luz[ 0 ].ligada = !luz[ 0 ].ligada;
        if ( luz[ 0 ].ligada )
            glEnable( GL_LIGHT0 );
        else
            glDisable( GL_LIGHT0 );
    }

    if ( toupper( key ) == 'F')
    {
        if ( tonalizacao == 'F' )
            tonalizacao = 'S';
        else
            tonalizacao = 'F';
        define_iluminacao();
    }

    if ( toupper( key ) == 'I' )
        inicializa();

    if ( toupper( key ) == 'X' || toupper( key ) == 'Y' || toupper( key ) == 'Z')
        eixo = toupper( key );

    if ( toupper( key ) == 'S' || toupper( key ) == 'R' || toupper( key ) == 'T' )
        transformacao = toupper( key );

    if ( key == 'C')
        if ( camera.ang+passo < 180 )
            camera.ang += passo;

    if ( key == 'c')
        if ( camera.ang-passo > 0 )
            camera.ang -= passo;

    if ( key == '-' || key == '_' ){
        switch( transformacao ){
            case 'S':
                if ( eixo == 'X')
                    transf[ objeto ].sx -= 0.1;
                if ( eixo == 'Y')
                    transf[ objeto ].sy -= 0.1;
                if ( eixo == 'Z')
                    transf[ objeto ].sz -= 0.1;
                break;

            case 'R':
                if ( eixo == 'X')
                    transf[ objeto ].angx -= passo;
                if ( eixo == 'Y')
                    transf[ objeto ].angy -= passo;
                if ( eixo == 'Z')
                    transf[ objeto ].angz -= passo;
                break;

            case 'T':
                if ( eixo == 'X')
                    transf[ objeto ].dx -= passo;
                if ( eixo == 'Y')
                    transf[ objeto ].dy -= passo;
                if ( eixo == 'Z')
                    transf[ objeto ].dz -= passo;
                break;
        }
    }else if ( key == '+' || key == '=' ){
        switch( transformacao ){
            case 'S':
                if ( eixo == 'X')
                    transf[ objeto ].sx += 0.1;
                if ( eixo == 'Y')
                    transf[ objeto ].sy += 0.1;
                if ( eixo == 'Z')
                    transf[ objeto ].sz += 0.1;
                break;

            case 'R':
                if ( eixo == 'X')
                    transf[ objeto ].angx += passo;
                if ( eixo == 'Y')
                    transf[ objeto ].angy += passo;
                if ( eixo == 'Z')
                    transf[ objeto ].angz += passo;
                break;

            case 'T':
                if ( eixo == 'X')
                    transf[ objeto ].dx += passo;
                if ( eixo == 'Y')
                    transf[ objeto ].dy += passo;
                if ( eixo == 'Z')
                    transf[ objeto ].dz += passo;
        break;
        }
    }
    especifica_parametros_visualizacao();

    glutSetWindow( jan[ 1 ] );
    glutPostRedisplay();

    glutSetWindow( jan[ 2 ] );
    glutPostRedisplay();
}

// Tratamento das teclas especiais (teclas de funcao e de navegacao).
// os parametros que recebe sao a tecla pressionada e a posicao x e y
void teclas_especiais( GLint key , GLint x , GLint y )
{
    GLint modificador = glutGetModifiers();

    if ( modificador & GLUT_ACTIVE_ALT)
    {
        // ALT pressionado
        if( key == '-' || key == '_' )
            if( passo - 1 > 0 )
                passo--;

        if( key == '+' || key == '=' )
            passo++;

        if ( key == GLUT_KEY_LEFT )
            luz[ 0 ].posicao[ 0 ] -= passo*3;

        if ( key == GLUT_KEY_RIGHT )
            luz[ 0 ].posicao[ 0 ] += passo*3;

        if ( key == GLUT_KEY_UP )
            luz[ 0 ].posicao[ 1 ] += passo*3;

        if ( key == GLUT_KEY_DOWN )
            luz[ 0 ].posicao[ 1 ] -= passo*3;

        if ( key == GLUT_KEY_PAGE_UP )
            luz[ 0 ].posicao[ 2 ] += passo*3;

        if ( key == GLUT_KEY_PAGE_DOWN)
            luz[ 0 ].posicao[ 2 ] -= passo*3;

        define_iluminacao();
    }
    if( !modificador )
    {
        if ( key == GLUT_KEY_LEFT )
        {
            camera.posx  -= passo;
            camera.alvox -= passo;
        }

        if ( key == GLUT_KEY_RIGHT )
        {
            camera.posx  += passo;
            camera.alvox += passo;
        }

        if ( key == GLUT_KEY_UP )
        {
            camera.posy  += passo;
            camera.alvoy += passo;
        }

        if ( key == GLUT_KEY_DOWN )
        {
            camera.posy  -= passo;
            camera.alvoy -= passo;
        }

        if ( key == GLUT_KEY_PAGE_UP ) // aumenta o tamanho da window
        {
            camera.posz  -= passo;
            camera.alvoz -= passo;
        }

        if ( key == GLUT_KEY_PAGE_DOWN) // diminui o tamanho da window
        {
            camera.posz  += passo;
            camera.alvoz += passo;
        }
    }
    especifica_parametros_visualizacao();

    glutSetWindow( jan[ 2 ] );
    glutPostRedisplay();
}


// Função callback para eventos de botões do mouse
void GerenciaMouse(int button, int state, int x, int y)
{
    if(state==GLUT_DOWN)
    {
        // Salva os parâmetros atuais
        x_ini = x;
        y_ini = y;
        obsX_ini = camera.posx;
        obsY_ini = camera.posy;
        obsZ_ini = camera.posz;
        rotX_ini = rotX;
        rotY_ini = rotY;
        bot = button;
    }
    else bot = -1;
}

// Função callback para eventos de movimento do mouse
#define SENS_ROT    5.0
#define SENS_OBS    10.0
#define SENS_TRANSL 10.0
void GerenciaMovim(int x, int y)
{
    // Botão esquerdo ?
    if(bot==GLUT_LEFT_BUTTON)
    {
        // Calcula diferenças
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        // E modifica ângulos
        rotY = rotY_ini - deltax/SENS_ROT;
        rotX = rotX_ini - deltay/SENS_ROT;
    }
    // Botão direito ?
    else if(bot==GLUT_RIGHT_BUTTON)
    {
        // Calcula diferença
        int deltaz = y_ini - y;
        // E modifica distância do observador
        camera.posz = obsZ_ini + deltaz/SENS_OBS;
    }
    // Botão do meio ?
    else if(bot==GLUT_MIDDLE_BUTTON)
    {
        // Calcula diferenças
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        // E modifica posições
        camera.posx = obsX_ini + deltax/SENS_TRANSL;
        camera.posy = obsY_ini - deltay/SENS_TRANSL;
    }
    PosicionaObservador();
    glutPostRedisplay();
}


// ************************************************************
// MAIN
// ************************************************************

int main( int argc , char *argv[] )
{
    glutInit( &argc , argv );

    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // janela do help
    glutInitWindowSize( LAR_HELP , ALT_HELP );
    glutInitWindowPosition( 10 , 10 );
    jan[ 0 ] = glutCreateWindow( "HELP" );

    // janela do help
    glutInitWindowSize( LAR_INFO , ALT_INFO );
    glutInitWindowPosition( 10 , 400 );
    jan[ 1 ] = glutCreateWindow( "INFO" );

    // janela principal
    glutInitWindowSize( LAR_MAIN , ALT_MAIN );
    glutInitWindowPosition( ( glutGet( GLUT_SCREEN_WIDTH  ) - LAR_MAIN ) / 2 ,
                           ( glutGet( GLUT_SCREEN_HEIGHT ) - ALT_MAIN  ) / 2 );
    jan[ 2 ] = glutCreateWindow( "Ar condicionado" );

    // callbacks da janela de help
    glutSetWindow( jan[ 0 ] );
    glutDisplayFunc( desenha_help );
    glutReshapeFunc( altera_tamanho_janela_help );

    // callbacks da janela de help
    glutSetWindow( jan[ 1 ] );
    glutDisplayFunc( desenha_info );
    glutReshapeFunc( altera_tamanho_janela_info );

    // callbacks da janela principal
    glutSetWindow( jan[ 2 ] );
    glutDisplayFunc( desenha );
    glutKeyboardFunc( teclado );
    glutSpecialFunc( teclas_especiais );
    glutReshapeFunc( altera_tamanho_janela );

    // Registra a função callback para eventos de botões do mouse
    glutMouseFunc(GerenciaMouse);

    // Registra a função callback para eventos de movimento do mouse
    glutMotionFunc(GerenciaMovim);

    // funcao que tem as inicializacoes de variaveis e estados do OpenGL
    inicializa();

    // Ultimo comando. Faz com que todas as janelas criadas sejam mostradas
    // Uma vez neste loop, somente sai quando o programa encerra
    glutMainLoop();

    return 0;
}
