#ifndef STYLER_H
#define STYLER_H


class Styler
{
public:
    Styler();

    bool isset_Bold();
    bool isset_Italic();
    bool isset_Underlined();

    void set_bold(bool value);
    void set_italic(bool value);
    void set_underlined(bool value);

private:
    bool bold_set;
    bool italic_set;
    bool underlined_set;
};

#endif // STYLER_H
