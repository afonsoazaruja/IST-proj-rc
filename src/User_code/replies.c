#include "replies.h"
#include "commands.h"

void analyze_reply_udp(char *buffer) {
    char type_reply[TYPE_REPLY_SIZE+1];
    char status[MAX_STATUS_SIZE+1];
    if (sscanf(buffer, "%3s %3s", type_reply, status) != 2) {
        sprintf(buffer, "sscanf could not read input\n");
    }  
    else if (!validate_buffer(buffer)) {
        sprintf(buffer, "invalid reply from server\n");
    } else if (strcmp(status, "ERR") == 0) {
        sprintf(buffer, "invalid syntax or values\n");
    } else if (strcmp(type_reply, "RLI") == 0) {
        reply_login(status, buffer);
    } else if (strcmp(type_reply, "RLO") == 0) {
        reply_logout(status, buffer); 
    } else if (strcmp(type_reply, "RUR") == 0) {
        reply_unregister(status, buffer);         
    } 
    else { // cases with list                   
        char *list = (char *)malloc(BUFFER_SIZE);       
        if (list == NULL) {
            perror("Error allocating memory for list"); return;
        }
        sscanf(buffer, "%*s %*s %[^\n]", list);

        if (strcmp(status, "OK") == 0) {
            handle_auctions(list, buffer, type_reply);
        } else if (strcmp(type_reply, "RMA") == 0) { 
            reply_myauctions(status, buffer);
        } else if (strcmp(type_reply, "RMB") == 0) { 
            reply_mybids(status, buffer);
        } else if (strcmp(type_reply, "RLS") == 0) { 
            reply_list(status, buffer);
        } else if (strcmp(type_reply, "RRC") == 0) { 
            reply_show_record(status, buffer, list);
        } else sprintf(buffer, "unknown reply\n");

        free(list);
    }
}

void handle_auctions(char *list, char *buffer, char *type) {
    buffer[0] = '\0';
    if (strcmp(type, "RRC") == 0) {
        char host_uid[MAX_NAME_DESC+1] = "";
        char name[MAX_NAME_DESC+1] = "";
        char asset_fname[MAX_FILENAME+1] = "";
        char start_value[MAX_START_VAL+1] = "";
        char start_date[10+1] = "";
        char start_time[8+1] = "";
        char timeactive[MAX_AUC_DURATION+1] = "";

        if (sscanf(list, "%s %10s %s %s %s %s %s", host_uid, name, asset_fname, start_value, start_date, start_time, timeactive) != 7) {
            perror("Sscanf could not read input"); return;
        }
        snprintf(buffer, BUFFER_SIZE, "┌───\t %s\n│Auction Host: %s\n│Opened in: %s %s\n│Duration: %s seconds\n│Asset Name: %s\n│Starting Bid: %s\n└───\n",
        name, host_uid, start_date, start_time, timeactive, asset_fname, start_value);

        char *token = strtok(list, " ");
        while (token != NULL) {
            if (strcmp(token, "B") == 0) {
                strcat(buffer, "┌\n│Bidder Name: "); 
                token = strtok(NULL, " ");
                strcat(buffer, token);
                strcat(buffer, "\n│Bid Value: ");
                token = strtok(NULL, " ");
                strcat(buffer, token);
                strcat(buffer, "\n│Date: ");
                token = strtok(NULL, " ");
                strcat(buffer, token);
                token = strtok(NULL, " ");
                strcat(buffer, "\n│Time: ");
                strcat(buffer, token);
                strcat(buffer, "\n└\n");
            }
            else if (strcmp(token, "E") == 0) {
                strcat(buffer, "┌───\n│Ended in: ");
                token = strtok(NULL, " ");
                strcat(buffer, token);
                token = strtok(NULL, " ");
                strcat(buffer, " ");
                strcat(buffer, token);
                token = strtok(NULL, " ");
                strcat(buffer, "\n│Timeactive: ");
                strcat(buffer, token);
                strcat(buffer, " seconds\n└───\n");
            }
            token = strtok(NULL, " ");
        }      
    } else {
        char *token = strtok(list, " ");
        if ((strcmp(type, "RLS") == 0) || (strcmp(type, "RMA") == 0) || (strcmp(type, "RMB") == 0)) {
            strcat(buffer, "AID ─ STATE\n");
            while (token != NULL) {
                strcat(buffer, token); // AID
                token = strtok(NULL, " "); // get STATE
                if (token[0] == '1') {
                    strcat(buffer, " ─ ACTIVE\n");
                }
                else if (token[0] == '0') {
                    strcat(buffer, " ─ INACTIVE\n");
                }
                token = strtok(NULL, " ");
            }
            if (strlen(buffer) == 0) sprintf(buffer, "no auction was yet started\n");
        }    
    }
}

void reply_login(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "successful login\n");
        user.logged = true;
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "incorrect login attempt\n");
    } else if (strcmp(status, "REG") == 0) {
        sprintf(buffer, "new user registered\n");
        user.logged = true;
    } 
}

