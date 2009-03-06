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

#include "Window.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // Window

  Window::Window(int height, int width) {
    resize(width, height);
    mylabel = new QLabel(this);
    mylabel->setGeometry(QRect(0, 0, width, height));
    mylabel->setScaledContents(true);
    mydisplay = new QPixmap(width, height);
    QColor color;
    color.setRgb(255, 255, 255);
    mydisplay->fill(color.rgb());
    mylabel->setPixmap(*mydisplay);
    painter = new QPainter(mydisplay);
    show();
  }

  Window::~Window() {
  }

  void Window::resizeEvent (QResizeEvent *event){
    resize(event->size());
    mylabel->resize(event->size());
  }

  void Window::gray_draw_matrix(void* idx, idx_type type, int x, int y, 
				   int minv, int maxv, int zoomx, int zoomy, 
				   QMutex* mutex){
    if(mutex != NULL) mutex->lock();
    int order = static_cast<Idx<double>*>(idx)->order();
    if(order < 2){
      ylerror("not designed for idx0 and idx1");
      if(mutex != NULL) mutex->unlock();
      return;
    }
    int height = (order == 2 )? static_cast<Idx<double>*>(idx)->dim(0): static_cast<Idx<double>*>(idx)->dim(0);
    int width = (order == 2 )? static_cast<Idx<double>*>(idx)->dim(1): static_cast<Idx<double>*>(idx)->dim(1);
    Idx<ubyte>* mycopy = new Idx<ubyte>(height, width);

    int min, max;
    if((minv == 0)&&(maxv == 0)){
      min = idx_min(*mycopy);
      max = idx_max(*mycopy);
    } else {
      min = minv;
      max = maxv;
    }

    if(type == DOUBLE){
      Idx<double> test1(height, width);
      if(order==2) idx_copy(*(static_cast<Idx<double>*>(idx)), test1);
      else {
	Idx<double> bla = static_cast<Idx<double>*>(idx)->select(0,0);
	idx_copy(bla, test1);
      }
      idx_dotc( test1, (double)(255/(max-min)), test1);
      idx_addc( test1, (double)(-min*255/(max-min)), test1);
      idx_copy( test1, *mycopy);
    }
    if(type == FLOAT){
      Idx<float> test2(height, width);
      if(order==2) idx_copy(*(static_cast<Idx<float>*>(idx)), test2);
      else {
	Idx<float> bla = static_cast<Idx<float>*>(idx)->select(0,0);
	idx_copy(bla, test2);
      }
      idx_dotc( test2, (float)(255/(max-min)), test2);
      idx_addc( test2, (float)(-min*255/(max-min)), test2);
      idx_copy( test2, *mycopy);
    }
    if(type == INTG){
      if(order==2) idx_copy(*(static_cast<Idx<intg>*>(idx)), *mycopy);
      else {
	Idx<intg> test3 = static_cast<Idx<intg>*>(idx)->select(2,0);
	idx_copy(test3, *mycopy);
      }
      idx_dotc(*mycopy, (ubyte)(255/(max-min)), *mycopy);
      idx_addc(*mycopy, (ubyte)(-min*255/(max-min)), *mycopy);
    }
    if(type == UBYTE){
      if(order==2) idx_copy(*(static_cast<Idx<ubyte>*>(idx)), *mycopy);
      else {
	Idx<ubyte> test4 = static_cast<Idx<ubyte>*>(idx)->select(2,0);
	idx_copy(test4, *mycopy);
      }
      idx_dotc(*mycopy, (ubyte)(255/(max-min)), *mycopy);
      idx_addc(*mycopy, (ubyte)(-min*255/(max-min)), *mycopy);
    }
    if(mutex != NULL) mutex->unlock();

    QImage* image;
    QVector<QRgb> table(256);
    for (int i = 0; i < 256; i++){
      table[i] = qRgb(i, i, i);
    }
    image = new QImage((uchar*)mycopy->idx_ptr(), width, height, width * sizeof(unsigned char), QImage::Format_Indexed8);
    image->setColorTable(table);
    //    image = new QImage(image->scaled(width*zoomx, height*zoomy));
    painter->drawImage(x, y, *image);

    mylabel->setPixmap(*mydisplay);
  }

  void Window::rgb_draw_matrix(void* idx, idx_type type, int x, int y, 
				  int zoomx, int zoomy, QMutex* mutex){
    Idx<double> *d = static_cast<Idx<double>*>(idx);
    if(mutex != NULL) mutex->lock();
    int order = d->order();
    if(order != 3){
      ylerror("designed for idx3 only !");
      if(mutex != NULL) mutex->unlock();
      return;
    }
    int width = d->dim(0);
    int height = d->dim(1);
    Idx<ubyte> myimage(height, width, 3);
    Idx<ubyte> mypartimage = myimage; // myimage.narrow(2,3,0);
    if(type == DOUBLE){
      Idx<double> test1 = static_cast<Idx<double>*>(idx)->narrow(2,3,0);
      idx_copy(test1, mypartimage);
    }
    if(type == FLOAT){
      Idx<float> test2 = static_cast<Idx<float>*>(idx)->narrow(2,3,0);
      idx_copy(test2, mypartimage);
    }
    if(type == INTG){
      Idx<intg> test3 = static_cast<Idx<intg>*>(idx)->narrow(2,3,0);
      idx_copy(test3, mypartimage);
    }
    if(type == UBYTE){
      Idx<ubyte> *test4 = static_cast<Idx<ubyte>*>(idx); //->narrow(2,3,0);
      idx_copy(*test4, mypartimage);
    }
    if(mutex != NULL) mutex->unlock();


    QImage* image;
    image = new QImage((uchar*)myimage.idx_ptr(), width, height, width * sizeof(unsigned char), QImage::Format_RGB32);
    //   image = new QImage(image->scaled(width*zoomx, height*zoomy).rgbSwapped());
    painter->drawImage(x, y, *image);

    mylabel->setPixmap(*mydisplay);
    show();
  }

} // end namespace ebl
