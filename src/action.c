#include <fcgi_stdio.h> /* FCGI_Accept FCGI_SetExitStatus FCGI_Finish */
#include <cgi.h> /* s_cgi cgiInit cgiGetValue cgiFree */
#include <unistd.h> /* sleep */
#include <postgresql/libpq-fe.h> /* PGconn PGresult PQconnectdb PQexecParams PQgetvalue PQclear PQfinish */
#include <string.h> /* strcmp */
#include <sys/types.h>
#include <regex.h> /* regex_t regcomp regerror regexec regfree */
#include <stdlib.h> /* exit */

#define dumpstr( var ) ( ( NULL != var ) && printf( #var" = %s\n", var ) )

#define NEW_CREATE_SQL     "select new_create_request( $1::text || '@bonito.pt', $2::text )"
#define NEW_CREATE_CODE_OK              0
#define NEW_CREATE_CODE_VIRTUAL_EXISTS -1
#define NEW_CREATE_CODE_REAL_EXISTS    -2
#define NEW_CREATE_CODE_BUG            -3

#define CONFIRM_CREATE_SQL "select confirm_create_request( $1::text )"
#define CONFIRM_CREATE_CODE_OK               0
#define CONFIRM_CREATE_CODE_TOKEN_VALIDATED -1
#define CONFIRM_CREATE_CODE_TOKEN_INVALID   -2
#define CONFIRM_CREATE_CODE_BUG             -3

#define DEFAULT_SQL_CODE -4

#define UNKNOWN_ACTION -1
#define NO_ACTION      0
#define CREATE_ACTION  1
#define CONFIRM_ACTION 2
int decode_action( char *action ) {
	if( NULL == action ) {
		return NO_ACTION;
	} else if( !strcmp( "create", action ) ) {
		return CREATE_ACTION;
	} else if( !strcmp( "confirm", action ) ) {
		return CONFIRM_ACTION;
	}
	return UNKNOWN_ACTION;
}

#define ERROR_SIZE 256
void handle_regcomp_error( int res, regex_t *re ) {
	char error[ ERROR_SIZE ] = { '\0' };
	if( 0 != res ) {
		fprintf( stderr, "Aborting on regcomp() ...\n" );
		regerror( res, re, error, ERROR_SIZE );
		fprintf( stderr, "Error: %s\n", error );
		exit( 1 );
	}
}

/* TODO: Review all regular expressions! */
#define VIRTUAL_RE "^[[:alpha:]][[:alnum:]\\._-]{2,40}$"
#define REAL_RE    "^[[:alnum:]\\._-]{3,40}@[[:alnum:]][[:alnum:]\\.-]+\\.[[:alpha:]]{2,4}$"
#define TOKEN_RE   "^[[:xdigit:]]{40}$"

