//************************************************************
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
//************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>

#if __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif linux
	#include <GL/glut.h>
#else
	#include <gl/glut.h>
#endif

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926
#endif

#define PI 3.14

#define LARGURA		800
#define ALTURA		600

#define NUM_OBJETOS   2
#define CENA          0
#define CAIXA         1

// #define TRONCO        2
// #define CABECA        3
// #define BRACO_D       4
// #define BRACO_E       5
// #define QUADRIL       6
// #define PERNA_D       7
// #define PERNA_E       8

#define NUM_TEX       5
#define TEXTURA1   1000
#define TEXTURA2   1001
#define TEXTURA3   1002
#define TEXTURA4   1003
#define TEXTURA5   1004
#define TEXTURA6   1005

GLuint  texture_id[ NUM_TEX ];

struct tipo_transformacao{
	GLfloat dx, dy, dz;         // paramatros de translacao
	GLfloat sx, sy, sz;         // parametros de escala
	GLfloat angx , angy , angz; // parametros de rotacao
};

// definicao dos valores de transformacao
struct tipo_transformacao transf[ NUM_OBJETOS ];

// Variáveis para controles de navegação
GLfloat angle, fAspect;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
GLint x_ini, y_ini, bot, objeto, passo;
char transformacao, eixo;

string objetos[] = {"Cena", "Ar Condiconado"};

struct point
{
	float x, y, z;
};

// ----------------------------------- VARIÁVEIS CAIXA AR CONDICIONADO -----------------------------------

int arLargura = 86;
int arAltura = 60;
int arProfundidade = 30;
int diametroHelice = 40;

int RH = diametroHelice/2;		// Raio da hélice
point FA = {-(arLargura/2), arAltura/2, arProfundidade/2};	// Canto Superior Esquerdo
point FB = {FA.x+arLargura, FA.y, FA.z};					// Canto Superior Direito
point FC = {FA.x+arLargura, FA.y-arAltura, FA.z};			// Canto Inferior Direito
point FD = {FA.x, FA.y-arAltura, FA.z};						// Canto Inferior Esquerdo

point CH = {FA.x+(arLargura/3), FA.y-(arAltura/2), FA.z};	// Centro da Hélice
point HA = {CH.x-RH, CH.y+RH, CH.z};						// Canto Superior Esquerdo da Hélice
point HB = {CH.x+RH, CH.y+RH, CH.z};						// Canto Superior Direito da Hélice
point HC = {CH.x+RH, CH.y-RH, CH.z};						// Canto Inferior Direito da Hélice
point HD = {CH.x-RH, CH.y-RH, CH.z};						// Canto Inferior Esquerdo da Hélice

float RC = 2;		// Raio dos Cantos
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


// ----------------------------------- VARIÁVEIS P/ HÉLICE -----------------------------------

// Numero de vértices para a curva bezier
GLint numero_vertices_helice = 8;

// Vertices que forma a curva bezier
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

