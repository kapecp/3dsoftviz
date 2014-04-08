#ifndef LUAGRAPHOBJECT_H
#define LUAGRAPHOBJECT_H

namespace Lua {

class LuaGraphObject
{
public:
    qlonglong getId() const;
    void setId(qlonglong value);

    QString getLabel() const;
    void setLabel(const QString &value);
private:
    qlonglong id;
    QString label;
};

}

#endif // LUAGRAPHOBJECT_H
