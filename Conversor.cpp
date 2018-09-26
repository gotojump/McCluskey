#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <unistd.h>

// ========================================================================== //

#define BIT_0											0x00										// Bit 0
#define BIT_1											0x01										// Bit 1
#define BIT_X											0x02										// Bit DontCare

#define START_STATE								0												// Estado inicial de um mintermo durante as combinações
#define COMBINED_STATE						1												// Estado onde o mintermo já foi combinado à outro mintermo
#define STABLE_STATE							2												// Estado onde o mintermo já não pode mais ser combinado

#define Type											uint32_t								// Tipo de variável que irá guardar os sinais dos bits
#define MAX_BITS									32											// Quantidade máxima de bits que está variável consegue representar
#define MAX_VET										4												// Tamanho do vetor que irá guardar as variáveis acima
#define MAX_CLASS									( MAX_BITS * MAX_VET )	// Tamanho máximo da posição de um determinado bit na representação

// ========================================================================== //

#define IsLetter( Ch )			( ( ( Ch >= 'a' ) & ( Ch <= 'z' ) ) | ( ( Ch >= 'A' ) & ( Ch <= 'Z' ) ) )
#define IsNumber( Ch )			( ( Ch >= '0' ) & ( Ch <= '9' ) )
#define IsNot( Ch )					( ( Data[ Nn ] == '!' ) || ( Data[ Nn ] == '\'' ) )

// ========================================================================== //

	//				Representação de um sinal binário						 //
	/*-----------------------------------------------------//
		Um sinal binário representa uma sequência de N bits de
	um mesmo componente ou circuito. A sequência de bits
	aqui é representada por dois vetores de inteiros que
	descrevem quais bits da sequência são '1' e quais são '0'.
		O sinal recebe um nome(representado por um
	 caractér) para identificar qual componente ou circuito
	 ele representa a fim de diferenciá-lo de sinais de outros
	 componentes.
	//-----------------------------------------------------*/

class Binary{
	public:
		Type Bits1[ MAX_VET ];	// Representações dos bits 1 do sinal (onde cada bit '1' no vetor representa um bit '1' no sinal binário)
		Type Bits0[ MAX_VET ];	// Representações dos bits 0 do sinal (onde cada bit '1' no vetor representa um bit '0' no sinal binário)
		Type NumOf1;						// Quantidade de bits 1 no sinal
		Type NumOf0;						// Quantidade de bits 0 no sinal
		char Name;							// Nome atribuido ao sinal

	public:
		Binary();
		~Binary();

		uint8_t Print();				// Imprime informações do mintermo
		void Free();						// Reinicia o valor de cada variável do mintermo com valores nulos

		uint8_t SetName( char _Name );							// Muda o nome do sinal binário
		void BitCount();														// Conta a quantidade de bits '0' e '1' representados
		uint8_t Set( uint32_t Pos, uint8_t Val );		// Muda o valor de um determinado bit dentro da representação do sinal (Val = BIT_0, BIT_1 ou BIT_X)
		uint8_t Get( uint32_t Pos );								// Retorna o valor de um determinado bit dentro da representação (Retorno = BIT_0, BIT_1 ou BIT_X)
		uint8_t Copy( Binary Bin );									// Copia um sinal binário
		uint8_t Diference( Binary Bin );						// Calcula quantos bits de diferença há entre dois sinais binários
		void RemoveDiference( Binary Bin );					// Remove as diferenças entre as representações de dois sinais binários
};

// -------------------------------------------------------------------------- //

	//					Representação de um mintermo							 //
	/*-----------------------------------------------------//
		Um mintermo consiste basicamente de um conjunto de sinais
	binários, podendo eles serem de um mesmo componente ou de
	diversos.
	//-----------------------------------------------------*/

class Minterm{
	public:
		std::vector<Binary> Binaries;		// Conjunto de sinais binários que representam o mintermo
		uint32_t NumOf1, NumOf0;				// Número total de '0's e '1's em todos os sinais binários do mintermo
		uint32_t State;									// Estado do mintermo (utilizado apenas nas funções de redução)

	public:
		Minterm();
		~Minterm();

		void Create( uint32_t Nn );			// Cria um mintermo com 'Nn' sinais binários
		void Print();										// Imprime im mintermo
		void Free();										// Reinicia o mintermo com valores nulos

		void Insert( Binary Bin );															// Insere um novo sinal binário ao mintermo
		uint8_t Set( Binary Bin );															// Procura por um sinal binário de mesmo nome que 'Bin' e o substitui por 'Bin' (caso não haja nenhum sinal de mesmo nome, 'Bin' é simplismente inserido ao mintermo)
		uint8_t Set( char _Name, uint32_t Pos, uint8_t Val );		// Similar à função de cima, porém, atualiza apenas um determinado bit do sinal binário de nome '_Name'
		Binary Get( char _Name );																// Retorna o sinal binário cujo nome é '_Name' (cajo não exista, um sinal binário nulo genérico é retornado)
		uint8_t BitCount();																			// Conta a quantidade total de bits '0' e '1' no mintermo
		uint8_t Diference( Minterm Mn );												// Retorna a quantidade de bits diferentes entre todos os sinais de dois mintermos
		void RemoveDiference( Minterm Mn );											// Remove a diferença de bits entre todos os sinais de dois mintermos
};

// -------------------------------------------------------------------------- //

	//						Classe geral de McCluskey								 //
	/*-----------------------------------------------------//
		A principal classe do programa, responsável por
	realizar as principais operações de redução lógica. Nela
	são guardados todos os mintermos que compõem a
	representação lógica a ser reduzida.
	//-----------------------------------------------------*/

class McCluskey{
	public:
		std::vector<Minterm> Classes[ MAX_CLASS ];		// Vetor responsável por classificar cada mintermo de acordo com o número de bits '1'
		std::vector<Minterm> Minterms;								// Vetor contendo a quantidade total de mintermos

	public:
		McCluskey();
		~McCluskey();

		void Print();										// Imprime todos os mintermos sendo representados pelo programa
		void Free();										// Libera todos os mintermos e reinicializa as variáveis da classe

		void Create( char* Data );			// Cria os mintermos atravéz de uma String de dados
		void Insert( Minterm Min );			// Insere um novo mintermo
		void Distribute();							// Distribui os mintermos entre as classes de acordo com o número de bits 1
		void UndoDistribute();					// Desfaz a distribuição anterior
		void RemoveRedundance();				// Remove todos os mintermos repetidos
		void Reduce();									// Realiza a redução de lógica
};

// ========================================================================== //

Binary::Binary(){		memset( this, 0, sizeof( Binary ) );	}

// -------------------------------------------------------------------------- //

Binary::~Binary(){			this->Free();		}

// -------------------------------------------------------------------------- //

// Imprime informações do mintermo
uint8_t Binary::Print(){
	uint32_t Nn, Nm, Np, Nc;

	for( Nm = 0 ; Nm < MAX_VET ; Nm++ ){
		for( Nn = Nc = 0, Np = 1 ; Np ; Nn++, Np = Np << 1 ){
			if( Bits1[ Nm ] & Np ){
				if( Nc )
					printf( "." );
				printf( "%c%d", Name, Nn );
				Nc++;
			}

			if( Bits0[ Nm ] & Np ){
				if( Nc )
					printf( "." );
				printf( "!%c%d", Name, Nn );
				Nc++;
			}
		}
	}

	return( 1 );
}

// -------------------------------------------------------------------------- //

// Reinicia o valor de cada variável do mintermo com valores nulos
void Binary::Free(){

	memset( Bits1, 0, MAX_VET * sizeof( Type ) );
	memset( Bits0, 0, MAX_VET * sizeof( Type ) );
	NumOf1 = NumOf0= 0;
	Name = '\0';
}

// -------------------------------------------------------------------------- //

// Muda o nome do sinal binário
uint8_t Binary::SetName( char _Name ){

	if( IsLetter( _Name ) ){			// Apenas verifica se o nome atribuido é valido (um caractére)
		Name = _Name;
		return( 1 );
	}

	Name = '\0';									// Caso não seja, seu nome passa a ser nulo
	return( 0 );
}

// -------------------------------------------------------------------------- //