int main() {
	/* Database variables. */
	PGconn *db = NULL;
	PGresult *res = NULL;
	char *params[ 2 ];
	int ret = DEFAULT_SQL_CODE;
	/* CGI variable. */
	s_cgi *cgi = NULL;
	/* Input parameters. */
	char *action = NULL;
	char *virtual = NULL;
	char *real = NULL;
	char *token = NULL;
	/* Regular expression variables. */
	regex_t virtual_re;
	regex_t real_re;
	regex_t token_re;
	/* Auxiliar variables. */
	int req = 0;

	/* Compile the regular expressions. */
	handle_regcomp_error( regcomp( & virtual_re, VIRTUAL_RE, REG_EXTENDED | REG_NOSUB ), & virtual_re );
	handle_regcomp_error( regcomp( & real_re   , REAL_RE   , REG_EXTENDED | REG_NOSUB ), & real_re    );
	handle_regcomp_error( regcomp( & token_re  , TOKEN_RE  , REG_EXTENDED | REG_NOSUB ), & token_re   );
	/* Connect to the database. */
	db = PQconnectdb( "host=localhost"\
			" user=bonitomail"\
			" password='my long password"\
			" dbname=bonitomail" );

	/* FastCGI request handling cycle. */
	while( req < 100 && FCGI_Accept() >= 0 ) {

		/* Start up CGI environment. */
		cgi = cgiInit();

		/* Reset SQL return code variable to error state. */
		ret = DEFAULT_SQL_CODE;

		/* Try to fetch the input parameters. */
		action  = cgiGetValue( cgi, "action" );
		virtual = cgiGetValue( cgi, "virtual" );
		real    = cgiGetValue( cgi, "real" );
		token   = cgiGetValue( cgi, "token" );

		/* Decide what to do based on the requested action. */
		switch( decode_action( action ) ) {

			case CREATE_ACTION :
				/* Check the real and virtual inputs. */
				if( NULL != real && NULL != virtual
					&& !regexec( & real_re   , real   , 0, NULL, 0 )
					&& !regexec( & virtual_re, virtual, 0, NULL, 0 ) ) {
					params[ 0 ] = virtual;
					params[ 1 ] = real;

					/* Register the alias creation request. */
					res = PQexecParams( db,	NEW_CREATE_SQL,
						2, NULL, params, NULL, NULL, 0 );
					ret = atoi( PQgetvalue( res, 0, 0 ) );
					PQclear( res );
				}

				/* Send the client to the next step page, on success. */
				switch( ret ) {
					case NEW_CREATE_CODE_OK :
						printf( "Status: 303 See Other\r\n" );
						printf( "Location: http://bonito.pt/confirma.html\r\n" );
						printf( "\r\n" );
					case NEW_CREATE_CODE_VIRTUAL_EXISTS :
					case NEW_CREATE_CODE_REAL_EXISTS :
					case NEW_CREATE_CODE_BUG :
					case DEFAULT_SQL_CODE :
					default :
						printf( "Status: 204 No Content\r\n" );
						printf( "\r\n" );
				}
				break;

			case CONFIRM_ACTION :
				/* Check the token input. */
				if( NULL != token
					&& !regexec( & token_re, token, 0, NULL, 0 ) ) {
					params[ 0 ] = token;

					/* Register the request confirmation. */
					res = PQexecParams( db,	CONFIRM_CREATE_SQL,
						1, NULL, params, NULL, NULL, 0 );
					ret = atoi( PQgetvalue( res, 0, 0 ) );
					PQclear( res );
				}

				/* Send the client to the next step page, on success. */
				switch( ret ) {
					case CONFIRM_CREATE_CODE_OK :
						printf( "Status: 303 See Other\r\n" );
						printf( "Location: http://bonito.pt/confirmado.html\r\n" );
						printf( "\r\n" );
					case CONFIRM_CREATE_CODE_TOKEN_VALIDATED :
					case CONFIRM_CREATE_CODE_TOKEN_INVALID :
					case CONFIRM_CREATE_CODE_BUG :
					case DEFAULT_SQL_CODE :
					default :
						printf( "Status: 204 No Content\r\n" );
						printf( "\r\n" );
				}
				break;

			case UNKNOWN_ACTION :
			case NO_ACTION :
			default :
				/* Send HTTP content type header. */
				printf( "Content-Type: text/plain; charset=UTF-8\r\n" );
				printf( "Request: %d\r\n", req );
				printf( "\r\n" );

				/* Send result. */
				dumpstr( action );
				dumpstr( virtual );
				dumpstr( real );
				dumpstr( token );

				break;
		}


		/* Clean up CGI environment. */
		cgiFree( cgi );
		req++;

		FCGI_SetExitStatus( 0 );
		FCGI_Finish();
	}

	/* Disconnet from the database. */
	PQfinish( db );

	/* Clean regular expression variables. */
	regfree( & virtual_re );
	regfree( & real_re    );
	regfree( & token_re   );

	return 0;
}
