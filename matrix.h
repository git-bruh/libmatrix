#ifndef MATRIX_MATRIX_H
#define MATRIX_MATRIX_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Must allocate enum + 1. */
enum matrix_limits {
	MATRIX_MXID_MAX = 255,
};

enum matrix_code {
	MATRIX_SUCCESS = 0,
	MATRIX_NOMEM,
	MATRIX_CURL_FAILURE,
	MATRIX_BACKED_OFF,
	MATRIX_MALFORMED_JSON,
	MATRIX_INVALID_ARGUMENT,
	MATRIX_NOT_LOGGED_IN,
};

struct matrix;

typedef struct cJSON matrix_json_t;

/* Members of all structs are non-nullable unless explicitly mentioned. */
/* The "base" members of all event structs. */
struct matrix_state_base {
	char *event_id;
	char *sender;
	char *type;
	char *state_key;
	uint64_t origin_server_ts;
};

struct matrix_room_base {
	char *event_id;
	char *sender;
	char *type;
	uint64_t origin_server_ts;
};

struct matrix_ephemeral_base {
	char *type;
	char *room_id;
};

struct matrix_file_info {
	int size;
	char *mimetype; /* nullable. */
};

struct matrix_room_typing {
	matrix_json_t *user_ids;
};

struct matrix_room_canonical_alias {
	char *alias; /* nullable. */
};

struct matrix_room_create {
	bool federate;
	char *creator;
	const char
	  *room_version; /* This is marked const as we assign a string literal to
						it if the room_version key is not present. */
};

struct matrix_room_join_rules {
	char *join_rule;
};

struct matrix_room_member {
	bool is_direct;
	char *membership;
	char *prev_membership; /* nullable. */
	char *avatar_url;	   /* nullable. */
	char *displayname;	   /* nullable. */
};

struct matrix_room_power_levels {
	int ban;
	int events_default;
	int invite;
	int kick;
	int redact;
	int state_default;
	int users_default;
	matrix_json_t *events;		  /* nullable. */
	matrix_json_t *users;		  /* nullable. */
	matrix_json_t *notifications; /* nullable. */
};

struct matrix_room_name {
	char *name;
};

struct matrix_room_topic {
	char *topic;
};

struct matrix_room_avatar {
	char *url;
	struct matrix_file_info info;
};

struct matrix_unknown_state {
	matrix_json_t *content;
	matrix_json_t *prev_content; /* nullable. */
};

struct matrix_room_message {
	char *body;
	char *msgtype;
	char *format;		  /* nullable. */
	char *formatted_body; /* nullable. */
};

struct matrix_room_redaction {
	char *redacts;
	char *reason; /* nullable. */
};

struct matrix_room_attachment {
	char *body;
	char *msgtype;
	char *url; /* nullable. */
	char *filename;
	struct matrix_file_info info;
};

struct matrix_room_summary {
	int joined_member_count;
	int invited_member_count;
	matrix_json_t *heroes; /* TODO somehow abstract the cJSON object away:
					  1. Make an iterator with (void *)
					  2. Dump raw JSON as (char *)
					  3. Maybe just don't */
};

struct matrix_room_timeline {
	char *prev_batch;
	bool limited;
};

enum matrix_event_type {
	/* TODO account_data */
	MATRIX_EVENT_STATE = 0,
	MATRIX_EVENT_TIMELINE,
	MATRIX_EVENT_EPHEMERAL,
	MATRIX_EVENT_MAX,
};

struct matrix_room {
	char *id;
	matrix_json_t *events[MATRIX_EVENT_MAX];
	struct matrix_room_summary summary;
	struct matrix_room_timeline
	  timeline; /* Irrelevant if type == MATRIX_ROOM_INVITE. */
	enum matrix_room_type {
		MATRIX_ROOM_LEAVE = 0,
		MATRIX_ROOM_JOIN,
		MATRIX_ROOM_INVITE,
		MATRIX_ROOM_MAX
	} type;
};

struct matrix_sync_response {
	char *next_batch;
	matrix_json_t *rooms[MATRIX_ROOM_MAX];
	/* struct matrix_account_data_events account_data; */
};

struct matrix_state_event {
	enum matrix_state_type {
		MATRIX_ROOM_MEMBER = 0,
		MATRIX_ROOM_POWER_LEVELS,
		MATRIX_ROOM_CANONICAL_ALIAS,
		MATRIX_ROOM_CREATE,
		MATRIX_ROOM_JOIN_RULES,
		MATRIX_ROOM_NAME,
		MATRIX_ROOM_TOPIC,
		MATRIX_ROOM_AVATAR,
		MATRIX_ROOM_UNKNOWN_STATE,
	} type;
	struct matrix_state_base base;
	char *state_key;
	union {
		struct matrix_room_member member;
		struct matrix_room_power_levels power_levels;
		struct matrix_room_canonical_alias canonical_alias;
		struct matrix_room_create create;
		struct matrix_room_join_rules join_rules;
		struct matrix_room_name name;
		struct matrix_room_topic topic;
		struct matrix_room_avatar avatar;
		struct matrix_unknown_state unknown_state;
	};
};