void Texturizacao() //faz o carregamento
{
	glEnable(GL_TEXTURE_2D);
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );//Como armazena o pixel
	glGenTextures ( NUM_TEX , texture_id );//armazena q qtidade de textura

	texture_id[ 0 ] = TEXTURA1; // define um numero (identificacao) para a textura
	glBindTexture ( GL_TEXTURE_2D, texture_id[0] );//armazena na posição 0 do vetor
	LoadBMP ( "Texturas/body.bmp" ); // lê a textura

	texture_id[ 1 ] = TEXTURA2;
	glBindTexture ( GL_TEXTURE_2D, texture_id[1] );
	LoadBMP ( "Texturas/head.bmp" );

	texture_id[ 2 ] = TEXTURA3;
	glBindTexture ( GL_TEXTURE_2D, texture_id[2] );
	LoadBMP ( "Texturas/hip.bmp" );

	texture_id[ 3 ] = TEXTURA4;
	glBindTexture ( GL_TEXTURE_2D, texture_id[3] );
	LoadBMP ( "Texturas/arm.bmp" );

	texture_id[ 4 ] = TEXTURA5;
	glBindTexture ( GL_TEXTURE_2D, texture_id[4] );
	LoadBMP ( "Texturas/leg.bmp" );

	texture_id[ 5 ] = TEXTURA6;
	glBindTexture ( GL_TEXTURE_2D, texture_id[5] );
	LoadBMP ( "Texturas/ground.bmp" );

	glTexGeni( GL_S , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
	glTexGeni( GL_T , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
}

// Função responsável pela especificação dos parâmetros de iluminação
void DefineIluminacao (void)
{
	GLfloat luzAmbiente[4]=  {0.4,0.4,0.4,1.0};
	GLfloat luzDifusa[4]=    {0.3,0.3,0.3,1.0};		// "cor"
	GLfloat luzEspecular[4]= {1.0, 1.0, 1.0, 1.0};	// "brilho"
	GLfloat posicaoLuz[4]=   {0.0, 50.0, 50.0, 1.0};

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	// GLint especMaterial = 60;
	GLint especMaterial = 20;

	// Define a refletância do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentração do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros da luz de número 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
}


void SRT (int i, float dx=0, float dy=0, float dz=0, float angx=0, float angy=0, float angz=0, float sx=0, float sy=0, float sz=0)
{
	if (i == -1)
	{
		glTranslatef( dx, dy, dz);
		glRotatef( angx, 1 , 0 , 0 );
		glRotatef( angy, 0 , 1 , 0 );
		glRotatef( angz, 0 , 0 , 1 );
		glScalef( sx, sy, sz);
	}
	else
	{
		glTranslatef( dx + transf[ i ].dx , dy + transf[ i ].dy , dz + transf[ i ].dz );
		glRotatef( angx + transf[ i ].angx , 1 , 0 , 0 );
		glRotatef( angy + transf[ i ].angy , 0 , 1 , 0 );
		glRotatef( angz + transf[ i ].angz , 0 , 0 , 1 );
		glScalef( sx + transf[ i ].sx , sy + transf[ i ].sy , sz + transf[ i ].sz );
	}
}

// desenha cubo passando {textura, height, width e depth}
void DrawCube(int texture, int w, int h, int d)
{
	float TXTx[5] = {0.0, 1/4.0, 2/4.0, 3/4.0, 4/4.0};
	float TXTy[4] = {0.0, 1/3.0, 2/3.0, 3/3.0};

	w/=2; h/=2; d/=2;
	glBindTexture ( GL_TEXTURE_2D, texture );
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin( GL_QUADS );

		// Face frontal
		glNormal3f(   0.0 ,   0.0 ,  1.0 );	// Normal da face
		glTexCoord2f( TXTx[1] , TXTy[1] ); glVertex3f( -w , -h , d );
		glTexCoord2f( TXTx[2] , TXTy[1] ); glVertex3f(  w , -h , d );
		glTexCoord2f( TXTx[2] , TXTy[2] ); glVertex3f(  w ,  h , d );
		glTexCoord2f( TXTx[1] , TXTy[2] ); glVertex3f( -w ,  h , d );

		// Face traseira
		glNormal3f(   0.0 ,   0.0 ,  -1.0 );	// Normal da face
		glTexCoord2f( TXTx[4] , TXTy[2] ); glVertex3f( -w ,  h , -d );
		glTexCoord2f( TXTx[3] , TXTy[2] ); glVertex3f(  w ,  h , -d );
		glTexCoord2f( TXTx[3] , TXTy[1] ); glVertex3f(  w , -h , -d );
		glTexCoord2f( TXTx[4] , TXTy[1] ); glVertex3f( -w , -h , -d );

		// Face superior
		glNormal3f(   0.0 , 1.0 ,  0.0 );	// Normal da face
		glTexCoord2f( TXTx[1] , TXTy[2] ); glVertex3f( -w ,  h ,  d );
		glTexCoord2f( TXTx[2] , TXTy[2] ); glVertex3f(  w ,  h ,  d );
		glTexCoord2f( TXTx[2] , TXTy[3] ); glVertex3f(  w ,  h , -d );
		glTexCoord2f( TXTx[1] , TXTy[3] ); glVertex3f( -w ,  h , -d );

		// Face inferior
		glNormal3f(   0.0 ,-1.0 ,  0.0 );	// Normal da face
		glTexCoord2f( TXTx[1] , TXTy[0] ); glVertex3f( -w , -h , -d );
		glTexCoord2f( TXTx[2] , TXTy[0] ); glVertex3f(  w , -h , -d );
		glTexCoord2f( TXTx[2] , TXTy[1] ); glVertex3f(  w , -h ,  d );
		glTexCoord2f( TXTx[1] , TXTy[1] ); glVertex3f( -w , -h ,  d );

		// Face esquerda
		glNormal3f(   -1.0 , 0.0 ,  0.0 );	// Normal da face
		glTexCoord2f( TXTx[0] , TXTy[1] ); glVertex3f( -w , -h , -d );
		glTexCoord2f( TXTx[1] , TXTy[1] ); glVertex3f( -w , -h ,  d );
		glTexCoord2f( TXTx[1] , TXTy[2] ); glVertex3f( -w ,  h ,  d );
		glTexCoord2f( TXTx[0] , TXTy[2] ); glVertex3f( -w ,  h , -d );

		// Face direita
		glNormal3f(    1.0 , 0.0 ,  0.0 );	// Normal da face
		glTexCoord2f( TXTx[2] , TXTy[1] ); glVertex3f(  w , -h ,  d );
		glTexCoord2f( TXTx[3] , TXTy[1] ); glVertex3f(  w , -h , -d );
		glTexCoord2f( TXTx[3] , TXTy[2] ); glVertex3f(  w ,  h , -d );
		glTexCoord2f( TXTx[2] , TXTy[2] ); glVertex3f(  w ,  h ,  d );

	glEnd();
}


void drawSimpleCircle(point pnt, int raio, float prof, int segmentos)
{
	point at;
	float ang;
	for ( GLint i = 0 ; i <= segmentos ; i ++ ){
		ang =  2 * M_PI * i / segmentos;
		at.x = raio * cos( ang );
		at.y = raio * sin( ang );
		glVertex3f( pnt.x + at.x , pnt.y + at.y , pnt.z - prof);
	}
}

void drawMainCircle( float prof, int segmentos)
{
	float ang;
	point at, ant, hp;	// Atual, Anterior, helice point
	glBegin( GL_TRIANGLE_STRIP );
	glColor4f(0.7, 0.7, 0.7, 1.0);
	for ( GLint i = 0 ; i <= segmentos ; i ++ ){
		ang =  2 * M_PI * i / segmentos;
		at.x = RH * cos( ang );
		at.y = RH * sin( ang );
		glVertex3f( CH.x + at.x , CH.y + at.y , CH.z - prof);
		glVertex3f( CH.x + at.x , CH.y + at.y , CH.z);
	}
	glEnd();

	glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
	for ( GLint i = 0 ; i <= segmentos ; i ++ ){
		ang =  2 * M_PI * i / segmentos;
		at.x = RH * cos( ang );
		at.y = RH * sin( ang );

		if (i==0) {ant = at; continue;}

		if (CH.y + at.y >= CH.y)		// Superior
			if (CH.x + at.x <= CH.x)			// Esquerdo
				hp = HA;
			else 								// Direito
				hp = HB;
		else 							// Inferior
			if (CH.x + at.x <= CH.x)			// Esquerdo
				hp = HD;
			else 								// Direito
				hp = HC;

		glBegin( GL_TRIANGLES );
			glVertex3f( CH.x + ant.x , CH.y + ant.y , CH.z);
			glVertex3f( hp.x , hp.y , hp.z);
			glVertex3f( CH.x + at.x , CH.y + at.y , CH.z);
		glEnd();

		ant = at;
	}

	glBegin( GL_TRIANGLE_FAN );
		glColor4f(0.5, 0.5, 0.5, 1.0);
		drawSimpleCircle(CH, RH, prof, segmentos);
	glEnd();
}

void myVertex3f(point pnt)
{
	glVertex3f(pnt.x, pnt.y, pnt.z);
}

void desenhaFrente()
{
	drawMainCircle( 20, 200 );

	glBegin( GL_TRIANGLE_STRIP );
	glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
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
}

void desenhaAtras()
{
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
	glBegin( GL_QUADS );
	glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
		myVertex3f(LDD);
		myVertex3f(LDC);
		myVertex3f(LDB);
		myVertex3f(LDA);
	glEnd();
}

void desenhaEsferaCanto(point pnt, float offsetZ, int cortes)
{
	glPushMatrix();
		glTranslatef( pnt.x , pnt.y , pnt.z+offsetZ );
		glutSolidSphere(RC, cortes, cortes );
	glPopMatrix();
}

void desenhaCilindroLado(float x, float y, float z, float h, char rot, int cortes)
{
	GLUquadricObj *qobj;
	qobj = gluNewQuadric();

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

void desenhaCaixaAr(void)
{
	SRT(CAIXA);

	// glBegin( GL_POINTS );
	// 	glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
	// 	myVertex3f(FA);
	// 	myVertex3f(FB);
	// 	myVertex3f(FC);
	// 	myVertex3f(FD);
	// 	myVertex3f(CH);
	// 	myVertex3f(HA);
	// 	myVertex3f(HB);
	// 	myVertex3f(HC);
	// 	myVertex3f(HD);
	// 	myVertex3f(CA);
	// 	myVertex3f(CB);
	// 	myVertex3f(CD);
	// 	myVertex3f(CC);
	// 	myVertex3f(LDA);
	// 	myVertex3f(LDB);
	// 	myVertex3f(LDC);
	// 	myVertex3f(LDD);
	// 	myVertex3f(BA);
	// 	myVertex3f(BB);
	// 	myVertex3f(BC);
	// 	myVertex3f(BD);
	// 	myVertex3f(LEA);
	// 	myVertex3f(LEB);
	// 	myVertex3f(LEC);
	// 	myVertex3f(LED);
	// 	myVertex3f(TA);
	// 	myVertex3f(TB);
	// 	myVertex3f(TC);
	// 	myVertex3f(TD);
	// glEnd();

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

	desenhaFrente();
	desenhaAtras();
	desenhaCima();
	desenhaBaixo();
	desenhaLadoEsquerdo();
	desenhaLadoDireito();

	glPopMatrix();


	// Executa os comandos OpenGL
	glutSwapBuffers();
}

// Funcao responsavel por desenhar os objetos
void desenhaHelice(void)
{
    GLUquadricObj *quadObj = gluNewQuadric(); // Objeto quádrico para o cilindro central
    gluQuadricTexture(quadObj, GL_TRUE);
    gluQuadricDrawStyle(quadObj, GLU_FILL);

    glColor4f( 1.0 , 1.0 , 0.0 , 1.0 );

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
}

// Função callback de redesenho da janela de visualização
void Desenha(void)
{
	// Limpa a janela de visualização com a cor
	// de fundo definida previamente
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Chama a função que especifica os parâmetros de iluminação
	DefineIluminacao();


	glColor3f(1.0f, 1.0f, 1.0f);
	// Função da GLUT para fazer o desenho de um "torus"
	// com a cor corrente
	glPushMatrix();
	SRT(CENA);

	desenhaCaixaAr();
	//desenhaHelice();
	// Desenha grade
	// Desenha .....


	glPopMatrix();

	// Executa os comandos OpenGL
	glutSwapBuffers();
}

// Função usada para especificar a posição do observador virtual
void PosicionaObservador(void)
{
	// Especifica sistema de coordenadas do modelo
	glMatrixMode(GL_MODELVIEW);
	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();
	DefineIluminacao();
	// Posiciona e orienta o observador
	glTranslatef(-obsX,-obsY,-obsZ);
	glRotatef(rotX,1,0,0);
	glRotatef(rotY,0,1,0);
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(void)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();

	// Especifica a projeção perspectiva(angulo,aspecto,zMin,zMax)
	gluPerspective(angle,fAspect,0.5,5000);

	PosicionaObservador();
}


// Função responsável por inicializar parâmetros e variáveis
void Inicializa (void)
{
	// Define a cor de fundo da janela de visualização como branca
	glClearColor(0.1, 0.1, 0.1, 1.0);

	// Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);

	// ativa a possibilidade de transparência dos objetos - canal alfa
	glEnable( GL_BLEND );

	// define a forma de cálculo da transparência
	glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

	// Habilita antialiasing
	glEnable( GL_LINE_SMOOTH );

	// Solicita melhor qualidade
	glHint( GL_LINE_SMOOTH_HINT , GL_NICEST );

	// ativa a remoçào das faces ocultas
	glEnable( GL_CULL_FACE );

	// Habilita o modelo de colorização de Gouraud
	glShadeModel(GL_SMOOTH);

	// Para curvas bezier
    glEnable( GL_MAP2_VERTEX_3 );

	for(int i = 0 ; i < NUM_OBJETOS ; i++)
	{
		transf[ i ].dx   = 0.0;
		transf[ i ].dy   = 0.0;
		transf[ i ].dz   = 0.0;
		transf[ i ].sx   = 1.0;
		transf[ i ].sy   = 1.0;
		transf[ i ].sz   = 1.0;
		transf[ i ].angx = 0.0;
		transf[ i ].angy = 0.0;
		transf[ i ].angz = 0.0;
	}

	objeto = CENA;
	transformacao = 'T';
	eixo = 'X';
	passo = 5;

	// Inicializa a variável que especifica o ângulo da projeção
	// perspectiva
	angle=45;

	// Inicializa as variáveis usadas para alterar a posição do
	// observador virtual
	rotX = 0;
	rotY = 0;
	obsX = obsY = 0;
	obsZ = 200;

	Texturizacao();

	cout << endl;
	cout  << "Mouse      Movimenta observador" << endl;
	cout  << "ESC        Sair" << endl;
	cout  << "ALT  +     Aumenta passo" << endl;
	cout  << "ALT  -     Diminui passo" << endl;
	cout  << "HOME       Diminui angulo de abertura da camera" << endl;
	cout  << "END        Aumenta angulo de abertura da camera" << endl;
	cout  << "I          Inicializa" << endl;
	cout  << "X Y Z      Escolhe eixo" << endl;
	cout  << "S R T      Escolhe transformação" << endl;
	cout  << "+ -        Aplica transformação" << endl;
	cout  << "  0        Seleciona cena" << endl;
	cout  << "  1        Seleciona ar condicionado" << endl;
	cout << endl;
}

// Função callback chamada para gerenciar eventos de teclas normais (ESC)
void Teclado ( GLubyte key , GLint x , GLint y )
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
	}
	else
	{
		if ( key == 27 )
			exit( 0 );

		if ( toupper( key ) == 'I' )
			Inicializa();

		if ( toupper( key ) == 'X' || toupper( key ) == 'Y' || toupper( key ) == 'Z')
			eixo = toupper( key );

		if ( toupper( key ) == 'S' || toupper( key ) == 'R' || toupper( key ) == 'T' )
			transformacao = toupper( key );

		if (key-'0' >= 0 && key-'0' <= NUM_OBJETOS-1)
			objeto = key - '0';

		if ( key == 'l' || key == 'L' )
		{
			if( glIsEnabled( GL_LINE_SMOOTH ) )
				glDisable( GL_LINE_SMOOTH );
			else
				glEnable( GL_LINE_SMOOTH );
		}

		if ( key == '+' || key == '=')
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

		if ( key == '-' || key == '_' )
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

	}

	if ( modificador || (key != '+' && key != '=' && key != '-' && key != '_') )
		cout << endl
			 << "    Objeto selecionado: " << objetos[objeto] << endl
			 << "    Transformação: " << transformacao
			 << "    Eixo: " << eixo
			 << "    Passo: " << passo << endl;

	EspecificaParametrosVisualizacao();

	// obriga redesenhar
	glutPostRedisplay();

}

