
#include "window.hh"
#include "logic.hh"

int main()
{
    window::add_logic(logic::run);
    window::run();

    return 0;
}
