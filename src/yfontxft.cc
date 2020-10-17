#include "config.h"

#ifdef CONFIG_XFREETYPE

#include "ystring.h"
#include "ypaint.h"
#include "ypointer.h"
#include "yxapp.h"
#include "intl.h"
#include <stdio.h>
#include <ft2build.h>
#include <X11/Xft/Xft.h>

#ifdef CONFIG_FRIBIDI
        // remove deprecated warnings for now...
        #include <fribidi/fribidi-config.h>
        #if FRIBIDI_USE_GLIB+0
                #include <glib.h>
                #undef G_GNUC_DEPRECATED
                #define G_GNUC_DEPRECATED
        #endif
        #include <fribidi/fribidi.h>
#endif

/******************************************************************************/

class YXftFont : public YFont {
public:
#ifdef CONFIG_I18N
    typedef class YUnicodeString string_t;
    typedef XftChar32 char_t;
#else
    typedef class YLocaleString string_t;
    typedef XftChar8 char_t;
#endif

    YXftFont(mslice name, bool xlfd, bool antialias);
    virtual ~YXftFont();

    virtual bool valid() const override { return fFonts.getCount() > 0; }
    int descent() const override { return fDescent; }
    int ascent() const override { return fAscent; }
    int textWidth(mslice s) const override;

    int textWidth(string_t const & str) const;
    void drawGlyphs(class Graphics & graphics, int x, int y,
                            char const * str, int len) override;

    bool supports(unsigned utf32char) override;

private:
    struct TextPart {
        XftFont * font;
        size_t length;
        unsigned width;
    };

    TextPart * partitions(char_t * str, size_t len, size_t nparts = 0) const;

    unsigned fAscent, fDescent;
    YArray<XftFont*> fFonts;
};

class XftGraphics {
public:
#ifdef CONFIG_I18N
    typedef XftChar32 char_t;

    #define XftDrawString XftDrawString32
    #define XftTextExtents XftTextExtents32
#else
    typedef XftChar8 char_t;

    #define XftTextExtents XftTextExtents8
    #define XftDrawString XftDrawString8
#endif

    static void drawString(Graphics &g, XftFont * font, int x, int y,
                           char_t * str, size_t len)
    {
#ifdef CONFIG_FRIBIDI
        const size_t bufsize = 256;
        char_t buf[bufsize];
        char_t *vis_str = buf;
        asmart<char_t> big;

        if (len >= bufsize) {
            big = new char_t[len+1];
            if (big == nullptr)
                return;
            vis_str = big;
        }

        FriBidiCharType pbase_dir = FRIBIDI_TYPE_N;

        if (fribidi_log2vis(str, len, &pbase_dir, //input
                            vis_str, // output
                            nullptr, nullptr, nullptr // "statistics" that we don't need
                            ))
        {
            str = vis_str;
        }
#endif

        XftDrawString(g.handleXft(), g.color().xftColor(), font,
                      x - g.xorigin(),
                      y - g.yorigin(),
                      str, len);
    }

    static void textExtents(XftFont * font, char_t * str, size_t len,
                            XGlyphInfo & extends) {
        XftTextExtents(xapp->display(), font, str, len, &extends);
    }

};

/******************************************************************************/

YXftFont::YXftFont(mslice name, bool use_xlfd, bool /*antialias*/):
    fAscent(0), fDescent(0)
{
    mslice s(name), elem;

    while(s.splitall(',', elem, s))
    {
        if (elem.isEmpty())
            continue;

        mstring fname = elem.trim();
        auto* font = use_xlfd
                ? XftFontOpenXlfd(xapp->display(), xapp->screen(), fname)
                : XftFontOpenName(xapp->display(), xapp->screen(), fname);

        if (nullptr != font) {
            fAscent = max(fAscent, (unsigned) max(0, font->ascent));
            fDescent = max(fDescent, (unsigned) max(0, font->descent));
            fFonts.append(font);
        } else {
            warn(_("Could not load font \"%s\"."), fname.c_str());
        }
    }

    if (!valid()) {
        msg("xft: fallback from '%s'", mstring(name).c_str());
        auto *sans = XftFontOpen(xapp->display(), xapp->screen(),
        XFT_FAMILY, XftTypeString, "sans-serif",
        XFT_PIXEL_SIZE, XftTypeInteger, 12,
        NULL);
        if (sans) {
            fAscent = sans->ascent;
            fDescent = sans->descent;
            fFonts.append(sans);
        } else {
            warn(_("Loading of fallback font \"%s\" failed."), "sans-serif");
        }
    }
}

YXftFont::~YXftFont() {
    if (xapp) {
        for (auto* font: fFonts)
            XftFontClose(xapp->display(), font);
    }
}

