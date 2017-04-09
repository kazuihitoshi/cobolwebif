typedef struct {
  char lockname [80];
  char l;
  int  mode ;
  int  id;
} T_WEB_LOCK;

#define LOCK_MODE_EXCLUSIVE 1
#define LOCK_MODE_SHARE     0

typedef struct {
  char name [80];
  char name_stop;
  char value[1024];
  char value_stop;
  char expires[80];
  char expires_stop;
  char path[80];
  char path_stop;
  char domain[80];
  char domain_stop;
  char secure[6];
  char secure_stop;
} T_WEB_COOKIE;

typedef struct web_cookie_list {
  char *name;
  char *value;
  char *expires;
  char *path;
  char *domain;
  char *secure;
  struct web_cookie_list *next;
} T_WEB_COOKIE_LIST;