struct matrix_timeline_event {
	enum matrix_timeline_type {
		MATRIX_ROOM_MESSAGE = 0,
		MATRIX_ROOM_REDACTION,
		MATRIX_ROOM_ATTACHMENT,
	} type;
	struct matrix_room_base base;
	union {
		struct matrix_room_message message;
		struct matrix_room_redaction redaction;
		struct matrix_room_attachment attachment;
	};
};

struct matrix_ephemeral_event {
	enum matrix_ephemeral_type {
		MATRIX_ROOM_TYPING = 0,
	} type;
	struct matrix_ephemeral_base base;
	union {
		struct matrix_room_typing typing;
	};
};

struct matrix_sync_event {
	enum matrix_event_type type;
	matrix_json_t *json;
	union {
		struct matrix_state_event state;
		struct matrix_timeline_event timeline;
		struct matrix_ephemeral_event ephemeral;
	};
};

struct matrix_sync_callbacks {
	/* Called on each successful sync response. */
	void (*sync_cb)(struct matrix *, struct matrix_sync_response *);
	/* Optional, called once for each failed sync request, makes the sync
	 * function wait for the returned amount of milliseconds before retrying the
	 * request. Returning a negative value breaks the sync loop. */
	int (*backoff_cb)(struct matrix *);
	/* Optional, called once after the first successful request after a failed
	 * one. Can be used to reset the internal timeout. */
	void (*backoff_reset_cb)(struct matrix *);
};

/* Functions returning int (Except enums) return -1 on failure and 0 on success.
 * Functions returning pointers return NULL on failure. */

/* ALLOC/DESTROY */

/* Must be the first function called only a single time. */
int
matrix_global_init(void);
struct matrix *
matrix_alloc(const char *mxid, const char *homeserver, void *userp);
/* Get the user pointer set in matrix_alloc(). */
void *
matrix_userp(struct matrix *matrix);
void
matrix_destroy(struct matrix *matrix);
/* Must be the last function called only a single time. */
void
matrix_global_cleanup(void);

/* SYNC */

enum matrix_code
matrix_login_with_token(struct matrix *matrix, const char *access_token);
/* nullable: device_id, initial_device_display_name */
enum matrix_code
matrix_login(struct matrix *matrix, const char *password, const char *device_id,
  const char *initial_device_display_name, char **access_token);

/* timeout specifies the maximum time in milliseconds that the server will wait
 * for events to be received. The recommended minimum is 1000 == 1 second to
 * avoid burning CPU cycles.
 * nullable: next_batch, should_stop */
enum matrix_code
matrix_sync_forever(struct matrix *matrix, const char *next_batch,
  unsigned timeout, struct matrix_sync_callbacks callbacks);

/* Fill in the passed struct with the current room. */
int
matrix_sync_room_next(
  struct matrix_sync_response *response, struct matrix_room *room);
/* Fill in the passed struct with the corresponding JSON item's
 * representation at the current index. Items must be accessed based on the
 * value of revent->type. */
int
matrix_sync_event_next(
  struct matrix_room *room, struct matrix_sync_event *revent);

/* Individual parsing functions (from JSON). Can be used if storing JSON in
 * database. JSON is obtained with matrix_json_parse. Items must be accessed
 * based on the value of revent->type. */
int
matrix_event_state_parse(
  struct matrix_state_event *revent, const matrix_json_t *event);
int
matrix_event_timeline_parse(
  struct matrix_timeline_event *revent, const matrix_json_t *event);
int
matrix_event_ephemeral_parse(
  struct matrix_ephemeral_event *revent, const matrix_json_t *event);

/* MISC */

/* size is length of buf. If 0, size is calculated automatically. */
matrix_json_t *
matrix_json_parse(const char *buf, size_t size);
char *
matrix_json_print(matrix_json_t *json);
void
matrix_json_delete(matrix_json_t *json);

/* API */

/* Cancel all ongoing requests from any thread (including syncing). */
void
matrix_cancel(struct matrix *matrix);
enum matrix_code
matrix_send_message(struct matrix *matrix, char **event_id, const char *room_id,
  const char *msgtype, const char *body, const char *formatted_body);
#endif /* !MATRIX_MATRIX_H */