int YXftFont::textWidth(mslice s) const {
    return textWidth(string_t(s.data(), s.length()));
}

int YXftFont::textWidth(string_t const & text) const {
    char_t * str((char_t *) text.data());
    size_t len(text.length());

    TextPart *parts = partitions(str, len);
    unsigned width(0);

    for (TextPart * p = parts; p && p->length; ++p) width+= p->width;

    delete[] parts;
    return width;
}

void YXftFont::drawGlyphs(Graphics & graphics, int x, int y,
                          char const * str, int len) {
    string_t xtext(str, len);
    if (0 == xtext.length()) return;

    int const y0(y - ascent());
    int const gcFn(graphics.function());

    char_t * xstr((char_t *) xtext.data());
    size_t xlen(xtext.length());

    TextPart *parts = partitions(xstr, xlen);
///    unsigned w(0);
///    unsigned const h(height());

///    for (TextPart *p = parts; p && p->length; ++p) w+= p->width;

///    YPixmap *pixmap = new YPixmap(w, h);
///    Graphics canvas(*pixmap, 0, 0);
//    XftGraphics textarea(graphics, xapp->visual(), xapp->colormap());

    switch (gcFn) {
        case GXxor:
///         textarea.drawRect(*YColor::black, 0, 0, w, h);
            break;

        case GXcopy:
///            canvas.copyDrawable(graphics.drawable(),
///                                x - graphics.xorigin(), y0 - graphics.yorigin(), w, h, 0, 0);
            break;
    }


    int xpos(0);
    for (TextPart *p = parts; p && p->length; ++p) {
        if (p->font) {
            XftGraphics::drawString(graphics, p->font,
                                    xpos + x, ascent() + y0,
                                    xstr, p->length);
        }

        xstr += p->length;
        xpos += p->width;
    }

    delete[] parts;

///    graphics.copyDrawable(canvas.drawable(), 0, 0, w, h, x, y0);
///    delete pixmap;
}

bool YXftFont::supports(unsigned utf32char) {
    // be conservative, only report when all font candidates can do it
    for(int i = 0; i < fFonts.getCount(); ++i) {
        if (!XftCharExists(xapp->display(), fFonts[i], utf32char))
            return false;
    }
    return true;
}

YXftFont::TextPart * YXftFont::partitions(char_t * str, size_t len,
                                          size_t nparts) const
{
    if (!valid())
        return nullptr;

    XGlyphInfo extends;
    // XXX: those are still legacy pointer ops with adapter, make this right
    XftFont ** bFont = (XftFont **) fFonts.begin();
    XftFont ** lFont = bFont + fFonts.getCount();
    XftFont ** font(nullptr);
    char_t * c(str);

    for (char_t * endptr(str + len); c < endptr; ++c) {
        XftFont ** probe(bFont);

        while (probe < lFont && !XftGlyphExists(xapp->display(), *probe, *c))
            ++probe;

        if (probe != font) {
            if (nullptr != font) {
                TextPart *parts = partitions(c, len - (c - str), nparts + 1);
                parts[nparts].length = (c - str);

                if (font < lFont) {
                    XftGraphics::textExtents(*font, str, (c - str), extends);
                    parts[nparts].font = *font;
                    parts[nparts].width = extends.xOff;
                } else {
                    parts[nparts].font = nullptr;
                    parts[nparts].width = 0;
                    MSG(("glyph not found: %d", *(c - 1)));
                }

                return parts;
            } else
                font = probe;
        }
    }

    TextPart *parts = new TextPart[nparts + 2];
    parts[nparts + 1].font =  nullptr;
    parts[nparts + 1].width = 0;
    parts[nparts + 1].length = 0;
    parts[nparts].length = (c - str);

    if (nullptr != font && font < lFont) {
        XftGraphics::textExtents(*font, str, (c - str), extends);
        parts[nparts].font = *font;
        parts[nparts].width = extends.xOff;
    } else {
        parts[nparts].font = nullptr;
        parts[nparts].width = 0;
    }

    return parts;
}

ref<YFont> getXftFont(mslice name, bool antialias, bool from_xlfd) {
    ref<YFont>font(new YXftFont(name, from_xlfd, antialias));
    if (font == null || !font->valid()) {
        msg("failed to load font '%s', trying fallback", mstring(name).c_str());
        font.init(new YXftFont("sans-serif:size=12", false, antialias));
        if (font == null || !font->valid()) {
            msg("Could not load fallback Xft font.");
            return null;
        }
    }
    return font;
}

#endif // CONFIG_XFREETYPE

// vim: set sw=4 ts=4 et:
