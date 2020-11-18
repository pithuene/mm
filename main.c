#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>

#include "ssh.h"
#include "sftp.h"
#include "list.h"

void init_colors(void) {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
}

void init_window(WINDOW * win) {
    if ((win = initscr()) == NULL) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    } else {
        clear();
        noecho();
        cbreak();
        curs_set(0);
        init_colors();
    }
}


struct Season {
    char * name;
    List episodes;
};

struct Show {
    char * name;
    List seasons;
};


struct ApplicationState {
    sftp_session sftp;
    char * domain;
    char * user;
    char * shows_dir;
    int col; // 0 => shows, 1 => seasons, 2 => episodes
    int show_idx;
    int season_idx;
    int episode_idx;
    List shows;
};

typedef struct ApplicationState * State;

static struct ApplicationState app_state;

void print_episode_item(struct list_item * el, int idx) {
    if(idx == app_state.episode_idx) {
        if(app_state.col == 2) {
            attron(COLOR_PAIR(3));
        } else {
            attron(COLOR_PAIR(2));
        }
    } else {
        attron(COLOR_PAIR(1));
    }
    mvprintw(idx, 50, "%-40.40s", (char *) list_item_value(el));
}

void print_season_item(struct list_item * el, int idx) {
    if(idx == app_state.season_idx) {
        if(app_state.col == 1) {
            attron(COLOR_PAIR(3));
        } else {
            attron(COLOR_PAIR(2));
        }
    } else {
        attron(COLOR_PAIR(1));
    }
    mvprintw(idx, 25, "%-25.25s", ((struct Season *) list_item_value(el))->name);
}

void print_showlist_item(struct list_item * el, int idx) {
    if(idx == app_state.show_idx) {
        if(app_state.col == 0) {
            attron(COLOR_PAIR(3));
        } else {
            attron(COLOR_PAIR(2));
        }
    } else {
        attron(COLOR_PAIR(1));
    }
    mvprintw(idx, 0, "%-25.25s", ((struct Show *) list_item_value(el))->name);
}

void index_season(sftp_attributes season_dir, void * seasons_list) {
    // show is always the last added one
    struct Show * show = list_item_value(list_get_last((List) app_state.shows));
    struct Season * season = malloc(sizeof(struct Season));
    list_append(seasons_list, season);

    char * seasonname = malloc(strlen(season_dir->name));
    strcpy(seasonname, season_dir->name);
    season->name = seasonname;

    char * season_dir_path = malloc(strlen(app_state.shows_dir) + 1 + strlen(show->name) + 1 + strlen(season->name) + 1);
    strcpy(season_dir_path, app_state.shows_dir);
    strcat(season_dir_path, "/");
    strcat(season_dir_path, show->name);
    strcat(season_dir_path, "/");
    strcat(season_dir_path, season->name);

    List episodes = sftp_dir_list_names(app_state.sftp, season_dir_path);

    free(season_dir_path);

    season->episodes = episodes;
}

void index_show(sftp_attributes show_dir, void * shows_list) {
    struct Show * show = malloc(sizeof(struct Show));
    list_append(shows_list, show);

    char * showname = malloc(strlen(show_dir->name));
    strcpy(showname, show_dir->name);
    show->name = showname;

    char * show_dir_path = malloc(strlen(app_state.shows_dir) + 1 + strlen(show->name) + 1);
    strcpy(show_dir_path, app_state.shows_dir);
    strcat(show_dir_path, "/");
    strcat(show_dir_path, show->name);

    List seasons = list_create();

    sftp_dir_foreach(app_state.sftp, show_dir_path, index_season, seasons, 1);

    free(show_dir_path);

    show->seasons = seasons;
}

void index_shows(State state) {
    List shows = list_create();
    state->shows = shows;
    sftp_dir_foreach(state->sftp, state->shows_dir, index_show, shows, 1);
}

int main(int argc, char * argv[]) {

    if(argc != 4) {
        printf("mm domain user /path/to/tvshows\n");
        return 1;
    }

    State state = &app_state;
    state->show_idx = 0;

    WINDOW * mainwin;
    init_window(mainwin);

    mvprintw(0,0, "Indexing...");
    refresh();

    state->domain = argv[1];
    state->user = argv[2];
    state->shows_dir = argv[3];
    state->sftp = sftp_session_create(state->domain, state->user);
    index_shows(state);


    int key = 0;
    while (key != 113) {
        struct Show * selected_show = list_item_value((struct list_item *) list_get_item(state->shows, state->show_idx));
        struct Season * selected_season = list_item_value(list_get_item(selected_show->seasons, state->season_idx));
        char * selected_episode = list_item_value(list_get_item(selected_season->episodes, state->episode_idx));

        clear();
        switch(key) {
            case 104: // h
                if (state->col > 0) {
                    state->col--;
                }
                break;
            case 106: // j
                if (state->col == 0) {
                    if (state->show_idx < list_length(state->shows) - 1) {
                        state->show_idx++;
                        state->season_idx = 0;
                        state->episode_idx = 0;
                    }
                } else if (state->col == 1) {
                    if (state->season_idx < list_length(selected_show->seasons) - 1) {
                        state->season_idx++;
                        state->episode_idx = 0;
                    }
                } else if (state->col == 2) {
                    if (state->episode_idx < list_length(selected_season->episodes) - 1) {
                        state->episode_idx++;
                    }
                }

                break;
            case 107: // k
                if (state->col == 0) {
                    if (state->show_idx > 0) {
                        state->show_idx--;
                        state->season_idx = 0;
                        state->episode_idx = 0;
                    }
                } else if (state->col == 1) {
                    if (state->season_idx > 0) {
                        state->season_idx--;
                        state->episode_idx = 0;
                    }
                } else if (state->col == 2) {
                    if (state->episode_idx > 0) {
                        state->episode_idx--;
                    }
                }
                break;
            case 108: // l
                if (state->col < 2) {
                    state->col++;
                } else {
                    char * addr = malloc(
                            5 + 7
                            + strlen(state->user) + 1
                            + strlen(state->domain)
                            + strlen(state->shows_dir) + 1
                            + strlen(selected_show->name) + 1
                            + strlen(selected_season->name) + 1
                            + strlen(selected_episode)
                            + 16);
                    strcpy(addr, "mpv '");
                    strcat(addr, "sftp://");
                    strcat(addr, state->user);
                    strcat(addr, "@");
                    strcat(addr, state->domain);
                    strcat(addr, state->shows_dir);
                    strcat(addr, "/");
                    strcat(addr, selected_show->name);
                    strcat(addr, "/");
                    strcat(addr, selected_season->name);
                    strcat(addr, "/");
                    strcat(addr, selected_episode);
                    strcat(addr, "' > /dev/null &");
                    mvprintw(20,0, "%s", addr);
                    system(addr);
                }
                break;
        }

        // Update after key handling
        selected_show = list_item_value((struct list_item *) list_get_item(state->shows, state->show_idx));
        selected_season = list_item_value(list_get_item(selected_show->seasons, state->season_idx));

        list_for_each(state->shows, print_showlist_item);
        list_for_each(selected_show->seasons, print_season_item);
        list_for_each(selected_season->episodes, print_episode_item);

        /*
        attron(COLOR_PAIR(1));
        mvprintw(12,0, "Row: %i", state->col);
        mvprintw(13,0, "Shows length: %i", list_length(state->shows));
        mvprintw(14,0, "Show idx: %i", state->show_idx);
        mvprintw(15,0, "Seasons length: %i", list_length(selected_show->seasons));
        mvprintw(16,0, "Season idx: %i", state->season_idx);
        mvprintw(15,0, "Episodes length: %i", list_length(selected_season->episodes));
        mvprintw(16,0, "Episode idx: %i", state->episode_idx);
        mvprintw(17,0, "Last key: %i", key);
        */
        refresh();
        key = getch();
    }

    sftp_free(state->sftp);

    delwin(mainwin);
    endwin();
    list_free(state->shows);
    free(state);
    return EXIT_SUCCESS;
}
