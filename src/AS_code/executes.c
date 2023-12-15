#include "executes.h"
#include "aux_executes.h"
#include "requests.h"

void ex_login(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);
    if (!is_login_valid(uid, password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RLI ERR\n");
        return;
    }
    // if (is_logged_in(uid)) {
    //     send_reply_to_user(fd, addr, "user already logged in\n");
    //     return;
    // }
    if (is_registered(uid)) 
        try_to_login(fd, addr, uid, password);
    else 
        register_user(fd, addr, uid, password);
}

void ex_logout(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char login_file_name[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);

    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RLO ERR\n");
    }
    else if (!is_registered(uid)) {
        send_reply_to_user(fd, addr, "RLO UNR\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RLO NOK\n");
    }
    else {
        logout(login_file_name, fd, addr, uid);
    }
}

void ex_unregister(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char login_file_name[100];
    char pass_file_name[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);

    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RUR ERR\n");
    }    
    else if (!is_registered(uid)) {
        send_reply_to_user(fd, addr, "RUR UNR\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RUR NOK\n");
    }
    else {
        unregister(login_file_name, pass_file_name, fd, addr, uid);
    }
}

void ex_myauctions(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    sscanf(request, "%*s %s", uid);

    if (!has_started_auctions(uid)) {
        send_reply_to_user(fd, addr, "RMA NOK\n");
    }
    if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RMA NLG\n");
    }
    else {
        send_myauctions(fd, addr, uid);
    }
}

void ex_mybids(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    sscanf(request, "%*s %s", uid);

    if (!has_placed_bids(uid)) {
        send_reply_to_user(fd, addr, "RMB NOK\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RMB NLG\n");
    }
    else {
        send_mybids(fd, addr, uid);
    }
}

void ex_list(int fd, struct sockaddr_in addr, char *request) {
    char *auctions_list = create_list_auctions(PATH_AUCTIONS_DIR);

    // no auctions was yet started 
    if (auctions_list[0] == '\0') {
        send_reply_to_user(fd, addr, "RLS NOK\n");
    }
    else {
        send_all_auctions(auctions_list, fd, addr);
    }
}

void ex_show_record(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}

void ex_open(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char password[SIZE_PASSWORD+1];
    char name[MAX_NAME_DESC+1];
    char start_value[MAX_START_VAL+1];
    char timeactive[MAX_AUC_DURATION+1];
    char asset_fname[MAX_FILENAME+1];
    long size = 0;
    char *aid = get_aid();

    sscanf(request, "%*s %s %s %s %s %s %s %ld", uid, password, name,
    start_value, timeactive, asset_fname, &size);

    if (strcmp(aid, "000") == 0) {
        send_reply_to_user(fd, addr, "ROA NOK\n"); 
    }
    else if (!is_logged_in(uid) || !is_correct_password(password, uid)) {
        send_reply_to_user(fd, addr, "ROA NLG\n");
    }
    else if (!is_open_valid(name, asset_fname, start_value, timeactive)) {
        send_reply_to_user(fd, addr, "ROA NOK\n");
    }
    else {
        register_auction(fd, addr, uid, name, asset_fname, start_value, timeactive, aid);  
        receive_asset(fd, addr, aid, asset_fname, size);
        sprintf(request, "ROA OK %s\n", aid);
        send_reply_to_user(fd, addr, request);
    }
    free(aid);
}

void ex_close(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char password[SIZE_PASSWORD+1];
    char aid[MAX_STATUS_SIZE+1];
    char dir[100];

    sscanf(request, "%*s %s %s %s", uid, password, aid);
    sprintf(dir, "%s/%s", PATH_AUCTIONS_DIR, aid); 

    if (!directoryExists(dir)) {
        send_reply_to_user(fd, addr, "RCL EAU\n"); 
    }
    else if (!is_correct_password(password, uid) || !is_registered(uid)) {
        send_reply_to_user(fd, addr, "RCL NOK\n"); 
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RCL NLG\n"); 
    }
    else if (!is_auction_owned(uid, aid)) {
        send_reply_to_user(fd, addr, "RCL EOW\n"); 
    }
    else if (!is_auction_active(aid)) {
        send_reply_to_user(fd, addr, "RCL END\n"); 
    }
    else {
        close_auction(aid, 0, time(NULL));
        send_reply_to_user(fd, addr, "RCL OK\n");
    }
}

void ex_show_asset(int fd, struct sockaddr_in addr, char *request) {
    char aid[MAX_STATUS_SIZE+1], directory[100];
    
    sscanf(request, "%*s %s", aid);
    
    sprintf(directory, "%s/%s", PATH_AUCTIONS_DIR, aid);
    if (!directoryExists(directory)) {
        send_reply_to_user(fd, addr, "RSA NOK\n");
    }
    else {
        send_asset(fd, addr, aid);
        send_reply_to_user(fd, addr, "\n");
    }
}

void ex_bid(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char password[SIZE_PASSWORD+1];
    char aid[MAX_STATUS_SIZE+1];
    int value;

    sscanf(request, "%*s %s %s %s %d", uid, password, aid, &value);
    
    if (!is_auction_active(aid)) {
        send_reply_to_user(fd, addr, "RBD NOK\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RBD NLG\n");
    }
    else if (is_auct_hosted_by_user(aid, uid)) {
        send_reply_to_user(fd, addr, "RBD ILG\n");
    }
    else if (is_bid_too_small(aid, value)) {
        send_reply_to_user(fd, addr, "RBD REF\n");
    }
    else {
        create_bid(aid, uid, value);
        send_reply_to_user(fd, addr, "RBD ACC\n");
    }
}