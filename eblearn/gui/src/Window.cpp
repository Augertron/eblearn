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

  RenderThread *window = NULL;

  Window::Window(int argc, char** argv, int height, int width) 
    : pixmapScale(1.0), curScale(1.0), scaleIncr(1), 
      thread(argc, argv) {
    pixmap = new QPixmap(width, height);
    resize(width, height);
    qRegisterMetaType<QImage>("QImage");
    connect(&thread, SIGNAL(renderedImage()), this, SLOT(updatePixmap()));
    connect(&thread, SIGNAL(appquit()), this, SLOT(appquit()));
  }

  Window::~Window() {
    delete pixmap;
  }

  void Window::appquit() {
    exit(0);
  }

  void Window::updatePixmap() {
    if (thread.mutex->tryLock(MUTEX_WAIT_MSEC)) {
      QImage *im = thread.qimage;
      if (im) {
	if ((pixmap->width() != im->width()) 
	    || (pixmap->height() != im->height())) {
	  delete pixmap;
	  pixmap = new QPixmap(im->width(), im->height());
	}
	*pixmap = QPixmap::fromImage(*im);
      }
      thread.mutex->unlock();
    } else lock_failed_warning();
    show();
    update();
  }

  void Window::paintEvent(QPaintEvent * /* event */) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    if (curScale == pixmapScale) {
      painter.drawPixmap(pixmapOffset, *pixmap);
    } else {
      double scaleFactor = pixmapScale / curScale;
      painter.save();
      painter.translate(pixmapOffset);
      painter.scale(scaleFactor, scaleFactor);
      QRectF exposed = painter.matrix().inverted().mapRect(rect())
	.adjusted(-1, -1, 1, 1);
      painter.drawPixmap(exposed, *pixmap, exposed);
      painter.restore();
    }
     
    QString text = tr("Use mouse wheel to zoom, left click to drag.");
    QFontMetrics metrics = painter.fontMetrics();
    int textWidth = metrics.width(text);
     
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 127));
    painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10,
		     metrics.lineSpacing() + 5);
    painter.setPen(Qt::white);
    painter.drawText((width() - textWidth) / 2,
		     metrics.leading() + metrics.ascent(), text);
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
