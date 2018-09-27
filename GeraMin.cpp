/*

	EM CONSTRUÇÂO!!!

*/

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <unistd.h>

// ========================================================================== //

#define MAX_POS						127
#define MAX_MINTERMS			512
#define MAX_NAMES					16
#define DEFAULT_FILE			"Minterms.min"

// ========================================================================== //

int main( int Argc, char **Argv ){
	uint32_t Minterms, MaxPos, MaxNames;
	char Name, *FileName = NULL;
	uint32_t Pos, Val, Nn, Terms, MaxTerms;
	FILE *Fl = NULL;

	MaxPos = MAX_POS;
	MaxNames = MAX_NAMES;
	Minterms = MAX_MINTERMS;
	FileName = (char*)DEFAULT_FILE;
	MaxTerms = MAX_POS;

	for( Nn = 0 ; (int)Nn < Argc ; Nn++ ){
		if( ( !strcmp( Argv[ Nn ], "-p" ) ) || ( !strcmp( Argv[ Nn ], "-pos" ) ) ){
			Nn++;
			if( Argv[ Nn ] )
				MaxPos = atoi( Argv[ Nn ] );
		}

		if( ( !strcmp( Argv[ Nn ], "-n" ) ) || ( !strcmp( Argv[ Nn ], "-name" ) ) ){
			Nn++;
			if( Argv[ Nn ] )
				MaxNames = atoi( Argv[ Nn ] );
		}

		if( ( !strcmp( Argv[ Nn ], "-f" ) ) || ( !strcmp( Argv[ Nn ], "-file" ) ) ){
			Nn++;
			if( Argv[ Nn ] )
				FileName = Argv[ Nn ];
		}

		if( ( !strcmp( Argv[ Nn ], "-t" ) ) || ( !strcmp( Argv[ Nn ], "-terms" ) ) ){
			Nn++;
			if( Argv[ Nn ] )
				MaxTerms = atoi( Argv[ Nn ] );
		}

		if( ( !strcmp( Argv[ Nn ], "-m" ) ) || ( !strcmp( Argv[ Nn ], "-minterms" ) ) ){
			Nn++;
			if( Argv[ Nn ] )
				Minterms = atoi( Argv[ Nn ] );
		}
	}

	printf( "Pos: %d, Names: %d, Min: %d", MaxPos, MaxNames, Minterms );

	Fl = fopen( FileName, "w" );
	if( !Fl ){
		puts( "ERRO AO ABRIR ARQUIVO!!!" );
		return 1;
	}

	for( ; Minterms ; Minterms-- ){
		Terms = rand() % ( MaxTerms + 1 );
		for( ; Terms ; Terms-- ){
			Pos = rand() % MaxPos;
			Val = rand() % 2;
			Name = 'a' + ( rand() % (int)MaxNames );
			if( ( Name > 'Z' ) && ( Name < 'a' ) )
				Name = 'a' + ( Name - 'Z' ) ;

			if( Val )
				fprintf( Fl, "%c%d", Name, Pos );
			else
				fprintf( Fl, "!%c%d", Name, Pos );

			if( Terms > 1 )
				fprintf( Fl, "." );
		}

		if( Minterms > 1 )
			fprintf( Fl, " + " );
	}

	return 0;
}

// ========================================================================== //
