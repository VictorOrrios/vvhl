#include <vvhl/Core/Config.hpp>
#include <vvhl/Core/foo.hpp>
#include <iostream>
using namespace std;

void bar(){
    cout<<" Engine function! MaxFramesInFlight:"<<Config::MaxFramesInFlight<<endl;
}