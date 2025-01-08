#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "instruction.h"

#define DEBUG 0

typedef struct{
    char* label;
    int val;
    int offset;
} label_t;

int check_for_label(char** args, label_t* label_arr, int num){
    bool label_equal, label_not_null;
    for (int i = 0; i < 3; i++){
        if (args[i] != NULL){
            for (int j = 0; j < num; j++){
                label_equal = (strcmp(args[i], label_arr[j].label) == 0);
                label_not_null = (label_arr[j].label != NULL);
                if (label_equal && label_not_null){
                    sprintf(args[i], "$%d", label_arr[j].val);
                }
            }
        }
    }
    return 0;
}
void usage(){
    // fprintf(stderr, "Usage: ./xas file");
    exit(1);
}
void invalid_instruction(){
    printf("Invalid inst usage\n");
    exit(2);
}
void check_args(char** args){
    for (int i = 0; i < 3; i++){
        if (args[i] != NULL) {
            char first = args[i][0];
            int len = strlen(args[i]);
            if (first != '$' || first != '%' || len == 1) {
                invalid_instruction();
            }
        }
    }
}
void print_command(char* command){
    if (DEBUG) printf("%s COMMAND FOUND\n", command);
}
int main(int argc, char** argv) {
    if (argc != 2) {
        usage();
        return 1;
    }
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        // perror("fopen");
        return 1;
    }
    int no_of_lines = 0;
    int no_of_labels = 0;
    while (true) {
        char line[100];
        char* ret = fgets(line, 100, file);
        if (ret == NULL) {
            break;
        }
        if (line[0] != '\n'){
            no_of_lines++;
        }
        if (line[strlen(line) - 2] == ':' && line[0] != '#'){
            no_of_labels++;
        }
        if (DEBUG) printf("%s", line);
    }
    if (DEBUG) printf("Number of lines: %d\n", no_of_lines);
    char** lines = malloc(no_of_lines * sizeof(char*));
    rewind(file);
    label_t* label_arr = calloc(no_of_labels, sizeof(label_t));
    int label_no = 0;
    int ins_no = 0;
    for (int i = 0; i < no_of_lines; i++) {
        char line[100];
        char* ret = fgets(line, 100, file);
        // char* cmd = strtok(line, ":");
        // printf("cmd: %s\n", cmd);
        if (ret == NULL) {
            break;
        }
        if (strcmp(line, "\n") != 0){
            lines[i] = strdup(line);
            // lines[i] = calloc(1, sizeof(char*));
            // char* lineptr
            // lines[i] = line;
            // strncpy(lines[i], line, strlen(line));
            // strcpy(lines[i], line);
            if (lines[i][strlen(lines[i]) - 1] == '\n'){
                lines[i][strlen(lines[i]) - 1] = '\0';
            }
            while (isspace(*lines[i])) {
                lines[i]++;
            }
/*             if (lines[i][strlen(lines[i]) - 1] != ':') {
                ins_no++;
            } */
        } else {
            while (strcmp(line, "\n") == 0){
                ret = fgets(line, 100, file);
            }
            // ret = fgets(line, 1024, file);
            lines[i] = strdup(line);
            lines[i][strlen(lines[i]) - 1] = '\0';
            while (isspace(*lines[i])) {
                lines[i]++;
            }
        }
        if (DEBUG) printf("Line: %s\n", lines[i]);
        if (lines[i][strlen(lines[i]) - 1] == ':' && lines[i][0] != '#') {
            label_arr[label_no].label = strdup(lines[i]);
            label_arr[label_no].label[strlen(label_arr[label_no].label)-1]='\0';
            label_arr[label_no].val = ins_no;
            label_no++;
        } else {
            ins_no++;
        }
    }
    fclose(file);
    FILE* obj = fopen("a.obj", "wb+");
    short start = (htons(DEFAULT_CODESTART));
    fwrite(&start, 1, sizeof(start), obj);
    int mem_loc = 0;
    for (int i = 0; i < no_of_lines; i++) {
        char* cmd = strtok(lines[i], " ");
        char* args[3];
        for (int i = 0; i < 3; i++) {
            args[i] = strtok(NULL, " ");
        }
        char* arg1 = args[0];
        char* arg2 = args[1];
        char* arg3 = args[2];
        uint16_t inst;
        int offset;
        check_for_label(args, label_arr, no_of_labels);
        int cmp_ld, cmp_ldi, cmp_lea, cmp_sti;
        cmp_ld = strcmp(cmd, "ld");
        cmp_ldi = strcmp(cmd, "ldi");
        cmp_lea = strcmp(cmd, "lea");
        cmp_sti = strcmp(cmd, "sti");
        if (!cmp_ld || !cmp_ldi || !cmp_lea || !cmp_sti) {
            if (arg2[0] != '$' && arg2[0] != '%'){
                return 2;
            }
        }
        // check_args(args);
        if (DEBUG) printf("cmd: %s\n", cmd);
        if (cmd == NULL){
        } else if (cmd[0] == '#') {
            print_command("comment");
        } else if (cmd[strlen(cmd) - 1] == ':') {
            print_command("label");
            // fprintf(stderr, "Invalid opcode: %s\n", cmd);
            // exit(1);
        } else if (!strcmp(cmd, "add")){
            // char* arg1 = strtok(NULL, " ");
            // char* arg2 = strtok(NULL, " ");
            // char* arg3 = strtok(NULL, " ");
            int ar1, ar2, ar3;
            ar1 = atoi(arg1+2);
            ar2 = atoi(arg2+2);
            ar3 = atoi(arg3+1);
            if (arg3[0] == '$'){
                inst = htons(emit_add_imm(ar1, ar2, ar3));
                fwrite(&inst, 1, sizeof(inst), obj);
            } else if (arg3[0] == '%'){
                inst = htons(emit_add_reg(ar1, ar2, atoi(arg3+2)));
                fwrite(&inst, 1, sizeof(inst), obj);
            } else {
                // invalid_instruction();
                return 2;
            }
            if (DEBUG) printf("arg1: %s\n", arg1);
            if (DEBUG) printf("arg2: %s\n", arg2);
            if (DEBUG) printf("arg3: %s\n", arg3);
            if (DEBUG) printf("ADD COMMAND FOUND\n");
            mem_loc++;
        } else if (!strcmp(cmd, "and")) {
            char* arg1 = strtok(NULL, " ");
            char* arg2 = strtok(NULL, " ");
            char* arg3 = strtok(NULL, " ");
            int ar1, ar2, ar3;
            ar1 = atoi(arg1+2);
            ar2 = atoi(arg2+2);
            ar3 = atoi(arg3+1);
            if (arg3[0] == '$' && strcmp(arg3, "$") != 0){
                inst = htons(emit_and_imm(ar1, ar2, ar3));
                fwrite(&inst, 1, sizeof(inst), obj);
            } else if (arg3[0] == '%' && strcmp(arg3, "%") != 0){
                inst = htons(emit_and_reg(ar1, ar2, ar3));
                fwrite(&inst, 1, sizeof(inst), obj);
            } else {
                invalid_instruction();
                // return 2;
            }
            if (DEBUG) printf("arg1: %s\n", arg1);
            if (DEBUG) printf("arg2: %s\n", arg2);
            if (DEBUG) printf("arg3: %s\n", arg3);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "brn")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(1, 0, 0, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "brp")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(0, 0, 1, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "brz")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(0, 1, 0, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "brzp")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(0, 1, 1, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "brnp")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(1, 0, 1, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "brnz")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(1, 1, 0, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "brnzp")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(1, 1, 1, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "br")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_br(0, 0, 0, offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "jmp")) {
            inst = htons(emit_jmp(atoi(arg1+2)));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;

        } else if (!strcmp(cmd, "jsr")) {
            offset = atoi(arg1 + 1) - mem_loc - 1;
            inst = htons(emit_jsr(offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "jsrr")) {
            inst = htons(emit_jsrr(atoi(arg1+2)));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "ld")) {
            offset = atoi(arg2 + 1) - mem_loc - 1;
            inst = htons(emit_ld(atoi(arg1+2), offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "ldi")) {
            offset = atoi(arg2 + 1) - mem_loc - 1;
            inst = htons(emit_ldi(atoi(arg1+2), offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "ldr")) {
            int ar1, ar2, ar3;
            ar1 = atoi(arg1+2);
            ar2 = atoi(arg2+2);
            ar3 = atoi(arg3+1);
            offset = atoi(arg3 + 1) - mem_loc - 1;
            inst = htons(emit_ldr(ar1, ar2, ar3));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "lea")) {
            if (arg1[0] != '%'){
                return 2;
            }
            offset = atoi(arg2 + 1) - mem_loc - 1;
            inst = htons(emit_lea(atoi(arg1+2), offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "not")) {
            inst = htons(emit_not(atoi(arg1+2), atoi(arg2+2)));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "st")) {
            offset = atoi(arg2 + 1) - mem_loc - 1;
            inst = htons(emit_st(atoi(arg1+2), offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "sti")) {
            offset = atoi(arg2 + 1) - mem_loc - 1;
            inst = htons(emit_sti(atoi(arg1+2), offset));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "str")) {
            int ar1, ar2, ar3;
            ar1 = atoi(arg1+2);
            ar2 = atoi(arg2+2);
            ar3 = atoi(arg3+1);
            offset = atoi(arg3 + 1) - mem_loc - 1;
            inst = htons(emit_str(ar1, ar2, ar3));
            fwrite(&inst, 1, sizeof(inst), obj);
            print_command(cmd);
            mem_loc++;
        } else if (!strcmp(cmd, "val")) {
            inst = htons(emit_value(atoi(arg1+1)));
            fwrite(&inst, 1, sizeof(inst), obj);
            mem_loc++;
        } else if (!strcmp(cmd, "halt")) {
            inst = htons(emit_trap(TRAP_HALT));
            fwrite(&inst, 1, sizeof(inst), obj);
            mem_loc++;
        } else if (!strcmp(cmd, "putc")) {
            inst = htons(emit_trap(TRAP_OUT));
            fwrite(&inst, 1, sizeof(inst), obj);
            mem_loc++;
        } else if (!strcmp(cmd, "getc")) {
            inst = htons(emit_trap(TRAP_GETC));
            fwrite(&inst, 1, sizeof(inst), obj);
            mem_loc++;
        } else if (!strcmp(cmd, "puts")) {
            inst = htons(emit_trap(TRAP_PUTS));
            fwrite(&inst, 1, sizeof(inst), obj);
            mem_loc++;
        } else if (!strcmp(cmd, "enter")) {
            inst = htons(emit_trap(TRAP_IN));
            fwrite(&inst, 1, sizeof(inst), obj);
            mem_loc++;
        } else if (!strcmp(cmd, "putsp")) {
            inst = htons(emit_trap(TRAP_PUTSP));
            fwrite(&inst, 1, sizeof(inst), obj);
            mem_loc++;
        } else {
            if (DEBUG) printf("Invalid opcode: %s\n", cmd);
            // exit(1);
            // return 2;
        }
        // fwrite(lines[i], 1, strlen(lines[i]), obj);
        // fwrite("\n", 1, 1, obj);
    }
    fclose(obj);
    //free everything in the lines array
    //10 is good
    for (int i = 0; i < no_of_lines; i++){
        if (lines[0+i] != NULL){free(lines[0]+i);}
    }
    while(lines[no_of_lines] != NULL){
        free(lines[no_of_lines]);
        no_of_lines--;
    }

    free(lines);
    free(label_arr);
    return 0;
}