// Conta a quantidade de bits '0' e '1' representados
void Binary::BitCount(){
	uint32_t Nn, Nm;							// Nn representa um bit a ser analisado, Nm representa uma posição do vetor de inteiros

	for( NumOf1 = 0, NumOf0 = 0, Nn = 1 ; Nn ; Nn = Nn << 1 ){
		for( Nm = 0 ; Nm < MAX_VET ; Nm++ ){

			if( Bits1[ Nm ] & Nn )		// Se há um bit positivo na representação de sinais '1' na posição Nn
				NumOf1++;								// O contador de '1's é incrementado
			if( Bits0[ Nm ] & Nn )		// Se há um bit positivo na representação de sinais '0' na posição Nn
				NumOf0++;								// O contador de '0's é incrementado
		}
	}

}

// -------------------------------------------------------------------------- //

// Muda o valor de um determinado bit dentro da representação do sinal (Val = BIT_0, BIT_1 ou BIT_X)
uint8_t Binary::Set( uint32_t Pos, uint8_t Val ){
	uint32_t Nm;

	if( Pos > MAX_CLASS )
		return( 0 );

	Nm = Pos / MAX_BITS;
	Pos = Pos % MAX_BITS;

	if( Val == BIT_0 ){								// Bit a ser setado é '0'
		Bits0[ Nm ] |= ( 1 << Pos );		// Representação de '0's recebe um bit '1'
		Bits1[ Nm ] &= ~( 1 << Pos );		// Representação de '1's recebe um bit '0'
	}

	if( Val == BIT_1 ){								// Bit a ser setado é '1'
		Bits0[ Nm ] &= ~( 1 << Pos );		// Representação de '0's recebe um bit '0'
		Bits1[ Nm ] |= ( 1 << Pos );		// Representação de '1's recebe um bit '1'
	}

	if( Val == BIT_X ){								// Bit a ser setado é DontCare
		Bits0[ Nm ] &= ~( 1 << Pos );		// Representação de '0's recebe um bit '0'
		Bits1[ Nm ] &= ~( 1 << Pos );		// Representação de '1's recebe um bit '0'
	}

	this->BitCount();

	return( 1 );
}

// -------------------------------------------------------------------------- //

// Retorna o valor de um determinado bit dentro da representação (Retorno = BIT_0, BIT_1 ou BIT_X)
uint8_t Binary::Get( uint32_t Pos ){
	uint32_t _Pos, Nm;

	if( Pos > MAX_CLASS )		// Posição é maior que o máximo permitido
		return( BIT_X );

	Nm = Pos / MAX_BITS;
	Pos = Pos % MAX_BITS;

	_Pos = 1 << Pos;

	if( Bits1[ Nm ] & _Pos )			// verifica se há algum bit '1' na posição pedida
		return( BIT_1 );

	if( Bits0[ Nm ] & _Pos )			// verifica se há algum bit '0' na posição pedida
		return( BIT_0 );

	return( BIT_X );
}

// -------------------------------------------------------------------------- //

// Copia um sinal binário
uint8_t Binary::Copy( Binary Bin ){

	memcpy( Bits1, Bin.Bits1, sizeof( Type ) * MAX_VET );
	memcpy( Bits0, Bin.Bits0, sizeof( Type ) * MAX_VET );
	NumOf1 = Bin.NumOf1;
	NumOf0 = Bin.NumOf0;
	Name = Bin.Name;

	return( 1 );
}

// -------------------------------------------------------------------------- //

// Calcula quantos bits de diferença há entre dois sinais binários
uint8_t Binary::Diference( Binary Bin ){
	uint32_t Nn, Nm, Nd;

	Nn = 0;

	for( Nm = 0 ; Nm < MAX_VET ; Nm++ ){
		Nd = ( this->Bits1[ Nm ] ^ Bin.Bits1[ Nm ] )
			 | ( this->Bits0[ Nm ] ^ Bin.Bits0[ Nm ] );			// Operação de XOR revela quais bits são diferentes entre os sinais
			 																								// O resultado da operação XOR representa com bits '1' cada posição cujos sinais se diferem
		while( Nd ){																			// Enquanto houverem posições com bits '1' a serem analisadas
			Nn += Nd & 1;																		// Pega apenas o bit menos significativo e incrementa o contador de diferenças se o bit for '1'
			Nd >>= 1;																				// Desloca o resultado de XOR uma casa para a direita
		}
	}

	return( Nn );
}

// -------------------------------------------------------------------------- //

// Remove as diferenças entre as representações de dois sinais binários
void Binary::RemoveDiference( Binary Bin ){
	uint32_t Nd, Nm;

	for( Nm = 0 ; Nm < MAX_VET ; Nm++ ){
		Nd = ~( ( this->Bits1[ Nm ] ^ Bin.Bits1[ Nm ] )
		 		| ( this->Bits0[ Nm ] ^ Bin.Bits0[ Nm ] ) );	// Pega a diferença entre os sinais e inverte os bits

		Bits1[ Nm ] &= Nd;																// Remove a diferença por meio de um AND com os bits invertidos
		Bits0[ Nm ] &= Nd;
	}
}

// ========================================================================== //

Minterm::Minterm(){		memset( this, 0, sizeof( Minterm ) );		}

// -------------------------------------------------------------------------- //

Minterm::~Minterm(){	this->Free();		}

// -------------------------------------------------------------------------- //

// Cria um mintermo com 'Nn' sinais binários
void Minterm::Create( uint32_t Nn ){

	this->Free();
	Binaries.reserve( Nn );
}

// -------------------------------------------------------------------------- //

// Imprime im mintermo
void Minterm::Print(){
	uint32_t Nn, Nm, Np, Nb, Nc;
	uint8_t Val;
	Binary Bin;

	for( Nm = Nc = 0 ; Nm < MAX_VET ; Nm++ ){
		for( Nn = 0, Np = 1 ; Np ; Nn++, Np = Np << 1 ){
			for( Nb = 0 ; Nb < Binaries.size() ; Nb++ ){
				Bin = Binaries[ Nb ];
				Val = Bin.Get( Nn + ( MAX_BITS * Nm ) );

				if( Val == BIT_0 ){
					if( Nc )
						printf( "." );
					printf( "!%c%d", Bin.Name, Nn + ( MAX_BITS * Nm ) );
					Nc++;
				}

				if( Val == BIT_1 ){
					if( Nc )
						printf( "." );
					printf( "%c%d", Bin.Name, Nn + ( MAX_BITS * Nm ) );
					Nc++;
				}

			}
		}
	}

}

// -------------------------------------------------------------------------- //

// Reinicia o mintermo com valores nulos
void Minterm::Free(){

	Binaries.clear();
	NumOf1 = 0;
	NumOf0 = 0;
	State = 0;
}

// -------------------------------------------------------------------------- //

// Insere um novo sinal binário ao mintermo
void Minterm::Insert( Binary Bin ){

	Binaries.push_back( Bin );
}

// -------------------------------------------------------------------------- //

// Procura por um sinal binário de mesmo nome que 'Bin' e o substitui por 'Bin' (caso não haja nenhum sinal de mesmo nome, 'Bin' é simplismente inserido ao mintermo)
uint8_t Minterm::Set( Binary Bin ){
	uint32_t Nn;

	for( Nn = 0 ; Nn < Binaries.size() ; Nn++ ){		// Verifica se a classe de mintermos de nome _Name existe
		if( Bin.Name == Binaries[ Nn ].Name ){				// Compara individualmente cada nome de cada sinal binário
			Binaries[ Nn ] = Bin;
			this->BitCount();
			return( 1 );
		}
	}

	this->Insert( Bin );														// Se não existe, insere no final do vetor de Binaries
	this->BitCount();
	return( 0 );
}

// -------------------------------------------------------------------------- //

// Similar à função de cima, porém, atualiza apenas um determinado bit do sinal binário de nome '_Name'
uint8_t Minterm::Set( char _Name, uint32_t Pos, uint8_t Val ){
	Binary Bin;
	uint32_t Nn;

	for( Nn = 0 ; Nn < Binaries.size() ; Nn++ ){		// Verifica se a classe de mintermos de nome _Name existe
		Bin = Binaries[ Nn ];
		if( _Name == Bin.Name ){											// Compara individualmente cada nome de cada sinal binário
			Bin.Set( Pos, Val );												// Se a classe existe, o sinal binário de nome _Name é atualizado
			Binaries[ Nn ] = Bin;
			this->BitCount();
			return( 1 );
		}
	}

	Bin.Free();																			// Se não existe, ela é criada
	Bin.SetName( _Name );
	Bin.Set( Pos, Val );
	Binaries.push_back( Bin );
	this->BitCount();

	return( 1 );
}

// -------------------------------------------------------------------------- //

// Retorna o sinal binário cujo nome é '_Name' (cajo não exista, um sinal binário nulo genérico é retornado)
Binary Minterm::Get( char _Name ){
	Binary Bin;
	uint32_t Nn;

	for( Nn = 0 ; Nn < Binaries.size() ; Nn++ ){		// Verifica se a classe de mintermos de nome _Name existe
		Bin = Binaries[ Nn ];
		if( _Name == Bin.Name )												// Compara individualmente cada nome de cada sinal binário
			return( Bin );
	}

	Bin.Free();																			// Se não existe, retorna um Binary vazio
	return( Bin );
}

// -------------------------------------------------------------------------- //

// Conta a quantidade total de bits '0' e '1' no mintermo
uint8_t Minterm::BitCount(){
	uint32_t Nn;
	Binary Bin;

	NumOf1 = NumOf0 = 0;
	for( Nn = 0 ; Nn < Binaries.size() ; Nn++ ){		// Para cada sinal binário no mintermo
		Bin = Binaries[ Nn ];
		Bin.BitCount();																// Realiza-se a contagem de bits do sinal
		NumOf1 += Bin.NumOf1;													// Soma-se a quantidade de bits '0' e '1' do sinal à quantidade total do mintermo
		NumOf0 += Bin.NumOf0;
		Binaries[ Nn ] = Bin;
	}

	return( 1 );
}

// -------------------------------------------------------------------------- //

// Retorna a quantidade de bits diferentes entre todos os sinais de dois mintermos
uint8_t Minterm::Diference( Minterm Mn ){
	Binary Bin1, Bin2;
	uint32_t Nd;
	char Nc;

	for( Nc = 'A' ; Nc <= 'z' ; Nc++ ){				// Realiza uma busca passando por todos os nomes possíveis de sinais binários

		if( ( Nc > 'Z' ) && ( Nc < 'a' ) )			// Apenas avança o intervalo entre 'Z' e 'a'
			Nc = 'a';

		Bin1 = this->Get( Nc );									// Procura por um sinal de nome Nc neste mintermo
		Bin2 = Mn.Get( Nc );										// Procura por um sinal de nome Nc no mintermo passado por parâmetro

		Nd += Bin1.Diference( Bin2 );						// Soma a diferença em bits de ambos os sinais à variável Nd
	}

	return( Nd );															// Retorna a soma total das diferenças
}

// -------------------------------------------------------------------------- //

// Remove a diferença de bits entre todos os sinais de dois mintermos
void Minterm::RemoveDiference( Minterm Mn ){
	Binary Bin1, Bin2;
	char Nc;

	for( Nc = 'A' ; Nc <= 'z' ; Nc++ ){				// Realiza uma busca passando por todos os nomes possíveis de sinais binários

		if( ( Nc > 'Z' ) && ( Nc < 'a' ) )			// Apenas avança o intervalo entre 'Z' e 'a'
			Nc = 'a';

		Bin1 = this->Get( Nc );									// Procura por um sinal de nome Nc neste mintermo
		Bin2 = Mn.Get( Nc );										// Procura por um sinal de nome Nc no mintermo passado por parâmetro

		if( Bin1.Name != '\0' ){								// Se o sinal com nome Nc existe, ou seja, Minterm::Get não retornou um sinal com nome nulo
			Bin1.RemoveDiference( Bin2 );					// Então é removida a diferença entre os dois sinais de nome NC
			this->Set( Bin1 );										// O valor do sinal é atualizado neste mintermo
		}
	}
}

// ========================================================================== //

McCluskey::McCluskey(){		memset( this, 0, sizeof( McCluskey ) );		}

// -------------------------------------------------------------------------- //

McCluskey::~McCluskey(){	this->Free();		}

// -------------------------------------------------------------------------- //

// Imprime todos os mintermos sendo representados pelo programa
void McCluskey::Print(){
	uint32_t Nn, Ors, Ands, Nots;
	Minterm Min;

	Ors = Ands = Nots = 0;

	printf( "\n\n ===== McCluskey =====\n\n" );
	for( Nn = 0 ; Nn < Minterms.size() ; Nn++ ){
		Min = Minterms[ Nn ];
		Min.Print();

		Ands += Min.NumOf0 + Min.NumOf1 - 1;
		Nots += Min.NumOf0;
		if( Nn < Minterms.size() - 1 ){
			printf( " + " );
			Ors++;
		}
	}

	puts( "\n" );
	for( Nn = 0 ; Nn < MAX_CLASS ; Nn++ ){
		if( Classes[ Nn ].size() )
			printf( "- %lu minterms with %d bits '1'.\n", Classes[ Nn ].size(), Nn );
	}

	puts("");
	printf( "Operadores NOT: %u\n", Nots );
	printf( "Operadores AND: %u\n", Ands );
	printf( "Operadores OR: %u\n", Ors );

	printf( "\n" );
}

// -------------------------------------------------------------------------- //

// Libera todos os mintermos e reinicializa as variáveis da classe
void McCluskey::Free(){
	uint32_t Nn;

	for( Nn = 0 ; Nn < MAX_CLASS ; Nn++ )			// Para cada vetor de classes existente
		Classes[ Nn ].clear();									// Libera o conteúdo do vetor
	Minterms.clear();													// Apaga todos os mintermos existentes
}

// -------------------------------------------------------------------------- //

// Cria os mintermos atravéz de uma String de dados
void McCluskey::Create( char* Data ){
	uint32_t Pos, Val;												// Pos indica uma posição de um bit em um sinal, Val indica o valor de um bit de um sinal
	uint32_t Nn;															// Nn indica qual caracter de 'Data' está sendo analisado (Qual posição no vetor de dados está sendo analisada)
	Minterm Min;															// Min é utilizado para armazenar as informações de um mintermo durante a leitura
	char Name;																// Name indica o nome de um sinal

	if( !Data )
		return;

	Nn = 0;
	Name = '\0';
	Pos = 0;
	Val = BIT_1;

	while( 1 ){
		if( IsLetter( Data[ Nn ] ) ){						// Verifica se o caracter analisado é uma letra
			Name = Data[ Nn ];										// Se sim, atualiza o valor de 'Name'
		}

		if( IsNumber( Data[ Nn ] ) ){						// Verifica se o caracter analisado é um número
			Pos = ( Pos * 10 ) + Data[ Nn ] - '0';// Se sim, atualiza o valor de 'Pos', multiplicando seu valor por 10 e somando o número lido
		}

		if( IsNot( Data[ Nn ] ) ){							// Verifica se o caracter analisado é '!' ou '
			Val = BIT_0;													// Se sim, atualiza o valor de 'Val' para '0'
		}

		if( Data[ Nn ] == '.' ){								// Verifica se o caracter analisado é '.'
			Min.Set( Name, Pos, Val );						// Se sim, gera/atualiza o sinal de nome 'Name' em 'Min' com os valores lidos
			Name = '\0';													// Reinicia os valores de 'Name', 'Pos' e 'Val'
			Pos = 0;
			Val = BIT_1;
		}

		if( Data[ Nn ] == '+' ){								// Verifica se o caracter analisado é '+'
			Min.Set( Name, Pos, Val );						// Se sim, gera/atualiza o sinal de nome 'Name' em 'Min' com os valores lidos
			this->Insert( Min );									// E adiciona o mintermo lido ao vetor de mintermos do programa

			Min.Free();														// Gera um mintermo vazio para continuar a leitura
			Name = '\0';													// Reinicia os valores de 'Name', 'Pos' e 'Val'
			Pos = 0;
			Val = BIT_1;
		}

		if( Data[ Nn ] == '\0' ){								// Verifica se não há mais caracteres a serem lidos
			Min.Set( Name, Pos, Val );						// Se não há, gera/atualiza o sinal de nome 'Name' em 'Min' com os valores lidos
			this->Insert( Min );									// E adiciona o mintermo lido ao vetor de mintermos do programa
			break;
		}

		Nn++;
	}

	this->RemoveRedundance();									// Busca remover mintermos repetidos
}

// -------------------------------------------------------------------------- //

// Insere um novo mintermo
void McCluskey::Insert( Minterm Min ){

	Minterms.push_back( Min );
}

// -------------------------------------------------------------------------- //

