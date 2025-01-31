#include <sphinxbase/ngram_model.h>
#include <sphinxbase/logmath.h>
#include <sphinxbase/strfuncs.h>
#include <sphinxbase/err.h>

#include "test_macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
test_lm_vals(ngram_model_t *model)
{
	int32 n_used, score;

	TEST_ASSERT(model);
	TEST_EQUAL(ngram_wid(model, "<UNK>"), 0);
	TEST_EQUAL(strcmp(ngram_word(model, 0), "<UNK>"), 0);
	TEST_EQUAL(ngram_wid(model, "absolute"), 13);
	TEST_EQUAL(strcmp(ngram_word(model, 13), "absolute"), 0);
	/* Test unigrams. */
	score = ngram_score(model, "<UNK>", NULL);
	E_INFO("%d\n", score);
	TEST_EQUAL_LOG(score, -75346);
	score = ngram_bg_score(model, ngram_wid(model, "<UNK>"),
			       NGRAM_INVALID_WID, &n_used);
	E_INFO("%d\n", score);
	TEST_EQUAL_LOG(score, -75346);
	TEST_EQUAL(n_used, 1);
	score = ngram_score(model, "sphinxtrain", NULL);
	E_INFO("%d\n", score);
	TEST_EQUAL_LOG(score, -64208);
	TEST_EQUAL_LOG(ngram_bg_score(model, ngram_wid(model, "sphinxtrain"),
				  NGRAM_INVALID_WID, &n_used), -64208);
	TEST_EQUAL(n_used, 1);
	/* Test bigrams. */
	score = ngram_score(model, "huggins", "david", NULL);
	E_INFO("%d\n", score);
	TEST_EQUAL_LOG(score, -831);
	/* Test trigrams. */
	score = ngram_score(model, "daines", "huggins", "david", NULL);
	E_INFO("%d\n", score);
	TEST_EQUAL_LOG(score, -9450);
	return 0;
}

int
main(int argc, char *argv[])
{
	logmath_t *lmath;
	ngram_model_t *model;

	(void)argc;
	(void)argv;
	err_set_loglevel(ERR_INFO);

	/* Initialize a logmath object to pass to ngram_read */
	lmath = logmath_init(1.0001, 0, 0);

	E_INFO("Converting ARPA to BIN\n");
	model = ngram_model_read(NULL, LMDIR "/100.lm.bz2", NGRAM_ARPA, lmath);
	E_INFO("Verifying ARPA\n");
	test_lm_vals(model);
	E_INFO("Writing BIN\n");
	TEST_EQUAL(0, ngram_model_write(model, "100.tmp.lm.bin", NGRAM_BIN));
	ngram_model_free(model);

#ifdef DEBUG_ENDIAN
	E_INFO("Debugging endianness, will not use pre-existing model\n");
#else
	E_INFO("Converting BIN to ARPA\n");
	model = ngram_model_read(NULL, LMDIR "/100.lm.bin", NGRAM_BIN, lmath);
	test_lm_vals(model);
	TEST_EQUAL(0, ngram_model_write(model, "100.tmp.lm", NGRAM_ARPA));
	ngram_model_free(model);
#endif

	E_INFO("Testing converted BIN\n");
	model = ngram_model_read(NULL, "100.tmp.lm.bin", NGRAM_BIN, lmath);
	test_lm_vals(model);
#ifdef DEBUG_ENDIAN
	TEST_EQUAL(0, ngram_model_write(model, "100.tmp.lm", NGRAM_ARPA));
#endif
	ngram_model_free(model);

	E_INFO("Testing converted ARPA\n");
	model = ngram_model_read(NULL, "100.tmp.lm", NGRAM_ARPA, lmath);
	test_lm_vals(model);
	ngram_model_free(model);

	E_INFO("Converting ARPA back to BIN\n");
	model = ngram_model_read(NULL, "100.tmp.lm", NGRAM_ARPA, lmath);
	test_lm_vals(model);
	TEST_EQUAL(0, ngram_model_write(model, "100.tmp.lm.bin", NGRAM_BIN));
	ngram_model_free(model);

	E_INFO("Converting BIN back to ARPA\n");
	model = ngram_model_read(NULL, "100.tmp.lm.bin", NGRAM_BIN, lmath);
	test_lm_vals(model);
	TEST_EQUAL(0, ngram_model_write(model, "100.tmp.lm", NGRAM_ARPA));
	ngram_model_free(model);

	E_INFO("Converting unigram ARPA to BIN\n");
	model = ngram_model_read(NULL, LMDIR "/turtle.ug.lm", NGRAM_ARPA, lmath);
	TEST_EQUAL(0, ngram_model_write(model, "turtle.ug.tmp.lm.bin", NGRAM_BIN));
	ngram_model_free(model);

	logmath_free(lmath);
	return 0;
}