// Função callback para tratar eventos de teclas especiais
void TeclasEspeciais (int tecla, int x, int y)
{
	switch (tecla)
	{
		case GLUT_KEY_HOME:	if(angle>=10)  angle -=5;
							break;
		case GLUT_KEY_END:	if(angle<=150) angle +=5;
							break;
	}
	EspecificaParametrosVisualizacao();
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
		obsX_ini = obsX;
		obsY_ini = obsY;
		obsZ_ini = obsZ;
		rotX_ini = rotX;
		rotY_ini = rotY;
		bot = button;
	}
	else bot = -1;
}

// Função callback para eventos de movimento do mouse
#define SENS_ROT	5.0
#define SENS_OBS	10.0
#define SENS_TRANSL	10.0
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
		obsZ = obsZ_ini + deltaz/SENS_OBS;
	}
	// Botão do meio ?
	else if(bot==GLUT_MIDDLE_BUTTON)
	{
		// Calcula diferenças
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		// E modifica posições
		obsX = obsX_ini + deltax/SENS_TRANSL;
		obsY = obsY_ini - deltay/SENS_TRANSL;
	}
	PosicionaObservador();
	glutPostRedisplay();
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	// Para previnir uma divisão por zero
	if ( h == 0 ) h = 1;

	// Especifica as dimensões da viewport
	glViewport(0, 0, w, h);

	// Calcula a correção de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao();
}

