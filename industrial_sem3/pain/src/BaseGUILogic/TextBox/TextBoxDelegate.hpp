#ifndef TEXT_BOX_DELEGATE
#define TEXT_BOX_DELEGATE

class TextBoxDelegate
{
public:
    virtual ~TextBoxDelegate() {};

    virtual void onTextChange(const char *newText, int userData) = 0;
};

#endif