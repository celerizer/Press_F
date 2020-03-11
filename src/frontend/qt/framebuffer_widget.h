#ifndef PRESS_F_FRAMEBUFFER_WIDGET_H
#define PRESS_F_FRAMEBUFFER_WIDGET_H

#include <QWidget>

#include "main.h"

class QPfFramebuffer : public QWidget
{
   Q_OBJECT

public:
   QPfFramebuffer(QWidget *parent);

   QSize getSize(void);
   bool  setScale(const QSize& parent);

protected:
   QSize minimumSizeHint(void) const override;
   void  paintEvent(QPaintEvent *event) override;

private:
   QRect  m_Rect;
   u8     m_Scale;
   QImage m_Texture;
};

#endif
