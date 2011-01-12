/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef WINDOW_H_
#define WINDOW_H_

#include <QPixmap>
#include <QWidget>
#include <QtGui>
#include <QResizeEvent>
#include <math.h>
#include <iostream>

#include "defines_idxgui.h"
#include "libidx.h"
#include "scroll_box0.h"
#include "defines.h"

using namespace std;

namespace ebl {

  // TODO: derive all objects from a common drawable_object and hold
  // all of them in a same vector, to retain the drawing order.
  
  ////////////////////////////////////////////////////////////////
  // string

  class text : public string {
  public:
    uint	h0, w0;
    bool                pos_reset;
    unsigned char       fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a;
    text(uint h0, uint w0, bool pos_reset = false,
	 unsigned char fg_r = 255, unsigned char fg_g = 255, 
	 unsigned char fg_b = 255, unsigned char fg_a = 255,
	 unsigned char bg_r = 0, unsigned char bg_g = 0, 
	 unsigned char bg_b = 0, unsigned char bg_a = 127);
    ~text() {};
  };

  ////////////////////////////////////////////////////////////////
  // arrow

  class arrow {
  public:
    int	                 h1, w1, h2, w2;
    arrow(int h1, int w1, int h2, int w2);
    ~arrow() {};
  };

  ////////////////////////////////////////////////////////////////
  // box

  class box {
  public:
    int			 h0, w0, h, w;
    unsigned char	 r, g, b, a;
    box(int h0, int w0, int h, int w, unsigned char r,
	unsigned char g, unsigned char b, unsigned char a);
    ~box() {};
  };

  ////////////////////////////////////////////////////////////////
  // image

  class image {
  public:
    uint         h0, w0;
    idx<ubyte>           img;
    image(idx<ubyte> &img, uint h0, uint w0);
    ~image() {};
  };

  ////////////////////////////////////////////////////////////////
  // mask

  class imask {
  public:
    //! Constructor
    imask(idx<ubyte> *img, uint h0, uint w0,
	 ubyte r, ubyte g, ubyte b, ubyte a);
    ~imask() {};
    
    // members
    uint        h0, w0;
    QPixmap     map;
  };

  ////////////////////////////////////////////////////////////////
  // window

  class IDXGUIEXPORT Window : public QWidget { 
    Q_OBJECT
      
  public:
    Window(uint wid, const char *wname = NULL, 
	   int height = 0, int width = 0);
    virtual ~Window();

    //! used to disable or enable updating of window, for batch displaying.
    //! this is useful to avoid flickering and speed up display.
    void set_wupdate(bool ud);
    
    void save(const string &filename);
    void set_silent(const std::string *filename);
    void add_text(const std::string *s);
    void add_arrow(int h1, int w1, int h2, int w2);
    void add_box(int h0, int w0, int h, int w, unsigned char r, unsigned char g,
		 unsigned char b, unsigned char a, string *s);
    void add_image(idx<ubyte> &img, uint h0, uint w0);

    void add_mask(idx<ubyte> *img, uint h0, uint w0,
		  ubyte r, ubyte g, ubyte b, ubyte a);
    
    //! add img to the window and delete img.
    void update_pixmap(idx<ubyte> *img, uint h0, uint w0, bool updatepix =true);
    //! add img to the window.
    void update_pixmap(idx<ubyte> &img, uint h0, uint w0, bool updatepix =true);
    //! Clears the window, but does not resize it, leaves it to the current
    //! size.
    void clear();
    //! Clears the window and resizes it to 1x1.
    void clear_resize();
    void set_text_origin(uint h0, uint w0);

    ////////////////////////////////////////////////////////////////
    // Style methods
    
    //! \param ignore_frozen Ignore style freeze and apply color changes anyway.
    void set_text_colors(unsigned char fg_r, unsigned char fg_g, 
			 unsigned char fg_b, unsigned char fg_a,
			 unsigned char bg_r, unsigned char bg_g, 
			 unsigned char bg_b, unsigned char bg_a,
			 bool ignore_frozen = false);
    //! Set color of background.
    void set_bg_colors(unsigned char r, unsigned char g, unsigned char b);

    //! Set font to this size.
    void set_font_size(int sz);

    //! Freeze or unfreeze style. No modification of colors are allowed
    //! in frozen mode.
    void freeze_style(bool freeze);
    
    void add_scroll_box(scroll_box0 *sb);
    void remove_scroll_box(scroll_box0 *sb);
    void replace_scroll_box_with_copy(scroll_box0 *sb);

    //! Change window title.
    void set_title(const char *title);

    ////////////////////////////////////////////////////////////////
    // Events methods
    
    //! Return first key pressed of the key-pressed even list for
    //! current window and pop it out of the list, or return -1 if no key.
    int pop_key_pressed();

  protected:
    ////////////////////////////////////////////////////////////////
    // clear methods

    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_text(bool clear_tmp = false);

    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_arrows(bool clear_tmp = false);

    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_boxes(bool clear_tmp = false);

    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_images(bool clear_tmp = false);

    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_masks(bool clear_tmp = false);

    ////////////////////////////////////////////////////////////////
    // event methods
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    ////////////////////////////////////////////////////////////////
    // update methods
    
    //! Resize current buffer to given height and width.
    //! \param force If true, will resize to any size, otherwise, will not
    //!   accept to resize smaller than current size.
    void buffer_resize(uint h, uint w, bool force = false);
    //! Fill given buffer with background color.
    void buffer_fill(idx<ubyte> *buf);
    //! Refresh display of the window.
    //! \param activate Raises the focus to this window if true.
    void update_window(bool activate = false);
    void update_qimage();
    void swap();

    ////////////////////////////////////////////////////////////////
    // painting/drawing methods
    void draw_text(QPainter &painter, double scaleFactor = 1.0);
    void draw_arrows(QPainter &painter);
    void draw_boxes(QPainter &painter);
    void draw_masks(QPainter &painter);
    void draw_images(bool update = false);

  private slots:
    void scroll_previous();
    void scroll_next();

  private:
    QPixmap		*pixmap;
    QPoint		 pixmapOffset;
    QPoint		 lastDragPos;
    double		 pixmapScale;
    double		 curScale;
    float		 scaleIncr;
    idx<ubyte>		*buffer;
    uint		 buffer_maxh;
    uint		 buffer_maxw;
    QVector<QRgb>	 colorTable;
    QImage		*qimage;
    vector<text*>        texts;
    vector<text*>        texts_tmp;
    text*		 txt;
    vector<arrow*>       arrows;
    vector<arrow*>       arrows_tmp;
    vector<box*>         boxes;
    vector<box*>         boxes_tmp;
    vector<image*>       images;
    vector<image*>       images_tmp;
    vector<imask*>       masks;
    vector<imask*>       masks_tmp;
    bool		 silent;
    uint		 id;
    string		 savefname;
    uint		 text_h0;
    uint		 text_w0;
    bool                 pos_reset;	// text position was reset or not
    bool                 wupdate;
    QColor               text_fg_color;
    QColor               text_bg_color;
    QColor               bg_color;
    unsigned char        fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a;
    scroll_box0         *scrollbox;
    int                  wupdate_ndisable;// count how many disables called
    bool                 frozen_style; //!< style is frozen or not
    list<int>            keyspressed; //!< a list of key pressed events
    int                  font_size; //!< The size of the font.
  };

} // namespace ebl {

#endif /* WINDOW_H_ */
