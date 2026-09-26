
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

static void place_o(int height, int width, int player_y, int player_x,
                    int *target_y, int *target_x)
{
    do {
        *target_y = 1 + rand() % (height - 2);
        *target_x = 1 + rand() % (width - 2);
    } while (*target_y == player_y && *target_x == player_x);
}

int main(void)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    struct Segment {
        int y, x;
    };

    struct Segment snake[4096] = {{5, 5}};
    int length = 1;

    int i;
    int y = 5, x = 5;
    int dy = 0, dx = 1;
    int running = 1;

    int game_over = 0;
    int next_y, next_x, ate_food;
    int hit_wall, hit_body, last_to_check;

    WINDOW *game = newwin(LINES - 2, COLS - 2, 1, 1);
    if (game == NULL) {
        endwin();
        return 1;
    }

    int height, width;
    getmaxyx(game, height, width);
    if (height < 4 || width < 4) {
        delwin(game);
        endwin();
        return 1;
    }

    srand((unsigned int)time(NULL));
    int target_y, target_x;
    place_o(height, width, y, x, &target_y, &target_x);

    while (running) {
        switch (getch()) {
            case KEY_UP:
                if (dy != 1) {
                    dy = -1;
                    dx =  0;
                }
                break;
            case KEY_DOWN:
                if (dy != -1) {
                    dy =  1;
                    dx =  0;
                }
                break;
            case KEY_LEFT:
                if (dx != 1) {
                    dy = 0;
                    dx = -1;
                }
                break;
            case KEY_RIGHT:
                if (dx != -1) {
                    dy =  0;
                    dx =  1;
                }
                break;
            case 'q':
                running = 0;
                break;
            case ERR:
                break;
        }

        if (!running) break;

        next_y = snake[0].y + dy;
        next_x = snake[0].x + dx;
        ate_food = (next_y == target_y && next_x == target_x);

        hit_wall = next_y <= 0 || next_y >= height - 1 ||
                   next_x <= 0 || next_x >= width - 1;
        hit_body = 0;
        last_to_check = length - (ate_food ? 0 : 1);

        for (i = 1; i < last_to_check; i++) {
            if (next_y == snake[i].y && next_x == snake[i].x) {
                hit_body = 1;
                break;
            }
        }

        if (hit_wall || hit_body) {
            game_over = 1;
            break;
        }

        if (ate_food && length < 4096) {
            place_o(height, width, y, x, &target_y, &target_x);
            length++;
        }

        for (i = length - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }

        snake[0] = (struct Segment){ next_y, next_x };

        werase(game);

        box(game, 0, 0);
        mvwprintw(game, 0, 1, "Snake game");
        mvwaddch(game, target_y, target_x, 'O');

        for (i = 0; i < length; i++) {
            mvwaddch(game, snake[i].y, snake[i].x, '@');
        }

        wrefresh(game);

        napms(100);
    }

    delwin(game);
    endwin();

    return 0;
}
