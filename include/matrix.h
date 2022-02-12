#ifndef MATRIX_MATRIX_H
#define MATRIX_MATRIX_H
#ifdef __cplusplus
extern "C" {
#endif
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
	MATRIX_CODE_MAX
};

struct matrix;

typedef struct cJSON matrix_json_t;

/* Members of all structs are non-nullable unless explicitly mentioned and the
 * .content_was_empty member is set to false (If state).
 * For state events, the members will be null if .content_was_empty is set
 * to true. It might indicate a redacted state event. */
/* The "base" members of all event structs. */
struct matrix_state_base {
	/* true if all fields of the actual event are invalid. The members in the
	 * base struct (this one) are guaranteed to be valid either way. */
	bool content_was_empty;
	char *event_id;
	char *sender;
	char *type;
	char *state_key; /* Not nullable, but can be zero-length */
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
	struct {
		char *event_id; /* nullable. */
		char *room_id;	/* nullable. */
	} predecessor;
	const char *room_version;
	char *type; /* nullable. */
};

struct matrix_room_join_rules {
	char *join_rule;
};

struct matrix_room_member {
	bool is_direct;
	char *membership;
	char *avatar_url;  /* nullable. */
	char *displayname; /* nullable. */
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

struct matrix_room_space_child {
	bool suggested;
	char *order;		/* nullable. */
	matrix_json_t *via; /* nullable. */
};

struct matrix_room_space_parent {
	bool canonical;
	matrix_json_t *via; /* nullable. */
};

struct matrix_unknown_state {
	const matrix_json_t *content;
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
	char *url;
	char *filename; /* nullable. */
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

enum matrix_room_type {
	MATRIX_ROOM_LEAVE = 0,
	MATRIX_ROOM_JOIN,
	MATRIX_ROOM_INVITE,
	MATRIX_ROOM_MAX
};

struct matrix_room {
	char *id;
	matrix_json_t *events[MATRIX_EVENT_MAX];
	struct matrix_room_summary summary;
	struct matrix_room_timeline
	  timeline; /* Irrelevant if type == MATRIX_ROOM_INVITE. */
	enum matrix_room_type type;
};

struct matrix_sync_response {
	char *next_batch;
	matrix_json_t *rooms[MATRIX_ROOM_MAX];
	/* struct matrix_account_data_events account_data; */
};

union matrix_state_event_content {
	struct matrix_room_member member;
	struct matrix_room_power_levels power_levels;
	struct matrix_room_canonical_alias canonical_alias;
	struct matrix_room_create create;
	struct matrix_room_join_rules join_rules;
	struct matrix_room_name name;
	struct matrix_room_topic topic;
	struct matrix_room_avatar avatar;
	struct matrix_room_space_child space_child;
	struct matrix_room_space_parent space_parent;
	struct matrix_unknown_state unknown;
};

enum matrix_state_type {
	MATRIX_ROOM_MEMBER = 1 << 1,
	MATRIX_ROOM_POWER_LEVELS = 1 << 2,
	MATRIX_ROOM_CANONICAL_ALIAS = 1 << 3,
	MATRIX_ROOM_CREATE = 1 << 4,
	MATRIX_ROOM_JOIN_RULES = 1 << 5,
	MATRIX_ROOM_NAME = 1 << 6,
	MATRIX_ROOM_TOPIC = 1 << 7,
	MATRIX_ROOM_AVATAR = 1 << 8,
	MATRIX_ROOM_SPACE_CHILD = 1 << 9,
	MATRIX_ROOM_SPACE_PARENT = 1 << 10,
	MATRIX_ROOM_UNKNOWN_STATE = 1 << 11,
};

struct matrix_state_event {
	/* Whether the state event was received in the sync timeline. If true, the
	 * state event should be shown in the client's UI along with other timeline
	 * events. */
	bool is_in_timeline;
	/* Whether the member in prev_content is valid. */
	bool prev_content_is_valid;
	enum matrix_state_type type;
	char *replaces_state; /* nullable (ID of replaced event) */
	struct matrix_state_base base;
	union matrix_state_event_content content;
	union matrix_state_event_content prev_content;
};

enum matrix_rel_type {
	MATRIX_RELATION_UNKNOWN = 0,
	MATRIX_RELATION_ANNOTATION,
	MATRIX_RELATION_IN_REPLY_TO,
	MATRIX_RELATION_REPLACE,
};

struct matrix_event_relation {
	enum matrix_rel_type rel_type;
	char *rel_type_str; /* nullable */
	char *event_id;		/* nullable */
	char *key;			/* nullable (Annotation key) */
	matrix_json_t
	  *new_content; /* nullable, the event struct can be passed to () */
};

enum matrix_timeline_type {
	MATRIX_ROOM_MESSAGE = 1 << 1,
	MATRIX_ROOM_REDACTION = 1 << 2,
	MATRIX_ROOM_ATTACHMENT = 1 << 3,
};

struct matrix_timeline_event {
	enum matrix_timeline_type type;
	struct matrix_room_base base;
	struct matrix_event_relation relation;
	union {
		struct matrix_room_message message;
		struct matrix_room_redaction redaction;
		struct matrix_room_attachment attachment;
	};
};

enum matrix_ephemeral_type {
	MATRIX_ROOM_TYPING = 1 << 1,
};

struct matrix_ephemeral_event {
	enum matrix_ephemeral_type type;
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
 * nullable: next_batch */
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
/* Only for state/timeline events. If -1 is returned, the contents of revent
 * will be invalid, but revent->type will be set to the most relevant type
 * depending on whether "state_key" was present or not. */
int
matrix_event_sync_parse(
  struct matrix_sync_event *revent, const matrix_json_t *event);
const char *
matrix_sync_event_id(struct matrix_sync_event *event);

/* MISC */

/* No transfers should be running. */
int
matrix_logout(struct matrix *matrix);
/* mxid/homeserver must not be modified. */
int
matrix_get_mxid_homeserver(
  struct matrix *matrix, char **mxid, char **homeserver);
/* Change MXID/Homesever before logging in. */
int
matrix_set_mxid_homeserver(
  struct matrix *matrix, const char *mxid, const char *homeserver);

const char *
matrix_strerror(enum matrix_code code);
/* size is length of buf. If 0, size is calculated automatically. */
matrix_json_t *
matrix_json_parse(const char *buf, size_t size);
char *
matrix_json_print(matrix_json_t *json);
/* Check if the event has a content key. */
bool
matrix_json_has_content(const matrix_json_t *json);
/* Clear the content key of an event if it exists. If it was called on a
 * matrix_sync_event, the event must be discarded unless you like
 * use-after-frees. */
int
matrix_json_clear_content(matrix_json_t *json);
void
matrix_json_delete(matrix_json_t *json);

/* API */

/* Cancel all ongoing requests from any thread (including syncing). */
void
matrix_cancel(struct matrix *matrix);
enum matrix_code
matrix_send_message(struct matrix *matrix, char **event_id, const char *room_id,
  const char *msgtype, const char *body, const char *formatted_body);
#ifdef __cplusplus
}
#endif
#endif /* !MATRIX_MATRIX_H */
