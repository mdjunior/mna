#define MAX_LINHA 80
#define MAX_NOME 11 // used in utils.h
#define MAX_ELEM 50
#define MAX_NOS 50 // used in utils.h
#define MAX_FILENAME 50
#define TOLG 1e-9
#define DEBUG

// defines para sscanf
#define FORMAT_ONE_STRING(S) "%" #S "s"
#define RESOLVE_ONE_STRING(S) FORMAT_ONE_STRING(S)
#define FORMAT_TWO_TERMINALS(S) "%" #S "s%" #S "s%lg"
#define RESOLVE_TWO_TERMINALS(S) FORMAT_TWO_TERMINALS(S)
#define FORMAT_FOUR_TERMINALS(S) "%" #S "s%" #S "s%" #S "s%" #S "s%lg"
#define RESOLVE_FOUR_TERMINALS(S) FORMAT_FOUR_TERMINALS(S)
#define FORMAT_AMPOP(S) "%" #S "s%" #S "s%" #S "s%" #S "s"
#define RESOLVE_AMPOP(S) FORMAT_AMPOP(S)

#define FORMAT_SOURCE(S) "%" #S "s%" #S "s%" #S "s"
#define RESOLVE_SOURCE(S) FORMAT_SOURCE(S)

#define FORMAT_SOURCE_DC(S) "%" #S "s%" #S "s DC %lg"
#define RESOLVE_SOURCE_DC(S) FORMAT_SOURCE_DC(S)

#define FORMAT_SOURCE_SIN(S) "%" #S "s%" #S "s SIN %lg %lg %lg %lg %lg %lg %lg"
#define RESOLVE_SOURCE_SIN(S) FORMAT_SOURCE_SIN(S)

#define FORMAT_SOURCE_PULSE(S) "%" #S "s%" #S "s PULSE %lg %lg %lg %lg %lg %lg %lg %lg"
#define RESOLVE_SOURCE_PULSE(S) FORMAT_SOURCE_PULSE(S)

#define FORMAT_DIODE(S) "%" #S "s%" #S "s"
#define RESOLVE_DIODE(S) FORMAT_DIODE(S)

#define RESOLVE_ANALISYS "%lg%lg%s%lg"

#define EXCEEDED_MAX_ELEMENTS 1
#define EXCEEDED_MAX_NOME 2
#define EXCEEDED_MAX_NOS 3
#define UNKNOWN_ELEMENT 4
#define UNKNOWN_SOURCE 5
#define INCORRECT_ANALYSIS_SETUP 6
#define SPECIAL_LINE 7