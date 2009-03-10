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

#include "Window.moc"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // Window

  Window::Window(unsigned int wid, const char *wname, int height, int width) 
    : pixmapScale(1.0), curScale(1.0), scaleIncr(1), colorTable(256),
      text(""), silent(false), id(wid), savefname("") {
    setAttribute(Qt::WA_DeleteOnClose);
    if (wname) {
      QString q(wname);
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
    text_h0 = 0;
    text_w0 = 0;
  }

  Window::~Window() {
    delete pixmap;
    if (buffer)
      delete buffer;
    if (qimage)
      delete qimage;
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

  void Window::addText(const std::string *s) {
    text += *s;
    delete s;
    update_window(false);
  }
  
  void Window::set_text_origin(unsigned int h0, unsigned int w0) {
    text_h0 = h0;
    text_w0 = w0;
  }

  void Window::buffer_resize(int h, int w) {
    resize(w, h);
    if (!buffer) {
      buffer = new Idx<ubyte>(h, w);
      idx_fill(*buffer, (ubyte) 255);
    }
    else {
      Idx<ubyte> *inew = new Idx<ubyte>(h, w);
      idx_fill(*inew, (ubyte) 255);
      Idx<ubyte> tmpnew = inew->narrow(0, MIN(h, buffer->dim(0)), 0);
      tmpnew = tmpnew.narrow(1, MIN(w, buffer->dim(1)), 0);
      Idx<ubyte> tmpbuf = buffer->narrow(0, MIN(h, buffer->dim(0)), 0);
      tmpbuf = tmpbuf.narrow(1, MIN(w, buffer->dim(1)), 0);
      idx_copy(tmpbuf, tmpnew);
      delete buffer;
      buffer = inew;
    }
    if (qimage)
      delete qimage;
    qimage = new QImage((unsigned char*) buffer->idx_ptr(), 
			buffer->dim(1), buffer->dim(0), 
			buffer->dim(1) * sizeof (unsigned char),
			QImage::Format_Indexed8);
    qimage->setColorTable(colorTable);
  }

  void Window::updatePixmap(Idx<ubyte> *img, unsigned int h0, unsigned int w0) {
    if (img) {
      unsigned int h = MAX(buffer?(unsigned int)buffer->dim(0):0, 
			   h0 + img->dim(0));
      unsigned int w = MAX(buffer?(unsigned int)buffer->dim(1):0, 
			   w0 + img->dim(1));
      if (!buffer)
	buffer_resize(h, w);
      else if ((h > (unsigned int) buffer->dim(0)) || 
	       (w > (unsigned int) buffer->dim(1)))
	buffer_resize(h, w);
      Idx<ubyte> tmpbuf = buffer->narrow(0, img->dim(0), h0);
      tmpbuf = tmpbuf.narrow(1, img->dim(1), w0);
      idx_copy(*img, tmpbuf);
      // copy buffer to pixmap
      *pixmap = QPixmap::fromImage(*qimage);
      delete img;
      update_window();
    }
  }

  void Window::update_window(bool activate) {
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
  
  void Window::clear() {
    delete buffer;
    delete qimage;
    buffer = NULL;
    qimage = NULL;
    if (pixmap) 
      pixmap->fill(Qt::white);
  }

  void Window::paintEvent(QPaintEvent * /* event */) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    if (curScale == pixmapScale) {
      painter.drawPixmap(pixmapOffset, *pixmap);
      drawText(painter);
    } 
    else {
      double scaleFactor = pixmapScale / curScale;
      painter.save();
      painter.translate(pixmapOffset);
      painter.scale(scaleFactor, scaleFactor);
      QRectF exposed = 
	painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
      painter.drawPixmap(exposed, *pixmap, exposed);
      drawText(painter);
      painter.restore();
    }

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

  void Window::drawText(QPainter &painter) {
    QString txt(text.c_str());
    QRectF bg;
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignLeft & Qt::TextWordWrap & Qt::AlignTop,
		     txt, &bg);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 127));
    painter.drawRect(bg);
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignLeft & Qt::TextWordWrap & Qt::AlignTop,
		     txt, &bg);
  }

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

} // end namespace ebl
