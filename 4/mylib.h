#include <termios.h>
#include <unistd.h>
#define ESC 27
#define ENTER 10

char mygetch() {
    struct termios oldt,
    newt;
    char ch;
    tcgetattr( STDIN_FILENO, &oldt );
 
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
 
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
 
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}
