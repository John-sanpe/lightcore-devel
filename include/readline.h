
struct readline_state {
    readline_read_t read;
    readline_write_t write;
    void *data;

    const char *prompt;
    unsigned int plen;

    char *buff;
    unsigned int len;
    unsigned int pos;
    unsigned int bsize;
    bool keylock;
    char esc_param;
    enum readline_esc esc_state;

    const char *workspace;
    unsigned int worklen;
    unsigned int worksize;

    const char *clipbrd;
    unsigned int cliplen;
    unsigned int clippos;
    unsigned int clipsize;
    bool clipview;

    struct list_head history;
    struct readline_history *curr;
};