void reply_logout(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "successful logout\n");
        user.logged = false;
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user not logged in\n");
    } else if (strcmp(status, "UNR") == 0) {
        sprintf(buffer, "unknown user\n");
    } 
}

void reply_unregister(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "successful unregister\n");
        user.logged = false;
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "incorrect unregister attempt\n");
    } else if (strcmp(status, "UNR") == 0) {
        sprintf(buffer, "unknown user\n");
    } 
}

void reply_myauctions(char *status, char *buffer) {
    if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user has no ongoing auctions\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    }
}

void reply_mybids(char *status, char *buffer) {
    if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user has not placed any bids\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    } 
}

void reply_list(char *status, char *buffer) {
    if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "no auction was yet started\n");
    } 
}

void reply_show_record(char *status, char *buffer, const char *list) {
    if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "the auction does not exist\n");
    }
}

void analyze_reply_tcp(char *buffer, int fd) {
    char type_reply[4] = "";
    char status[4] = "";
    
    if (read_reply_tcp(buffer, type_reply, fd) == -1) return;
    if (read_reply_tcp(buffer, status, fd) == -1) return;

    if (strcmp(type_reply, "ROA") == 0) { 
       reply_open(status, buffer, fd);
    } else if (strcmp(type_reply, "RCL") == 0) { 
        reply_close(status, buffer);
    } else if (strcmp(type_reply, "RSA") == 0) { 
        reply_show_asset(status, buffer, fd);
    } else if (strcmp(type_reply, "RBD") == 0) { 
        reply_bid(status, buffer);
    } else {
        sprintf(buffer, "unknown reply\n");
    }
}

void reply_open(char *status, char *buffer, int fd) {
    char aid[SIZE_AID+1] = "";
    if (strcmp(status, "OK") == 0) {
        if (read_reply_tcp(buffer, aid, fd) == -1) return;
        sprintf(buffer, "auction successfully created AID %s\n", aid);
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "auction could not be started\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    }
}

void reply_close(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "auction successfully closed\n");
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user does not exist or incorrect password\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    } else if (strcmp(status, "EAU") == 0) {
        sprintf(buffer, "auction AID does not exist\n");
    } else if (strcmp(status, "EOW") == 0) {
        sprintf(buffer, "auction is not owned by user UID\n");
    } else if (strcmp(status, "END") == 0) {
        sprintf(buffer, "auction time already ended\n");
    } 
}

void reply_show_asset(char *status, char *buffer, int fd) {
    if (strcmp(status, "OK") == 0) {
        ssize_t n = 0;
        char asset_dir[14+MAX_FILENAME+1];
        char fname[MAX_FILENAME+1] = "";
        char *endptr = "";
        char fsize[8+1] = ""; // 10*10⁶ (8 digits)
        long size = 0;

        memset(buffer, 0, BUFFER_SIZE);
        if (read_reply_tcp(buffer, fname, fd) == -1) return;
        if (read_reply_tcp(buffer, fsize, fd) == -1) return;

        size = strtol(fsize, &endptr, 10);
        char *data = malloc(size);
        if (data == NULL) {
            perror("Error allocating memory for data"); return;
        }
        sprintf(asset_dir, "%s/%s", SA_DIR, fname);
        FILE *file = fopen(asset_dir, "wb");
        if (file == NULL) {
            perror("Error opening file"); free(data); return;
        }
        do { // read bytes of file and write
            n = read(fd, data, size);
            if(n == -1) {
                perror("Error reading bytes of file"); free(data); fclose(file); return;
            }
            size_t bytes_written = fwrite(data, 1, n, file);
            if (bytes_written != n) {
                perror("Error writing to file"); free(data); fclose(file); return;
            }
            size -= n;
        } while (size != 0);

        sprintf(buffer, "asset transfer: success\n");
        free(data);
        fclose(file);
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "asset transfer: failure\n");
    } 
}

void reply_bid(char *status, char *buffer) {
    if (strcmp(status, "ACC") == 0) {
        sprintf(buffer, "bid accepted\n");
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "auction AID is not active\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    } else if (strcmp(status, "REF") == 0) {
        sprintf(buffer, "a bigger bid was already made.\n");
    } else if (strcmp(status, "ILG") == 0) {
        sprintf(buffer, "you cannot make a bid in an auction hosted by yourself\n");
    } 
}

int read_reply_tcp(char *src, char *dst, int fd) {
    ssize_t n = 0, total = 0;
    memset(src, 0, BUFFER_SIZE);
    while(true) {
        n = recv(fd, src + total, 1, 0);
        if(n==-1) {
            if(errno == EAGAIN) {
                sprintf(src, "Timeout"); return -1;
            }
            else {
                perror("Error in recv"); return -1;
            }  
        }
        if (src[total] == ' ' || 
            src[total] == '\n' || 
            src[total] == '\0') 
            break;
        total += n;
    }
    src[total] = '\0';
    strcpy(dst, src);
    return 0;
}