// Distribui os mintermos entre as classes de acordo com o número de bits 1
void McCluskey::Distribute(){
	Minterm Min;
	uint32_t Nn;

	this->UndoDistribute();															// Desfaz uma distribuição anterior

	for( Nn = 0 ; Nn < Minterms.size() ; Nn++ ){				// Para cada mintermo existente
		Min = Minterms[ Nn ];
		//if( Min.State == COMBINED_STATE )
		Min.State = START_STATE;													// Muda seu estado para o estado inicial
		Min.BitCount();																		// Realiza a contagem de bits no mintermo
		Classes[ Min.NumOf1 ].push_back( Min );						// E insere o mintermo na classe de número igual a sua quantidade de bits '1'
	}
}

// -------------------------------------------------------------------------- //

// Desfaz a distribuição anterior
void McCluskey::UndoDistribute(){
	uint32_t Nn;

	for( Nn = 0 ; Nn < MAX_CLASS ; Nn++ ){																	// Para cada mintermo existente
		Classes[ Nn ].erase( Classes[ Nn ].begin(), Classes[ Nn ].end() );		// Apaga os valores de cada classe de mintermos
	}
}

// -------------------------------------------------------------------------- //

// Remove todos os mintermos repetidos
void McCluskey::RemoveRedundance(){
	Minterm Min1, Min2;
	int32_t Nn, Nm;

	for( Nn = 0 ; Nn < (int32_t)Minterms.size() ; Nn++ ){										// Para cada mintermo existente, indo desde o primeiro até o último
		Min1 = Minterms[ Nn ];
		for( Nm = Nn + 1 ; Nm < (int32_t)Minterms.size() ; Nm++ ){						// Para cada mintermo entre as posições Nm + 1 e a última posição do vetor
			Min2 = Minterms[ Nm ];
			if( Min1.Diference( Min2 ) == 0 ){																	// Verifica se a diferença entre ambos os mintermos é 0
				Minterms.erase( Minterms.begin() + Nm );													// Se sim, remove o segundo mintermo
				Nm--;																															// E recua o contador do segundo mintermo em uma posição
			}
		}
	}

}

// -------------------------------------------------------------------------- //

// Realiza a redução de lógica
void McCluskey::Reduce(){
	int32_t Cls1, Cls2, N1, N2, Total;
	Minterm Mint1, Mint2;
	std::vector<Minterm> Temp;																							// Temp é um vetor temporário que guarda os novos mintermos gerados pela redução

	do{																																			// Loop realizado enquanto houverem reduções sendo feitas
		this->Distribute();																										// Distribui os mintermos existentes entre as classes
		Total = 0;																														// Inicializa o contador de reduções em 0

		for( Cls1 = MAX_CLASS - 1 ; Cls1 >= 0 ; Cls1-- ){											// Desde a última classe até a primeira
			for( N1 = 0 ; N1 < (int32_t)Classes[ Cls1 ].size() ; N1++ ){				// Para cada elemento da classe de Cls1
				Mint1 = Classes[ Cls1 ][ N1 ];

				for( Cls2 = Cls1 - 1 ; Cls2 >= 0 ; Cls2-- ){											// Desde a classe anterior a de Cls1 até a primeira classe
					for( N2 = 0 ; N2 < (int32_t)Classes[ Cls2 ].size() ; N2++ ){		// Para cada elemento da classe de Cls2
						Mint2 = Classes[ Cls2 ][ N2 ];

						#ifdef DEBUG
						printf("Comparando [%d][%d]: ", Cls1, N1);		Mint1.Print();		printf(" com [%d][%d]: ", Cls2, N2);		Mint2.Print();			printf("\n");
						printf("  -> Diferença = %d bits.\n", Mint1.Diference( Mint2 ) );
						#endif

						if( Mint1.Diference( Mint2 ) == 1 ){													// Verifica se a diferença entre os mintermos é de 1 bit
							#ifdef DEBUG
							printf("\tGerando intersecção entre mintermos!!\n");
							#endif

							Mint1.State = Mint2.State = COMBINED_STATE;									// Se sim, muda o estado de ambos os mintermos para o estado combinado
							Classes[ Cls1 ][ N1 ] = Mint1;															// Atualiza ambos os mintermos nas classes
							Classes[ Cls2 ][ N2 ] = Mint2;
							Mint2.RemoveDiference( Mint1 );															// Remove a diferença ente os mintermos
							Temp.push_back( Mint2 );																		// E insere o mintermo diferenciado no vetor de novos mintermos

							Total++;																										// Soma o contador de reduções
						}
					}
				}

				if( Mint1.State != COMBINED_STATE ){															// Verifica se o mintermo analisado não foi combinado
					#ifdef DEBUG
					printf("\tMintermo [%d][%d]: estável!!\n", Cls1, N1);
					#endif

					Mint1.State = STABLE_STATE;																			// Caso não tenha sido, seu estado é mudado para Estável
					Classes[ Cls1 ][ N1 ] = Mint1;																	// O mintermo é atualizado no vetor de classes
					Temp.push_back( Mint1 );																				// E o mintermo é inserido diretamente no vetor de novos mintermos
				}
			}
		}

		#ifdef DEBUG
		printf("\nTerminando!!!\nNúmero de Mintermos: %lu\n\n", Temp.size());
		//scanf("%d", &Cls1);
		#endif

		Minterms = Temp;																											// Os mintermos do programa passam a ser os novos mintermos gerados
		this->RemoveRedundance();																							// Mintermos repetidos são removidos
		Temp.erase( Temp.begin(), Temp.end() );																// Reinicia o vetor temporário para uma possível nova iteração do loop

		#ifdef DEBUG
		this->Print();																												// Imprime o resultado final
		#endif
	}while( Total );

}

