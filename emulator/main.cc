#include <iostream>

#include <fortuna.hh>

int main(int argc, char* argv[])
{
    fortuna::init();

    // TODO

    for (;;) {
        static fortuna::Event e;
        while (fortuna::next_event(&e)) {
            switch (e.type) {
            }
        }
    }
}