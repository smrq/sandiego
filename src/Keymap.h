#define KEYMAP( \
	KA01, KA02, KA03, KA04, KA05, KA06,             KA09, KA10, KA11, KA12, KA13, KA14, \
	KB01, KB02, KB03, KB04, KB05, KB06, KB07, KB08, KB09, KB10, KB11, KB12, KB13, KB14, \
	KC01, KC02, KC03, KC04, KC05, KC06, KC07, KC08, KC09, KC10, KC11, KC12, KC13, KC14, \
	KD01, KD02, KD03, KD04, KD05, KD06, KD07, KD08, KD09, KD10, KD11, KD12, KD13, KD14, \
	      KE02, KE03, KE04, KE05, KE06, KE07, KE08, KE09, KE10, KE11, KE12, KE13        \
) { \
	{ K_##KA01, K_##KB01, K_##KC01, K_##KD01, K_NULL,   K_NULL,   K_##KB08, K_##KC08, K_##KD08, K_##KE08 }, \
	{ K_##KA02, K_##KB02, K_##KC02, K_##KD02, K_##KE02, K_##KA09, K_##KB09, K_##KC09, K_##KD09, K_##KE09 }, \
	{ K_##KA03, K_##KB03, K_##KC03, K_##KD03, K_##KE03, K_##KA10, K_##KB10, K_##KC10, K_##KD10, K_##KE10 }, \
	{ K_##KA04, K_##KB04, K_##KC04, K_##KD04, K_##KE04, K_##KA11, K_##KB11, K_##KC11, K_##KD11, K_##KE11 }, \
	{ K_##KA05, K_##KB05, K_##KC05, K_##KD05, K_##KE05, K_##KA12, K_##KB12, K_##KC12, K_##KD12, K_##KE12 }, \
	{ K_##KA06, K_##KB06, K_##KC06, K_##KD06, K_##KE06, K_##KA13, K_##KB13, K_##KC13, K_##KD13, K_##KE13 }, \
	{ K_NULL,   K_##KB07, K_##KC07, K_##KD07, K_##KE07, K_##KA14, K_##KB14, K_##KC14, K_##KD14, K_NULL   }, \
}

/*
	{ K_##KA01, K_##KA02, K_##KA03, K_##KA04, K_##KA05, K_##KA06, K_NULL },   \
	{ K_##KB01, K_##KB02, K_##KB03, K_##KB04, K_##KB05, K_##KB06, K_##KB07 }, \
	{ K_##KC01, K_##KC02, K_##KC03, K_##KC04, K_##KC05, K_##KC06, K_##KC07 }, \
	{ K_##KD01, K_##KD02, K_##KD03, K_##KD04, K_##KD05, K_##KD06, K_##KD07 }, \
	{ K_NULL,   K_NULL,   K_##KE03, K_##KE04, K_##KE05, K_##KE06, K_##KE07 }, \
	{ K_NULL,   K_##KA09, K_##KA10, K_##KA11, K_##KA12, K_##KA13, K_NULL },   \
	{ K_##KB08, K_##KB09, K_##KB10, K_##KB11, K_##KB12, K_##KB13, K_##KB14 }, \
	{ K_##KC08, K_##KC09, K_##KC10, K_##KC11, K_##KC12, K_##KC13, K_##KC14 }, \
	{ K_##KD08, K_##KD09, K_##KD10, K_##KD11, K_##KD12, K_##KD13, K_##KD14 }, \
	{ K_##KE08, K_##KE09, K_##KE10, K_##KE11, K_##KE12, K_##KE13, K_NULL },   \
*/