// ========================================================================== //

// Função básica de leitura de arquivo. Retorna um char* contendo o conteúdo lido
char* ReadFile( FILE* Fl ){
	char *Data = NULL, *Temp = NULL;
	int32_t Nn, Ns;

	if( !Fl )
		return( NULL );

	Nn = Ns = 0;
	while( !feof( Fl ) ){									// Enquanto houverem dados a serem lidos

		while( Nn >= Ns - 1 ){							// Verifica se há espaço em 'Data' para guardar um novo byte de informação
			Ns = ( Ns == 0 ? 1 : Ns * 2 );		// Dobra o tamanho de Data caso seu tamanho seja diferente de 0, caso contrário, seu tamanho passa a ser de 1 byte
			Temp = (char*)malloc( Ns );				// aloca um novo espaço para 'Data'
			if( !Temp ){
				puts( "Erro de alocação!!\n\tEncerrando o programa!!" );
				exit( 0 );
			}

			memset( Temp, 0, Ns );						// Inicializa o novo espaço com 0
			memcpy( Temp, Data, ( Ns >> 1 ) );// Copia os valores antigos de Data
			if( Data )
				free( Data );
			Data = Temp;											// Atribui o novo espaço a 'Data'
		}

		Data[ Nn ] = fgetc( Fl );						// Lê um caracter do arquivo e atribui à ultima posição de 'Data'
		Nn++;																// Incrementa a última posição de 'Data'
	}
	Data[ --Nn ] = '\0';

	return( Data );
}

// ========================================================================== //

int main( int Argc, char** Argv ){
	char *Data = NULL;
	FILE *Fl = NULL;
	McCluskey Mc;

	puts( "Algoritmo de redução de funções booleanas de Quine-McCluskey." );

	if( Argc < 2 ){											// Inicializa o programa recebendo dados da entrada padrão
		puts( "Este programa requer um arquivo de entrada que contenha a função a ser reduzida." );
		puts( "\tModo de uso: Redutor <Nome_do_arquivo>" );
		puts( "\nO programa utilizará a entrada padrão para ler os dados de entrada!!" );

		Fl = stdin;
	}

	else{																// Caso um arquivo tenha sido passado pela linha de comando
		Fl = fopen( Argv[ 1 ], "r" );
		if( !Fl ){
			puts( "Não foi possível ler o arquivo de entrada!!\nEncerrando o programa!!" );
			return( 0 );
		}
	}

	Data = ReadFile( Fl );							// Lê os dados da entrada e armazena em 'Data'
	if( !Data ){
		puts("Não foi possível ler os dados de entrada!!\n\tEncerrando o programa!!");
		return( 0 );
	}
	fclose( Fl );

	Mc.Create( Data );									// Gera os mintermos por meio da variável 'Data'
	free( Data );

	Mc.Print();													// Imprime os dados lidos da entrada
	Mc.Reduce();												// Reduz os mintermos
	Mc.Print();													// Imprime os mintermos reduzidos

	Mc.Free();

	return( 0 );
}

// ========================================================================== //
