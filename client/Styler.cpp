#include "Styler.h"

Styler::Styler():bold_set(false),italic_set(false),underlined_set(false)
{

}

bool Styler::isset_Bold(){
    return bold_set;
}

bool Styler::isset_Italic(){
    return italic_set;
}

bool Styler::isset_Underlined(){
    return underlined_set;
}

void Styler::set_bold(bool value){
    bold_set=value;
}

void Styler::set_italic(bool value){
    italic_set=value;
}

void Styler::set_underlined(bool value){
    underlined_set=value;
}
