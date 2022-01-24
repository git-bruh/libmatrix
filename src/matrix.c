#include "matrix-priv.h"

static const char *const errors[MATRIX_CODE_MAX] = {
  [MATRIX_SUCCESS] = "No Error",
  [MATRIX_NOMEM] = "Out Of Memory",
  /* TODO more descriptive codes based on response. */
  [MATRIX_CURL_FAILURE] = "Request Failed",
  [MATRIX_BACKED_OFF] = "Backed Off",
  [MATRIX_MALFORMED_JSON] = "Malformed JSON",
  [MATRIX_INVALID_ARGUMENT] = "Invalid Argument",
  [MATRIX_NOT_LOGGED_IN] = "Not Logged In",
};

int
matrix_global_init(void) {
	return (curl_global_init(CURL_GLOBAL_DEFAULT)) == CURLE_OK ? 0 : -1;
}

struct matrix *
matrix_alloc(const char *mxid, const char *homeserver, void *userp) {
	{
		size_t len_mxid = 0;

		if (!mxid || !homeserver || (len_mxid = strlen(mxid)) < 1
			|| len_mxid > MATRIX_MXID_MAX || (strnlen(homeserver, 1)) < 1) {
			return NULL;
		}
	}

	struct matrix *matrix = malloc(sizeof(*matrix));

	if (matrix) {
		*matrix = (struct matrix) {.ll_mutex = PTHREAD_MUTEX_INITIALIZER,
		  .transfers = matrix_ll_alloc(NULL),
		  .userp = userp};
		if (matrix->transfers
			&& (matrix_set_mxid_homeserver(matrix, mxid, homeserver)) == 0) {
			return matrix;
		}
	}

	matrix_destroy(matrix);
	return NULL;
}

void *
matrix_userp(struct matrix *matrix) {
	return matrix ? matrix->userp : NULL;
}

void
matrix_destroy(struct matrix *matrix) {
	if (!matrix) {
		return;
	}

	matrix_ll_free(matrix->transfers);
	free(matrix->access_token);
	free(matrix->homeserver);
	free(matrix->mxid);
	free(matrix);
}

void
matrix_global_cleanup(void) {
	curl_global_cleanup();
}

int
matrix_logout(struct matrix *matrix) {
	if (matrix) {
		pthread_mutex_lock(&matrix->ll_mutex);
		bool transfers_running = !!matrix->transfers->tail;
		pthread_mutex_unlock(&matrix->ll_mutex);

		if (!transfers_running) {
			free(matrix->access_token);
			matrix->access_token = NULL;

			return 0;
		}
	}

	return -1;
}

int
matrix_get_mxid_homeserver(
  struct matrix *matrix, char **mxid, char **homeserver) {
	if (matrix && mxid && homeserver) {
		*mxid = matrix->mxid;
		*homeserver = matrix->homeserver;

		return 0;
	}

	return -1;
}

int
matrix_set_mxid_homeserver(
  struct matrix *matrix, const char *mxid, const char *homeserver) {
	/* Ensure we haven't logged in. */
	if (matrix && !matrix->access_token && mxid && homeserver) {
		size_t len_mxid = strlen(mxid);

		if (len_mxid < 1 || len_mxid > MATRIX_MXID_MAX
			|| (strnlen(homeserver, 1)) < 1) {
			return -1;
		}

		char *tmp_mxid = strdup(mxid);
		char *tmp_homeserver = strdup(homeserver);

		if (tmp_mxid && tmp_homeserver) {
			free(matrix->mxid);
			free(matrix->homeserver);

			matrix->mxid = tmp_mxid;
			matrix->homeserver = tmp_homeserver;

			return 0;
		}

		free(tmp_mxid);
		free(tmp_homeserver);
	}

	return -1;
}

const char *
matrix_strerror(enum matrix_code code) {
	if (code < 0 || code >= MATRIX_CODE_MAX) {
		return "";
	}

	return errors[code];
}

matrix_json_t *
matrix_json_parse(const char *buf, size_t size) {
	return size ? cJSON_ParseWithLength(buf, size) : cJSON_Parse(buf);
}

char *
matrix_json_print(matrix_json_t *json) {
	return cJSON_Print(json);
}

void
matrix_json_delete(matrix_json_t *json) {
	cJSON_Delete(json);
}
