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

#include "moc_window.cxx"
#include <qstyle.h>
#include <math.h>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // text

  text::text(unsigned int h0_, unsigned int w0_, bool pos_reset_,
	     unsigned char fg_r_, unsigned char fg_g_, 
	     unsigned char fg_b_, unsigned char fg_a_,
	     unsigned char bg_r_, unsigned char bg_g_, 
	     unsigned char bg_b_, unsigned char bg_a_)
    : string(""), h0(h0_), w0(w0_), pos_reset(pos_reset_),
      fg_r(fg_r_), fg_g(fg_g_), fg_b(fg_b_), fg_a(fg_a_), 
      bg_r(bg_r_), bg_g(bg_g_), bg_b(bg_b_), bg_a(bg_a_) {
  }

  ////////////////////////////////////////////////////////////////
  // arrow

  arrow::arrow(int h1_, int w1_, int h2_, int w2_)
    : h1(h1_), w1(w1_), h2(h2_), w2(w2_) {
  }

  ////////////////////////////////////////////////////////////////
  // box

  box::box(int h0_, int w0_, int h_, int w_, unsigned char r_, unsigned char g_,
	   unsigned char b_)
    : h0(h0_), w0(w0_), h(h_), w(w_), r(r_), g(g_), b(b_) {
  }

  ////////////////////////////////////////////////////////////////
  // image

  image::image(idx<ubyte> &img_, unsigned int h0_, unsigned int w0_)
    : h0(h0_), w0(w0_) {
    img = img_;
  }

  ////////////////////////////////////////////////////////////////
  // Window

  Window::Window(unsigned int wid, const char *wname, int height, int width) 
    : pixmapScale(1.0), curScale(1.0), scaleIncr(1), colorTable(256),
      texts(), silent(false), id(wid), savefname("") {
    setAttribute(Qt::WA_DeleteOnClose);
    if (wname) {
      QString q(wname);
      setWindowTitle(q);
    } else {
      QString q("EBLearn Graphics");
      setWindowTitle(q);
    }
    pixmap = new QPixmap(1, 1);
    buffer = NULL;
    qimage = NULL;
    for (int i = 0; i < 256; i++){
      colorTable[i] = qRgb(i, i, i);
    }
    clear();
    if ((height != 0) && (width != 0))
      buffer_resize(height, width);
    wupdate = true; // always update display
    set_text_colors(255, 255, 255, 255, 0, 0, 0, 127);
    buffer_maxh = height;
    buffer_maxw = width;
    pos_reset = true;
    scrollbox = NULL;
    setFocusPolicy(Qt::NoFocus);
  }

  Window::~Window() {
    delete pixmap;
    if (buffer)
      delete buffer;
    if (qimage)
      delete qimage;
    clear_text();
    clear_arrows();
    clear_boxes();
    clear_images();
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  void Window::clear() {
    if (buffer)
      idx_fill(*buffer, (ubyte) 255);
    if (pixmap) 
      pixmap->fill(Qt::white);
    clear_text();
    clear_arrows();
    clear_boxes();
    clear_images();
    update_window();
  }

  void Window::clear_text() {
    for (vector<text*>::iterator i = texts.begin(); i != texts.end(); ++i)
      if (*i)
	delete (*i);
    texts.clear();
    txt = NULL;
    text_h0 = 0;
    text_w0 = 0;
  }

  void Window::clear_arrows() {
    for (vector<arrow*>::iterator i = arrows.begin(); i != arrows.end(); ++i)
      if (*i)
	delete (*i);
    arrows.clear();
  }

  void Window::clear_boxes() {
    for (vector<box*>::iterator i = boxes.begin(); i != boxes.end(); ++i)
      if (*i)
	delete (*i);
    boxes.clear();
  }

  void Window::clear_images() {
    for (vector<image*>::iterator i = images.begin(); i != images.end(); ++i)
      if (*i)
	delete (*i);
    images.clear();
    buffer_maxh = 0;
    buffer_maxw = 0;
  }

  ////////////////////////////////////////////////////////////////

  void Window::set_wupdate(bool ud) {
    if (wupdate != ud) {
      wupdate = ud;
      if (wupdate) {
	setUpdatesEnabled(true);
	draw_images();
	repaint();
	update_window();
      }
      else
	setUpdatesEnabled(false);
    }
  }

  void Window::save(const char *filename) {
    QPixmap p = QPixmap::grabWidget(this, rect());
    if (!p.save(filename, "PNG", 90))
      cerr << "Warning: failed to save window to " << filename << "." << endl;
  }
  
  void Window::set_silent(const std::string *filename) {
    silent = true;
    ostringstream o;
    if ((filename) && (strcmp(filename->c_str(), "") != 0))
      o << *filename << ".";
    o << id << ".png";
    savefname = o.str();
    hide();
    update();
  }

  ////////////////////////////////////////////////////////////////
  // add methods

  void Window::add_text(const std::string *s) {
    if (!txt) {
      txt = new text(text_h0, text_w0, pos_reset, fg_r, fg_g, fg_b, fg_a,
		     bg_r, bg_g, bg_b, bg_a);
      texts.push_back(txt);
    }
    *txt += *s;
    delete s;
    update_window();
    pos_reset = false;
  }
  
  void Window::add_arrow(int h1, int w1, int h2, int w2) {
    arrows.push_back(new arrow(h1, w1, h2, w2));
    update_window();
  }
  
  void Window::add_box(int h0, int w0, int h, int w, unsigned char r,
		       unsigned char g, unsigned char b, string *s) {
    // add box
    boxes.push_back(new box(h0, w0, h, w, r, g, b));
    // add caption
    set_text_origin(h0 + 1, w0 + 1);
    if (s)
      add_text(s);
    update_window();
  }
  
  void Window::add_image(idx<ubyte> &img, unsigned int h0, unsigned int w0) {
    images.push_back(new image(img, h0, w0));
    update_window();
  }

  void Window::set_text_colors(unsigned char fg_r_, unsigned char fg_g_, 
			       unsigned char fg_b_, unsigned char fg_a_,
			       unsigned char bg_r_, unsigned char bg_g_, 
			       unsigned char bg_b_, unsigned char bg_a_) {
    fg_r = fg_r_;
    fg_g = fg_g_;
    fg_b = fg_b_;
    fg_a = fg_a_;
    bg_r = bg_r_;
    bg_g = bg_g_;
    bg_b = bg_b_;
    bg_a = bg_a_;
    txt = NULL;
  }
  
  void Window::set_text_origin(unsigned int h0, unsigned int w0) {
    text_h0 = h0;
    text_w0 = w0;
    txt = NULL;
    pos_reset = true;
  }

  void Window::add_scroll_box(scroll_box0 *sb) {
    scrollbox = sb;
    scrollbox->set_parent(this);
    cout << "adding scrollbox " << sb << endl;
  }

  void Window::remove_scroll_box(scroll_box0 *sb) {
    scrollbox = NULL;
    cout << "removing scroll box " << sb << endl;
  }

  void Window::replace_scroll_box_with_copy(scroll_box0 *sb) {
    cout << "replacing scroll box " << scrollbox << " with a copy: " << endl;
    scrollbox = sb->copy();
    cout << scrollbox << endl;
  }

  ////////////////////////////////////////////////////////////////
  // update methods

  void Window::buffer_resize(uint h, uint w) {
    // arbitrary bounding of h and w to prevent gigantic erroneous values.
    uint bound = 10000;
    if (MAX(h, w) > bound) {
      cerr << "error: trying to resize display buffer to " << h << "x" << w;
      cerr << ", one of those dimensions is greater than " << bound;
      cerr << " and probably erroneous." << endl;
      eblerror("too big values for resizing");
    }
    if ((!buffer || (((uint)buffer->dim(0))< h) || (((uint)buffer->dim(1)) < w))
	&& ((h != 0) && (w != 0))) {
      resize(w, h);
      if (!buffer) {
	buffer = new idx<ubyte>(h, w, 3);
	idx_fill(*buffer, (ubyte) 255);
      }
      else {
	idx<ubyte> *inew = new idx<ubyte>(h, w, 3);
	idx_fill(*inew, (ubyte) 255);
	idx<ubyte> tmpnew = inew->narrow(0, MIN(h, (uint)buffer->dim(0)), 0);
	tmpnew = tmpnew.narrow(1, MIN(w, (uint)buffer->dim(1)), 0);
	idx<ubyte> tmpbuf = buffer->narrow(0, MIN(h, (uint)buffer->dim(0)), 0);
	tmpbuf = tmpbuf.narrow(1, MIN(w, (uint)buffer->dim(1)), 0);
	idx_copy(tmpbuf, tmpnew);
	delete buffer;
	buffer = inew;
      }
      if (qimage)
	delete qimage;
      qimage = new QImage((unsigned char*) buffer->idx_ptr(), 
			  buffer->dim(1), buffer->dim(0), 
 			  buffer->dim(1) * buffer->dim(2) *
 			  sizeof (unsigned char),
			  QImage::Format_RGB888);
      qimage->setColorTable(colorTable);
    }
  }

  void Window::update_pixmap(idx<ubyte> *img, unsigned int h0, 
			     unsigned int w0) {
    if (img) {
      update_pixmap(*img, h0, w0);
      delete img;
    }
  }

  void Window::update_pixmap(idx<ubyte> &img, unsigned int h0, 
			     unsigned int w0) {
    unsigned int h = MAX(buffer?(unsigned int)buffer->dim(0):0, 
			 h0 + img.dim(0));
    unsigned int w = MAX(buffer?(unsigned int)buffer->dim(1):0, 
			 w0 + img.dim(1));
    if (wupdate) {
      if (!buffer)
	buffer_resize(h, w);
      else if ((h > (unsigned int) buffer->dim(0)) || 
	       (w > (unsigned int) buffer->dim(1)))
	buffer_resize(h, w);
      idx<ubyte> tmpbuf = buffer->narrow(0, img.dim(0), h0);
      tmpbuf = tmpbuf.narrow(1, img.dim(1), w0);
      if ((img.order() == 3) && (img.dim(2) == 3)) // RGB
	idx_copy(img, tmpbuf);
      else if ((img.order() == 2) ||
	       ((img.order() == 3) && (img.dim(2) == 1))) { // greyscale
	idx<ubyte> tmpbufl = tmpbuf.select(2, 0);
	idx_copy(img, tmpbufl);
	tmpbufl = tmpbuf.select(2, 1);
	idx_copy(img, tmpbufl);
	tmpbufl = tmpbuf.select(2, 2);
	idx_copy(img, tmpbufl);
      }
      else {
	cerr << "unknown image dimensions: " << img << endl;
	eblerror("expected a grayscale or rgb image");
      }      
      // copy buffer to pixmap
      *pixmap = QPixmap::fromImage(*qimage);
      update_window();
    }
    else { // don't add the image if wupdate is false
      // instead keep it in a list of images to be displayed later
      add_image(img, h0, w0);
      // and remember the maximum size of the display buffer
      buffer_maxh = h;
      buffer_maxw = w;
    }
  }

  void Window::update_window(bool activate) {
    if (wupdate) {
      update(); 
      // saving pixmap if silent or show it otherwise
      if (silent) 
	save(savefname.c_str());
      else {
	show();
	if (activate)
	  activateWindow();
      }
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // painting/drawing methods

  void Window::paintEvent(QPaintEvent * /* event */) {
    QStylePainter painter(this);
    painter.fillRect(rect(), Qt::white);
    double scaleFactor = pixmapScale / curScale;
    painter.save();
    painter.translate(pixmapOffset);
    painter.scale(scaleFactor, scaleFactor);
    QRectF exposed = 
      painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
    painter.drawPixmap(exposed, *pixmap, exposed);
    draw_boxes(painter);
    draw_text(painter);
    draw_arrows(painter);
    painter.restore();
    if (!silent) {
      QString txt = tr("Use mouse wheel to zoom, left click to drag.");
      QFontMetrics metrics = painter.fontMetrics();
      int textWidth = metrics.width(txt);
      painter.setPen(Qt::NoPen);
      painter.setBrush(QColor(0, 0, 0, 127));
      painter.drawRect((width() - textWidth) / 2 - 5, height() - 15, 
		       textWidth + 10,
		       metrics.lineSpacing() + 5);
      painter.setPen(Qt::white);
      painter.drawText((width() - textWidth) / 2,
		       height() - 15 + metrics.leading() + metrics.ascent(), 
		       txt);
    }
  }

  void Window::draw_arrows(QPainter &painter) {
    int len_factor = 1;
    int ax1, ay1, ax2, ay2;
    painter.setBrush(QColor(255, 255, 255, 127));
    painter.setPen(Qt::red);
    for (vector<arrow*>::iterator i = arrows.begin(); i != arrows.end(); ++i) {
      if (*i) {

//     QStyleOption opt;
//     opt.rect.setWidth(100);
//     opt.rect.setHeight(150);
//     QPixmap qp = painter.standardPixmap(QStyle::SP_ArrowUp, &opt);
//     painter.drawPixmap(exposed, qp, exposed);
//     painter.drawPrimitive(QStyle::PE_IndicatorSpinUp, opt);

	ax1 = (*i)->h1;
	ay1 = (*i)->w1;
	ax2 = (*i)->h2;
	ay2 = (*i)->w2;
	double angle = atan2( (double) ay1 - ay2, (double) ax1 - ax2);
	double hypotenuse = sqrt( pow((float)ay1 - ay2, 2) + pow((float)ax1 - ax2, 2));
	/* Here we lengthen the arrow by a factor of three. */
	ax2 = (int) (ax1 - len_factor * hypotenuse * cos(angle));
	ay2 = (int) (ay1 - len_factor * hypotenuse * sin(angle));
	painter.drawLine(ax1, ay1, ax2, ay2);
	ax1 = (int) (ax2 + 9 * cos(angle + M_PI / 4));
	ay1 = (int) (ay2 + 9 * sin(angle + M_PI / 4));
	painter.drawLine(ax1, ay1, ax2, ay2);
	ax1 = (int) (ax2 + 9 * cos(angle - M_PI / 4));
	ay1 = (int) (ay2 + 9 * sin(angle - M_PI / 4));
	painter.drawLine(ax1, ay1, ax2, ay2);
      }
    }
    painter.setPen(Qt::black);
  }

  void Window::draw_boxes(QPainter &painter) {
    painter.setBrush(QColor(255, 255, 255, 127));
    for (vector<box*>::iterator i = boxes.begin(); i != boxes.end(); ++i) {
      if (*i) {
	// set color
	painter.setPen(QColor((*i)->r, (*i)->g, (*i)->b));
	// draw box lines
	painter.drawLine((*i)->w0,               (*i)->h0,
			 (*i)->w0,               (*i)->h0 + (*i)->h - 1);
	painter.drawLine((*i)->w0,               (*i)->h0 + (*i)->h - 1,
			 (*i)->w0 + (*i)->w - 1, (*i)->h0 + (*i)->h - 1);
	painter.drawLine((*i)->w0,               (*i)->h0,
			 (*i)->w0 + (*i)->w - 1, (*i)->h0);
	painter.drawLine((*i)->w0 + (*i)->w - 1, (*i)->h0,
			 (*i)->w0 + (*i)->w - 1, (*i)->h0 + (*i)->h - 1);
      }
    }
    painter.setPen(Qt::black);
  }

  void Window::draw_text(QPainter &painter) {
    unsigned int th = 0, tw = 0;
    for (vector<text*>::iterator i = texts.begin(); i != texts.end(); ++i) {
      if (*i) {
	text_fg_color.setRgb((*i)->fg_r, (*i)->fg_g, (*i)->fg_b, (*i)->fg_a);
	text_bg_color.setRgb((*i)->bg_r, (*i)->bg_g, (*i)->bg_b, (*i)->bg_a); 
	QString txt((*i)->c_str());
	QRectF bg;
	painter.setPen(text_fg_color);
	QRect qr = rect();
	qr.setLeft((*i)->pos_reset ? (*i)->w0 : tw);
	qr.setTop( (*i)->pos_reset ? (*i)->h0 - 1 : th);

// 	// resize buffer if text is out?
// 	QRect br = painter.boundingRect(qr, Qt::AlignLeft & Qt::TextWordWrap 
// 					& Qt::AlignTop, txt);
// 	buffer_maxh = MAX(buffer?(unsigned int)buffer->dim(0):0, 
// 			  (*i)->h0 + br.height());
// 	buffer_maxw = MAX(buffer?(unsigned int)buffer->dim(1):0, 
// 			  (*i)->w0 + br.width());
// 	buffer_resize(buffer_maxh, buffer_maxw);

	painter.drawText(qr, Qt::AlignLeft & Qt::TextWordWrap & Qt::AlignTop,
			 txt, &bg);
	th = bg.top();
	tw = bg.right();
	painter.setPen(Qt::NoPen);
	painter.setBrush(text_bg_color);
	bg.setTop(bg.top() + 1);
	bg.setHeight(bg.height() - 3);
	painter.drawRect(bg);
	painter.setPen(text_fg_color);
	painter.drawText(qr, Qt::AlignLeft & Qt::TextWordWrap & Qt::AlignTop,
			 txt, &bg);
      }
    }
  }

  void Window::draw_images() {
    buffer_resize(buffer_maxh, buffer_maxw); // resize to maximum size first
    // then display all images not displayed
    for (vector<image*>::iterator i = images.begin(); i != images.end(); ++i)
      if (*i) {
	update_pixmap((*i)->img, (*i)->h0, (*i)->w0);
	delete (*i);
      }
    images.clear();
  }

  ////////////////////////////////////////////////////////////////
  // events methods

  void Window::wheelEvent(QWheelEvent *event) {
    float precision = .25;
    float sign = (event->delta() > 0)? precision : -precision;
    if ((scaleIncr >= -1) && (scaleIncr <= 1)) {
      if (sign > 0)
	scaleIncr = 1.0;
      else
	scaleIncr = -1;
    }
    scaleIncr += sign;
    if ((scaleIncr >= -1) && (scaleIncr <= 1)) {
      if (sign > 0)
	scaleIncr = 1.0;
      else
	scaleIncr = -1;
    }
    if (scaleIncr >= 1)
      pixmapScale = scaleIncr;
    else
      pixmapScale = fabs(1 / scaleIncr);
    update();
  }

  void Window::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      lastDragPos = event->pos();
      QCursor qc(Qt::SizeAllCursor);
      setCursor(qc);
    }
  }

  void Window::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
      pixmapOffset += event->pos() - lastDragPos;
      lastDragPos = event->pos();
      update();
    }
  }

  void Window::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      pixmapOffset += event->pos() - lastDragPos;
      lastDragPos = QPoint();
      
      int deltaX = (width() - pixmap->width()) / 2 - pixmapOffset.x();
      int deltaY = (height() - pixmap->height()) / 2 - pixmapOffset.y();
      scroll(deltaX, deltaY);
      QCursor qc(Qt::ArrowCursor);
      setCursor(qc);
      update();
    }
  }

  void Window::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape)
      close();
    else if (event->key() == Qt::Key_Left) {
      cout << "key left" << endl;
      if (scrollbox) {
	cout << "display_previous" << endl;
	scrollbox->display_previous();
      }
    }
    else if (event->key() == Qt::Key_Right) {
      cout << "key right" << endl;
      if (scrollbox) {
	cout << "display_next" << endl;
	scrollbox->display_next();
      }
    }
  }

  void Window::scroll_previous() {
    if (scrollbox)
      scrollbox->display_previous();
    else {
    //    if (scrollbox- != sb) {
      cerr << "sender: " << sender() << endl;
      cerr << "warning: trying to call operation on a scroll_box object ";
      cerr << "that no longer exists.";
    }    
  }

  void Window::scroll_next() {
    if (scrollbox)
      scrollbox->display_next();
    else {
      //    if (scrollbox != sb) {
      cerr << "sender: " << sender() << endl;
      cerr << "warning: trying to call operation on a scroll_box object ";
      cerr << "that no longer exists.";
    }    
  }

} // end namespace ebl