// Programa Principal
int main(int argc , char *argv[])
{
	glutInit( &argc , argv );

	// Define o modo de operação da GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Especifica o tamanho inicial em pixels da janela GLUT
	glutInitWindowSize(LARGURA, ALTURA);

	// Especifica a posição inicial da janela GLUT
	glutInitWindowPosition( ( glutGet( GLUT_SCREEN_WIDTH  ) - LARGURA ) / 2 ,
							( glutGet( GLUT_SCREEN_HEIGHT ) - ALTURA  ) / 2 );

	// Cria a janela passando como argumento o título da mesma
	glutCreateWindow("Robo - Anderson A. Fontana");

	// Registra a função callback de redesenho da janela de visualização
	glutDisplayFunc(Desenha);

	// Registra a função callback de redimensionamento da janela de visualização
	glutReshapeFunc(AlteraTamanhoJanela);

	// Registra a função callback para tratamento das teclas normais
	glutKeyboardFunc (Teclado);

	// Registra a função callback para tratamento das teclas especiais
	glutSpecialFunc (TeclasEspeciais);

	// Registra a função callback para eventos de botões do mouse
	glutMouseFunc(GerenciaMouse);

	// Registra a função callback para eventos de movimento do mouse
	glutMotionFunc(GerenciaMovim);

	// Chama a função responsável por fazer as inicializações
	Inicializa();

	// Inicia o processamento e aguarda interações do usuário
	glutMainLoop();

	return 0;
}
