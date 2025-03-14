#include "config.h"

#ifdef CONFIG_LIBRSVG

#ifdef CONFIG_IMLIB2
#include "yimage2.h"
#include <librsvg/rsvg.h>
#include <string.h>
#include "yfileio.h"
#include "base.h"
#else
#include "yimage.h"
#endif

ref<YImage> YImage::loadsvg(upath filename) {
    ref<YImage> icon;

#ifdef CONFIG_IMLIB2
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

    typedef Imlib_Image Image;

    fcsmart filedata(filename.loadText());
    if (filedata) {
        size_t length = strlen(filedata);
        GError* error = nullptr;
        const guint8* gudata = reinterpret_cast<const guint8 *>(filedata.data());
        RsvgHandle* handle = rsvg_handle_new_from_data(gudata, length, &error);
        if (handle) {
            RsvgDimensionData dim = { 0, 0, 0, 0 };
            rsvg_handle_get_dimensions(handle, &dim);
            GdkPixbuf* pixbuf = rsvg_handle_get_pixbuf(handle);
            rsvg_handle_close(handle, &error);
            bool alpha = gdk_pixbuf_get_has_alpha(pixbuf);
            int nchans = gdk_pixbuf_get_n_channels(pixbuf);
            int stride = gdk_pixbuf_get_rowstride(pixbuf);
            int width = dim.width;
            int height = dim.height;
            guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);
            Image image = imlib_create_image(width, height);
            if (image) {
                imlib_context_set_image(image);
                imlib_image_set_has_alpha(1);
                imlib_context_set_mask_alpha_threshold(ATH);
                imlib_context_set_anti_alias(1);
                DATA32* data = imlib_image_get_data();
                DATA32* argb = data;
                for (int row = 0; row < height; row++) {
                    const guchar* rowpix = pixels + row * stride;
                    for (int col = 0; col < width; col++, rowpix += nchans) {
                        const guchar red = rowpix[0];
                        const guchar grn = rowpix[1];
                        const guchar blu = rowpix[2];
                        const guchar alp = rowpix[3];
                        if (alpha && alp < ATH)
                            *argb++ = 0;
                        else if (alpha)
                            *argb++ = red << 16 | grn << 8 | blu | alp << 24;
                        else
                            *argb++ = red << 16 | grn << 8 | blu | 0xFF000000;
                    }
                }
                imlib_image_put_back_data(data);
                icon.init(new YImage2(width, height, image));
            }
            g_object_unref(G_OBJECT(pixbuf));
        }
        else {
            TLOG(("SVG %s error: %s", filename.string(), error->message));
            g_clear_error(&error);
        }
    }
    else {
        TLOG(("SVG %s error: %s", filename.string(), errno_string()));
    }

#pragma GCC diagnostic pop
#endif
#ifdef CONFIG_GDK_PIXBUF_XLIB
    icon = load(filename);
#endif

    return icon;
}
#endif

// vim: set sw=4 ts=4 et:
