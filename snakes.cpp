#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT
} Orientation;

class Snake {

    public:
        // represents a single bit of the snake
        typedef struct {            
            int xCoord;
            int yCoord;
            // orientation determines where the snake will move on next movement
            Orientation orient;
            // link to next and previous snake bit (next is closer to head)
            void* next;
            void* previous;
        } snakeBit;
        int snakeLength = 0;

        // head of the snake acts as the start of a double linked list
        // storing all snake bits    
        snakeBit* head = (snakeBit*) malloc(sizeof(snakeBit));
    
        Snake() {
            head->xCoord = 5;
            head->yCoord = 10;
            head->orient = RIGHT;
            head->next = NULL;
            head->previous = NULL;

            // initialise snake with a start length of 3
            for (int i = 0; i < 3; ++i) {
                this->increment_length();                
            }   
        }    

        // return current tail
        snakeBit* get_tail() {           
            snakeBit* current = head;
            while (current->previous != NULL) {
                current = (snakeBit*) current->previous;
            }
            return current;
        }

        // add a new snake bit to the end of the current tail
        void increment_length() {
            
            snakeBit* newTail = (snakeBit*) malloc(sizeof(snakeBit));
            snakeBit* tail = get_tail();
            tail->previous = (snakeBit*) newTail;
            newTail->next = (snakeBit*) tail;
            newTail->orient = tail->orient;
            newTail->previous = NULL;
            set_position(newTail);   
            snakeLength += 1; 
        }

        // set the position of the given bit based off the position 
        // and orientation of the succeeding bit
        void set_position(snakeBit* bit) {


            bit->xCoord = ((snakeBit*)bit->next)->xCoord;
            bit->yCoord = ((snakeBit*)bit->next)->yCoord;
            
            if (bit->orient == RIGHT) {
                bit->xCoord -= 1;
            }
            if (bit->orient == DOWN) {
                bit->yCoord -= 1;
            }
            if (bit->orient == LEFT) {
                bit->xCoord += 1;
            }
            if (bit->orient == UP) {
                bit->yCoord += 1;
            }
        
        }

        // move the given snake bit forward by 1 unit, dependent on its orientation
        void increment_position(snakeBit* bit) {
            
            if (bit->orient == RIGHT) {
                bit->xCoord += 1;
            }
            if (bit->orient == DOWN) {
                bit->yCoord += 1;
            }
            if (bit->orient == LEFT) {
                bit->xCoord -= 1;
            }
            if (bit->orient == UP) {
                bit->yCoord -= 1;
            }
        }

        // chain move forward all bits of the snake
        void move_forward() {           
           snakeBit* current = head;
           while (current != NULL) {
                increment_position(current);
                current = (snakeBit*) current->previous;                
           }

           update_orientations();    
        }

        // update the direction each snake bit is facing to equal the orientation
        // of the succeeding bit, from tail to head
        void update_orientations() {       

            snakeBit* current = get_tail();
            // exclude head of snake from updating (handled directly by key press)
            while (current->next != NULL) {
                current->orient = ((snakeBit*)current->next)->orient;
                current = (snakeBit*) current->next;
            }           
        }

        // return true if any snake bit (excluding head) occupies the given coordinates
        bool coordinate_occupied(int x, int y) {
            snakeBit* current = (snakeBit*) head->previous;
            while (current != NULL) {
                if ( (current->xCoord == x) && (current->yCoord == y) ) {
                    return true;
                }
                current = (snakeBit*) current->previous;
            }
            return false;
            
        }

        void free_all() {
            snakeBit* current = (snakeBit*) head;
            snakeBit* temp;
            while (current != NULL) {
                temp = current;
                current = (snakeBit*) current->previous;
                free(temp);
            }
        }
};


class Board {

    private:
        // width of board
        int squareWidth = 15;
        // the current apple on board, represented as an x and y coordinate
        typedef struct {
            int xCoord;
            int yCoord;
        } Apple;
        // score counter
        int score = 0;
        // store locally the apple and snake on board        
        Snake snake;
        Apple apple; 

        void set_apple_location(Apple* apple) {
            do {
                apple->xCoord = rand() % (squareWidth-1);
                apple->yCoord = rand() % (squareWidth-1);
            
            } while (snake.coordinate_occupied(apple->xCoord, apple->yCoord)); 
        }

    public:

        Board() {
            set_apple_location(&apple);            
        }

        void handle_key_press(char input) {

            switch (input) {
            case 'w':
                if (snake.head->orient != DOWN) {
                    snake.head->orient = UP;
                }
                break;
            case 'a':
                if (snake.head->orient != RIGHT) {
                    snake.head->orient = LEFT;
                }
                break;
            case 's':
                if (snake.head->orient != UP) {
                    snake.head->orient = DOWN;
                }
                break;
            case 'd':
                if (snake.head->orient != LEFT) {
                    snake.head->orient = RIGHT;
                }
            default:
                break;
            };
        }

        void free_snake() {
            snake.free_all();
        }

        // increment position of the snake and check if the apple has been hit
        void update_board() {

            snake.move_forward();
            // check if snake ate the apple, if so increment score and reset apple position
            if ( (snake.head->xCoord == apple.xCoord) && (snake.head->yCoord == apple.yCoord) ) {
                score += 1;
                set_apple_location(&apple);
                snake.increment_length();
            }
            print_board();
        }

        // if snake has left board bounds or eaten itself, set game over true
        void check_game_over(bool* gameOver) {
            int x = snake.head->xCoord;
            int y = snake.head->yCoord;

            if ( (x < 0) || (x >= squareWidth) 
                || (y < 0) || (y >= squareWidth) 
                || (snake.coordinate_occupied(x,y))) {
                    *gameOver = true;
                }
        }

        void print_board() {
            system("cls");
            for (int y = 0; y < squareWidth; ++y) {
                for (int x = 0; x < squareWidth; ++x) {
                    // check for either apple, snake head, snake body, or empty coordinate
                    if ( (apple.xCoord == x)  && (apple.yCoord == y) ) {
                        std::cout << " A";
                    } else if ( (snake.head->xCoord == x) && (snake.head->yCoord == y) ) {
                        std::cout << " x";
                    } else if (snake.coordinate_occupied(x,y)) {
                        std::cout << " O";
                    } else {
                        std::cout << " .";
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n\n\n";
        }
        int get_score() {
            return score;
        }
};

int play_game() {

    Board board; 
    bool gameOver = false;
    while (!gameOver) {
        if (kbhit()) {
            board.handle_key_press(getch());
        }
        board.update_board();
        board.check_game_over(&gameOver);
        Sleep(300);
    }
    std::cout << "u scored " << board.get_score() << " points\n";
    board.free_snake();
    return 1;
}
int main(int argc, char** argv) {
    return play_game();
}