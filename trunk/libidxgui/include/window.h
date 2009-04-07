/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtGui/QtGui>
#include <QtGui/QResizeEvent>
#include <math.h>
#include <iostream>

#include "libidx.h"
#include "idxgui.h"

using namespace std;

namespace ebl {

  class text : public string {
  public:
    unsigned int	 h0, w0;
    text(unsigned int h0, unsigned int w0);
    ~text() {};
  };

  class arrow {
  public:
    int	                 h1, w1, h2, w2;
    arrow(int h1, int w1, int h2, int w2);
    ~arrow() {};
  };

  class Window : public QWidget { 
    Q_OBJECT
  private:
    QPixmap		*pixmap;
    QPoint		 pixmapOffset;
    QPoint		 lastDragPos;
    double		 pixmapScale;
    double		 curScale;
    float		 scaleIncr;
    idx<ubyte>		*buffer;
    QVector<QRgb>	 colorTable;
    QImage		*qimage;
    vector<text*>        texts;
    text*		 txt;
    vector<arrow*>        arrows;
    bool		 silent;
    unsigned int	 id;
    string		 savefname;
    unsigned int	 text_h0;
    unsigned int	 text_w0;

  public:
    Window(unsigned int wid, const char *wname = NULL, 
	   int height = 0, int width = 0);
    virtual ~Window();

    void save(const char *filename);
    void set_silent(const std::string *filename);
    void add_text(const std::string *s);
    void add_arrow(int h1, int w1, int h2, int w2);
    void update_pixmap(idx<ubyte> *img, unsigned int h0, unsigned int w0);
    void clear();
    void clear_text();
    void clear_arrows();
    void draw_text(QPainter &painter);
    void draw_arrows(QPainter &painter);
    void set_text_origin(unsigned int h0, unsigned int w0);

  protected:
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    
    void buffer_resize(int h, int w);
    void update_window(bool activate = true);
  };

} // namespace ebl {

#endif /* WINDOW_H_ */
