#ifndef __YMENUITEM_H
#define __YMENUITEM_H

#include "ypaint.h"
#include "yicon.h"
#include "yaction.h"

class YMenu;

class YMenuItem {
public:
    YMenuItem(const mstring &name, int hotCharPos, mslice param, YAction action, YMenu *submenu);
    YMenuItem(const mstring &name);
    YMenuItem();
    virtual ~YMenuItem();

    const mstring& getName() const { return fName; }
    mstring getParam() const { return fParam; }
    YAction getAction() const { return fAction; }
    YMenu *getSubmenu() const { return fSubmenu; }

    int getHotChar() const {
        return (fName.nonempty() && fHotCharPos >= 0)
                ? fName.charAt(fHotCharPos) : -1;
    }

    int getHotCharPos() const {
        return fHotCharPos;
    }

    ref<YIcon> getIcon() const { return fIcon; }
    void setChecked(bool c);
    int isChecked() const { return fChecked; }
    int isEnabled() const { return fEnabled; }
    void setEnabled(bool e) { fEnabled = e; }
    void setSubmenu(YMenu *submenu) { fSubmenu = submenu; }

    virtual void actionPerformed(YActionListener *listener, YAction action, unsigned int modifiers);

    int queryHeight(int &top, int &bottom, int &pad) const;

    int getIconWidth() const;
    int getNameWidth() const;
    int getParamWidth() const;

    bool isSeparator() { return getName().nonempty() && getSubmenu() == nullptr; }

    void setIcon(ref<YIcon> icon);
private:
    mstring fName;
    mstring fParam;
    YAction fAction;
    int fHotCharPos;
    YMenu *fSubmenu;
    ref<YIcon> fIcon;
    bool fChecked;
    bool fEnabled;
};

#endif

// vim: set sw=4 ts=4 et:
